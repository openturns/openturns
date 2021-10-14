//                                               -*- C++ -*-
/**
 *  @brief The Normal distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cstdlib>
#include <cmath>

#include "openturns/Normal.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/MatrixImplementation.hxx"
#include "openturns/NormalCopula.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/GaussKronrodRule.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Normal)

static const Factory<Normal> Factory_Normal;

/* Constructor for multiD standard normal distribution */
Normal::Normal(const UnsignedInteger dimension)
  : EllipticalDistribution(Point(dimension, 0.0)
                           , Point(dimension, 1.0)
                           , CorrelationMatrix(dimension),
                           1.0)
  , logNormalizationFactor_((-1.0 * dimension) * SpecFunc::LOGSQRT2PI)
  , hasIndependentCopula_(true)
{
  setName("Normal");
  // Compute the range, the upper class cannot do it.
  computeRange();
}

/* Constructor for 1D normal distribution */
Normal::Normal(const Scalar mu,
               const Scalar sd)
  : EllipticalDistribution(Point(1, mu)
                           , Point(1, sd)
                           , CorrelationMatrix(1)
                           , 1.0)
  , logNormalizationFactor_(-SpecFunc::LOGSQRT2PI)
  , hasIndependentCopula_(true)
{
  setName("Normal");
  // Compute the range, the upper class cannot do it.
  computeRange();
}

/* Constructor for multiD normal distribution */
Normal::Normal(const Point & mean,
               const Point & sigma,
               const CorrelationMatrix & R)
  : EllipticalDistribution(mean
                           , sigma
                           , R
                           , 1.0)
  , logNormalizationFactor_((-1.0 * mean.getDimension()) * SpecFunc::LOGSQRT2PI)
  , hasIndependentCopula_(false)
{
  setName("Normal");
  // Compute the range, the upper class cannot do it.
  computeRange();
  checkIndependentCopula();
}

Normal::Normal(const Point & mean,
               const Point & sigma)
  : Normal(mean, sigma, CorrelationMatrix(mean.getDimension()))
{
  // Nothing to do
}

Normal::Normal(const Point & mean,
               const CovarianceMatrix & C)
  : EllipticalDistribution(mean
                           , Point(mean.getDimension(), 1.0)
                           , CorrelationMatrix(mean.getDimension())
                           , 1.0)
  , logNormalizationFactor_((-1.0 * mean.getDimension()) * SpecFunc::LOGSQRT2PI)
  , hasIndependentCopula_(false)
{
  setName("Normal");
  UnsignedInteger dimension = mean.getDimension();
  if (C.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the mean vector and the covariance matrix have incompatible dimensions";
  if (!C.isPositiveDefinite()) throw InvalidArgumentException(HERE) << "Error: the covariance matrix is not positive definite";
  Point sigma(dimension);
  CorrelationMatrix R(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    sigma[i] = std::sqrt(C(i, i));
    for (UnsignedInteger j = 0; j < i; ++j) R(i, j) = C(i, j) / (sigma[i] * sigma[j]);
  }
  // To check that the values are > 0. This call also compute the range.
  setSigma(sigma);
  setCorrelation(R);
  checkIndependentCopula();
}

/* Comparison operator */
Bool Normal::operator ==(const Normal & other) const
{
  if (this == &other) return true;
  return EllipticalDistribution::equals(other);
}

/* String converter */
String Normal::__repr__() const
{
  OSS oss;
  oss << "class=" << Normal::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " mean=" << mean_
      << " sigma=" << sigma_
      << " correlationMatrix=" << R_.__repr__();
  return oss;
}

String Normal::__str__(const String & offset) const
{
  OSS oss;
  oss << getClassName();
  if (getDimension() == 1) oss << "(mu = " << getMean()[0] << ", sigma = " << getSigma()[0] << ")";
  else oss << "(mu = " << getMean().__str__() << ", sigma = " << getSigma().__str__() << ", R = " << getCorrelation().__str__(offset) << ")";
  return oss;
}

/* Virtual constructor */
Normal * Normal::clone() const
{
  return new Normal(*this);
}

/* Get one realization of the distribution */
Point Normal::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1) return Point(1, mean_[0] + sigma_[0] * DistFunc::rNormal());
  Point value(dimension);
  // First, a realization of independant standard coordinates
  for (UnsignedInteger i = 0; i < dimension; i++) value[i] = DistFunc::rNormal();
  // Then, transform the independant standard coordinates into the needed ones */
  if (hasIndependentCopula_)
  {
    for (UnsignedInteger i = 0; i < dimension; i++)
    {
      value[i] *= sigma_[i];
      value[i] += mean_[i];
    }
    return value;
  }
  // General case
  return cholesky_ * value + mean_;
}

Sample Normal::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger dimension = getDimension();
  Sample result(size, dimension);
  if (dimension == 1)
    result.getImplementation()->setData(sigma_[0] * DistFunc::rNormal(size));
  else
    {
      for (UnsignedInteger i = 0; i < size; ++i)
        for (UnsignedInteger j = 0; j < dimension; ++j) result(i, j) = DistFunc::rNormal();
      if (hasIndependentCopula_) result *= sigma_;
      else result = cholesky_.getImplementation()->genSampleProd(result, true, false, 'R');
    }
  result += mean_;
  result.setName(getName());
  result.setDescription(getDescription());
  return result;
}

/* Compute the density generator of the ellipticalal generator, i.e.
 *  the function phi such that the density of the distribution can
 *  be written as p(x) = phi(t(x-mu)S^(-1)(x-mu))                      */
Scalar Normal::computeDensityGenerator(const Scalar betaSquare) const
{
  return std::exp(logNormalizationFactor_ - 0.5 * betaSquare);
}

Scalar Normal::computeLogDensityGenerator(const Scalar betaSquare) const
{
  return logNormalizationFactor_ - 0.5 * betaSquare;
}

/* Compute the derivative of the density generator */
Scalar Normal::computeDensityGeneratorDerivative(const Scalar betaSquare) const
{
  return -0.5 * std::exp(logNormalizationFactor_ - 0.5 * betaSquare);
}

/* Compute the seconde derivative of the density generator */
Scalar Normal::computeDensityGeneratorSecondDerivative(const Scalar betaSquare) const
{
  return 0.25 * std::exp(logNormalizationFactor_ - 0.5 * betaSquare);
}

/* Get the PDF of the distribution */
Scalar Normal::computePDF(const Scalar x) const
{
  const Scalar y = (x - mean_[0]) / sigma_[0];
  return DistFunc::dNormal(y) / sigma_[0];
}

Scalar Normal::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has a dimension incompatible with the distribution.";
  // Special case for dimension 1
  if (dimension == 1) return computePDF(point[0]);
  return EllipticalDistribution::computePDF(point);
}

/* Get the CDF of the distribution */
Scalar Normal::computeCDF(const Scalar x) const
{
  return DistFunc::pNormal((x - mean_[0]) / sigma_[0]);
}

Scalar Normal::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has a dimension incompatible with the distribution.";
  // Special case for dimension 1
  if (dimension == 1) return computeCDF(point[0]);
  // Normalize the point to use the standard form of the multivariate normal distribution
  Point u(normalize(point));
  /* Special treatment for independent components */
  if (hasIndependentCopula_)
  {
    Scalar value = DistFunc::pNormal(u[0]);
    for (UnsignedInteger i = 1; i < dimension; ++i) value *= DistFunc::pNormal(u[i]);
    return value;
  }
  // General multivariate case
  const Point lowerBounds(getRange().getLowerBound());
  const Point upperBounds(getRange().getUpperBound());
  // Indices of the components to take into account in the computation
  Indices toKeep(0);
  Point reducedPoint(0);
  for (UnsignedInteger k = 0; k < dimension; ++ k)
  {
    const Scalar xK = point[k];
    // Early exit if one component is less than its corresponding range lower bound
    if (xK <= lowerBounds[k]) return 0.0;
    // Keep only the indices for which xK is less than its corresponding range upper bound
    // Marginalize the others
    if (xK < upperBounds[k])
    {
      toKeep.add(k);
      reducedPoint.add(xK);
    }
  } // k
  // The point has all its components greater than the corresponding range upper bound
  LOGINFO(OSS() << "In Normal::computeCDF, point=" << point << ", toKeep=" << toKeep << ", dimension=" << dimension << ", reducedPoint=" << reducedPoint);
  if (toKeep.getSize() == 0) return 1.0;
  // The point has some components greater than the corresponding range upper bound
  if (toKeep.getSize() != dimension) return getMarginal(toKeep).computeCDF(reducedPoint);
  /* General case */
  // For the bidimensional case, use specialized high precision routine
  if (dimension == 2) return DistFunc::pNormal2D(u[0], u[1], R_(0, 1));
  // For the tridimensional case, use specialized high precision routine
  if (dimension == 3) return DistFunc::pNormal3D(u[0], u[1], u[2], R_(0, 1), R_(0, 2), R_(1, 2));
  // For moderate dimension, use a Gauss-Kronrod integration. We use a non-adaptive
  // integration based on Kronrod's nodes only.
  if (dimension <= ResourceMap::GetAsUnsignedInteger("Normal-SmallDimension"))
  {
    GaussKronrodRule rule;
    switch (dimension)
    {
      case 4:
        rule = GaussKronrodRule::G15K31;
        break;
      case 5:
        rule = GaussKronrodRule::G11K23;
        break;
      case 6:
        rule = GaussKronrodRule::G7K15;
        break;
      default:
        LOGWARN(OSS() << "The dimension=" << dimension << " of the Normal distribution is large for Gauss quadrature! Expect a high computational cost and a reduced accuracy for CDF evaluation.");
        rule = GaussKronrodRule::G7K15;
        break;
    }
    Point kronrodWeights(1, rule.getZeroKronrodWeight());
    kronrodWeights.add(rule.getOtherKronrodWeights());
    kronrodWeights.add(rule.getOtherKronrodWeights());
    Point kronrodNodes(1, 0.0);
    kronrodNodes.add(rule.getOtherKronrodNodes());
    kronrodNodes.add(rule.getOtherKronrodNodes() * (-1.0));
    // Perform the integration
    const UnsignedInteger marginalNodesNumber = kronrodNodes.getDimension();
    const UnsignedInteger size = static_cast< UnsignedInteger >(round(std::pow(1.0 * marginalNodesNumber, static_cast<int>(dimension))));
    Indices indices(dimension, 0);
    Sample allNodes(size, dimension);
    Point allWeights(size);
    for (UnsignedInteger linearIndex = 0; linearIndex < size; ++linearIndex)
    {
      Point node(dimension);
      Scalar weight = 1.0;
      for (UnsignedInteger j = 0; j < dimension; ++j)
      {
        const UnsignedInteger indiceJ = indices[j];
        const Scalar delta = 0.5 * (reducedPoint[j] - lowerBounds[j]);
        node[j] = lowerBounds[j] + delta * (1.0 + kronrodNodes[indiceJ]);
        weight *= delta * kronrodWeights[indiceJ];
      }
      allNodes[linearIndex] = node;
      allWeights[linearIndex] = weight;
      /* Update the indices */
      ++indices[0];
      /* Propagate the remainders */
      for (UnsignedInteger j = 0; j < dimension - 1; ++j) indices[j + 1] += (indices[j] == marginalNodesNumber);
      /* Correction of the indices. The last index cannot overflow. */
      for (UnsignedInteger j = 0; j < dimension - 1; ++j) indices[j] = indices[j] % marginalNodesNumber;
    } // Loop over the n-D nodes
    // Parallel evalusation of the PDF
    const Sample allPDF(computePDF(allNodes));
    // Some black magic to use BLAS on the internal representation of samples
    const Scalar probability = allWeights.dot(allPDF.getImplementation()->getData());
    return probability;
  }
  // For very large dimension, use a MonteCarlo algorithm
  LOGWARN(OSS() << "Warning, in Normal::computeCDF(), the dimension is very high. We will use a Monte Carlo method for the computation with a relative precision of 0.1% at 99% confidence level and a maximum of " << 10 * ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ) << " realizations. Expect a long running time and a poor accuracy for small values of the CDF...");
  RandomGeneratorState initialState(RandomGenerator::GetState());
  RandomGenerator::SetSeed(ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ));
  Scalar value = 0.0;
  Scalar variance = 0.0;
  Scalar a99 = DistFunc::qNormal(0.995);
  const UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" );
  UnsignedInteger outerMax = 10 * ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ) / blockSize;
  Scalar precision = 0.0;
  for (UnsignedInteger indexOuter = 0; indexOuter < outerMax; ++indexOuter)
  {
    const Sample sample(getSample(blockSize));
    LOGDEBUG(OSS(false) << "indexOuter=" << indexOuter << ", point=" << point << ", sample=" << sample);
    const Scalar valueBlock = sample.computeEmpiricalCDF(point);
    const Scalar varianceBlock = valueBlock * (1.0 - valueBlock) / blockSize;
    LOGDEBUG(OSS(false) << "valueBlock=" << valueBlock << ", varianceBlock=" << varianceBlock);
    const Scalar norm = 1.0 / (indexOuter + 1.0);
    variance = (varianceBlock + indexOuter * variance + (1.0 - norm) * (value - valueBlock) * (value - valueBlock)) * norm;
    value = (value * indexOuter + valueBlock) * norm;
    LOGDEBUG(OSS(false) << "value=" << value << ", variance=" << variance);
    // Quick return for value = 1
    if ((value >= 1.0 - ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon")) && (variance == 0.0)) return 1.0;
    precision = a99 * std::sqrt(variance / (indexOuter + 1.0) / ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ));
    if (precision < ResourceMap::GetAsScalar( "Normal-MinimumCDFEpsilon" ) * value) return value;
    // 0.1 * ((1000 * indexOuter) / outerMax) is to print percents with one figure after the decimal point
    LOGINFO(OSS() << 0.1 * ((1000 * indexOuter) / outerMax) << "% value=" << value << " absolute precision(99%)=" << precision << " relative precision(99%)=" << ((value > 0.0) ? precision / value : -1.0));
  }
  RandomGenerator::SetState(initialState);
  return value;
} // computeCDF

Sample Normal::computeCDF(const Sample & sample) const
{
  if (dimension_ <= ResourceMap::GetAsUnsignedInteger("Normal-SmallDimension"))
    return DistributionImplementation::computeCDFParallel(sample);
  return DistributionImplementation::computeCDFSequential(sample);
}

Scalar Normal::computeComplementaryCDF(const Scalar x) const
{
  return DistFunc::pNormal((x - mean_[0]) / sigma_[0], true);
}

Scalar Normal::computeComplementaryCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has a dimension incompatible with the distribution.";
  // Special case for dimension 1
  if (dimension == 1) return computeComplementaryCDF(point[0]);
  return EllipticalDistribution::computeComplementaryCDF(point);
} // computeComplementaryCDF

/* Compute the entropy of the distribution */
Scalar Normal::computeEntropy() const
{
  // EllipticalDistribution::normalizationFactor_ == 1/sqrt(det(Sigma))
  // logNormalizationFactor_ == log(1/sqrt(2*Pi)^dim)
  return 0.5 * getDimension() - std::log(EllipticalDistribution::normalizationFactor_) - logNormalizationFactor_;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Normal::computeCharacteristicFunction(const Scalar x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

Complex Normal::computeCharacteristicFunction(const Point & x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

Complex Normal::computeLogCharacteristicFunction(const Scalar x) const
{
  return Complex(-0.5 * sigma_[0] * sigma_[0] * x * x, mean_[0] * x);
}

Complex Normal::computeLogCharacteristicFunction(const Point & x) const
{
  if (x.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << x.getDimension();
  return Complex(-0.5 * x.dot(getCovariance() * x), x.dot(mean_));
}

/* Compute the probability content of an interval */
Scalar Normal::computeProbability(const Interval & interval) const
{
  if (interval.isEmpty()) return 0.0;
  const UnsignedInteger dimension = getDimension();
  // The generic implementation provided by the DistributionImplementation upper class is more accurate than the generic implementation provided by the ContinuousDistribution upper class for dimension = 1
  if (dimension == 1) return DistributionImplementation::computeProbability(interval);
  // Decompose and normalize the interval
  Point lower(normalize(interval.getLowerBound()));
  Point upper(normalize(interval.getUpperBound()));
  Interval::BoolCollection finiteLower(interval.getFiniteLowerBound());
  Interval::BoolCollection finiteUpper(interval.getFiniteUpperBound());
  /* Special treatment for independent components */
  if (hasIndependentCopula_)
  {
    Scalar lowerCDF = 0.0;
    if (finiteLower[0]) lowerCDF = DistFunc::pNormal(lower[0]);
    Scalar upperCDF = 1.0;
    if (finiteUpper[0]) upperCDF = DistFunc::pNormal(upper[0]);
    Scalar value = upperCDF - lowerCDF;
    for (UnsignedInteger i = 1; i < dimension; ++i)
    {
      lowerCDF = 0.0;
      if (finiteLower[i]) lowerCDF = DistFunc::pNormal(lower[i]);
      upperCDF = 1.0;
      if (finiteUpper[i]) upperCDF = DistFunc::pNormal(upper[i]);
      value *= upperCDF - lowerCDF;
    }
    return value;
  }
  /* General case */
  // For moderate dimension, use a Gauss-Legendre integration
  if (dimension <= ResourceMap::GetAsUnsignedInteger("Normal-SmallDimension"))
  {
    // Reduce the default integration point number for CDF computation in the range 3 < dimension <= Normal-SmallDimension
    const UnsignedInteger maximumNumber = static_cast< UnsignedInteger > (round(std::pow(ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ), 1.0 / getDimension())));
    const UnsignedInteger candidateNumber = ResourceMap::GetAsUnsignedInteger( "Normal-MarginalIntegrationNodesNumber" );
    if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of PDF evaluations. It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"Normal-MaximumNumberOfPoints\"");
    setIntegrationNodesNumber(std::min(maximumNumber, candidateNumber));
    return ContinuousDistribution::computeProbability(interval);
  }
  // For very large dimension, use a MonteCarlo algorithm
  LOGWARN(OSS() << "Warning, in Normal::computeProbability(), the dimension is very high. We will use a Monte Carlo method for the computation with a relative precision of 0.1% at 99% confidence level and a maximum of " << 10 * ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ) << " realizations. Expect a long running time and a poor accuracy for low values of the CDF...");
  Scalar value = 0.0;
  Scalar variance = 0.0;
  Scalar a99 = DistFunc::qNormal(0.995);
  UnsignedInteger outerMax = 10 * ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ) / ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" );
  Scalar precision = 0.0;
  for (UnsignedInteger indexOuter = 0; indexOuter < outerMax; ++indexOuter)
  {
    Scalar valueBlock = 0.0;
    Scalar varianceBlock = 0.0;
    for (UnsignedInteger indexSample = 0; indexSample < ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ); ++indexSample)
    {
      // ind value is 1.0 if the realization is inside of the integration domain, 0.0 else.
      Scalar ind = interval.numericallyContains(getRealization());
      Scalar norm = 1.0 / (indexSample + 1.0);
      varianceBlock = (varianceBlock * indexSample + (1.0 - norm) * (valueBlock - ind) * (valueBlock - ind)) * norm;
      valueBlock = (valueBlock * indexSample + ind) * norm;
    }
    Scalar norm = 1.0 / (indexOuter + 1.0);
    variance = (varianceBlock + indexOuter * variance + (1.0 - norm) * (value - valueBlock) * (value - valueBlock)) * norm;
    value = (value * indexOuter + valueBlock) * norm;
    // Quick return for value = 1
    if ((value >= 1.0 - ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon")) && (variance == 0.0)) return 1.0;
    precision = a99 * std::sqrt(variance / (indexOuter + 1.0) / ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ));
    if (precision < ResourceMap::GetAsScalar( "Normal-MinimumCDFEpsilon" ) * value) return value;
    // 0.1 * ((1000 * indexOuter) / outerMax) is to print percents with one figure after the decimal point
    LOGINFO(OSS() << 0.1 * ((1000 * indexOuter) / outerMax) << "% value=" << value << " absolute precision(99%)=" << precision << " relative precision(99%)=" << ((value > 0.0) ? precision / value : -1.0));
  }
  return value;
}

/* Get the CDF gradient of the distribution */
Point Normal::computeCDFGradient(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  Point gradientCDF(2 * dimension);
  if (dimension == 1)
  {
    Scalar pdf = computePDF(point);
    gradientCDF[0] = -pdf;
    gradientCDF[1] = -pdf * (point[0] - mean_[0]) / sigma_[0];
    return gradientCDF;
  }
  else
    gradientCDF = EllipticalDistribution::computeCDFGradient(point);
  return gradientCDF;
}

/* Compute the scalar quantile of the 1D normal distribution */
Scalar Normal::computeScalarQuantile(const Scalar prob,
                                     const Bool tail) const
{
  if (dimension_ != 1) throw InvalidDimensionException(HERE) << "Error: the method computeScalarQuantile is only defined for 1D distributions";
  return mean_[0] + sigma_[0] * DistFunc::qNormal(prob, tail);
} // computeScalarQuantile

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1)
   For Normal distribution, the conditional distribution is also Normal, with mean and covariance
   such as:
   mean_cond = mean(x) + cov(x, y).cov(y, y)^(-1)(y - mean(y))
   cov_cond = cov(x, x) - cov(x, y).cov(y, y)^(-1)cov(x, y)
   This expression simplifies if we use the inverse of the Cholesky factor of the covariance matrix.
   See [Lebrun, Dutfoy, "Rosenblatt and Nataf transformation"]
*/
Scalar Normal::computeConditionalPDF(const Scalar x,
                                     const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
  {
    const Scalar z = (x - mean_[conditioningDimension]) / sigma_[conditioningDimension];
    // Interest is to compute \sqrt{\frac{1}{2 \pi}} exp(-z*z)
    // With x = \sqrt{2} z, we use the Gaussian density function
    return DistFunc::dNormal(z * std::sqrt(2.0)) / sigma_[conditioningDimension];
  }
  // General case
  Scalar meanRos = 0.0;
  const Scalar sigmaRos = 1.0 / inverseCholesky_(conditioningDimension, conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    meanRos += inverseCholesky_(conditioningDimension, i) / std::sqrt(sigma_[i]) * (y[i] - mean_[i]);
  }
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  const Scalar z = (x - meanRos) / sigmaRos;
  return DistFunc::dNormal(z) / sigmaRos;
}

Point Normal::computeSequentialConditionalPDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional PDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  Point result(dimension_);
  if (hasIndependentCopula())
    for (UnsignedInteger i = 0; i < dimension_; ++i)
    {
      const Scalar u = (x[i] - mean_[i]) / sigma_[i];
      result[i] = DistFunc::dNormal(u) / sigma_[i];
    }
  else
  {
    const Point u(inverseCholesky_ * (x - mean_));
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = DistFunc::dNormal(u[i]) * inverseCholesky_(i, i);
  }
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Normal::computeConditionalCDF(const Scalar x,
                                     const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula()))
    return DistFunc::pNormal((x - mean_[conditioningDimension]) / sigma_[conditioningDimension]);
  // General case
  Scalar meanRos = 0.0;
  const Scalar sigmaRos = 1.0 / inverseCholesky_(conditioningDimension, conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    meanRos += inverseCholesky_(conditioningDimension, i) / std::sqrt(sigma_[i]) * (y[i] - mean_[i]);
  }
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  return DistFunc::pNormal((x - meanRos) / sigmaRos);
}

Point Normal::computeSequentialConditionalCDF(const Point & x) const
{
  if (x.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional CDF with an argument of dimension=" << x.getDimension() << " different from distribution dimension=" << dimension_;
  if (hasIndependentCopula())
  {
    Point result(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = DistFunc::pNormal((x[i] - mean_[i]) / sigma_[i]);
    return result;
  }
  return DistFunc::pNormal(inverseCholesky_ * (x - mean_));
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar Normal::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return mean_[conditioningDimension] + sigma_[conditioningDimension] * DistFunc::qNormal(q);
  // General case
  Scalar meanRos = 0.0;
  const Scalar sigmaRos = 1.0 / inverseCholesky_(conditioningDimension, conditioningDimension);
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    meanRos += inverseCholesky_(conditioningDimension, i) / std::sqrt(sigma_[i]) * (y[i] - mean_[i]);
  }
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  return meanRos + sigmaRos * DistFunc::qNormal(q);
}

Point Normal::computeSequentialConditionalQuantile(const Point & q) const
{
  if (q.getDimension() != dimension_) throw InvalidArgumentException(HERE) << "Error: cannot compute sequential conditional quantile with an argument of dimension=" << q.getDimension() << " different from distribution dimension=" << dimension_;
  if (hasIndependentCopula())
  {
    Point result(dimension_);
    for (UnsignedInteger i = 0; i < dimension_; ++i)
      result[i] = mean_[i] + sigma_[i] * DistFunc::qNormal(q[i]);
    return result;
  }
  return mean_ + cholesky_ * DistFunc::qNormal(q);
}

/* Get the i-th marginal distribution */
Distribution Normal::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  CorrelationMatrix R(1);
  Point sigma(1, sigma_[i]);
  Point mean(1, mean_[i]);
  Normal::Implementation marginal(new Normal(mean, sigma, R));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution Normal::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  CorrelationMatrix R(outputDimension);
  Point sigma(outputDimension);
  Point mean(outputDimension);
  Description description(getDescription());
  Description marginalDescription(outputDimension);
  // Extract the correlation matrix, the marginal standard deviations and means
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i = indices[i];
    sigma[i] = sigma_[index_i];
    mean[i] = mean_[index_i];
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      R(i, j) = R_(index_i, indices[j]);
    }
    marginalDescription[i] = description[index_i];
  }
  Normal::Implementation marginal(new Normal(mean, sigma, R));
  marginal->setDescription(marginalDescription);
  return marginal;
} // getMarginal(Indices)

/* Get the skewness of the distribution */
Point Normal::getSkewness() const
{
  return Point(getDimension(), 0.0);
}

/* Get the moments of the standardized distribution */
Point Normal::getStandardMoment(const UnsignedInteger n) const
{
  if (n % 2 == 1) return Point(1, 0.0);
  Scalar moment = 1.0;
  for (UnsignedInteger i = 1; i < n / 2; ++i)
    moment *= 2.0 * i + 1.0;
  return Point(1, moment);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Normal::getStandardRepresentative() const
{
  return Normal(0.0, 1.0);
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Normal::getRoughness() const
{
  // 0.2820947917738781434740398 = 1 / (2 * sqrt(Pi))
  if (dimension_ == 1)
    return 0.2820947917738781434740398 / getSigma()[0];

  Scalar roughness = 1.0;
  if (hasIndependentCopula())
  {
    for (UnsignedInteger d = 0; d < dimension_; ++d)
      roughness *= 0.2820947917738781434740398 / getSigma()[d];
    return roughness;
  }
  else
  {
    for (UnsignedInteger d = 0; d < dimension_; ++d)
      roughness *= 0.2820947917738781434740398 / cholesky_(d, d);
  }
  return roughness;
}

/* Get the kurtosis of the distribution */
Point Normal::getKurtosis() const
{
  return Point(getDimension(), 3.0);
}

/* Get the copula of the distribution */
Distribution Normal::getCopula() const
{
  return new NormalCopula(R_);
}

/* Compute the radial distribution CDF */
Scalar Normal::computeRadialDistributionCDF(const Scalar radius,
    const Bool tail) const
{
  return tail ? ChiSquare(getDimension()).computeComplementaryCDF(radius * radius) : ChiSquare(getDimension()).computeCDF(radius * radius);
}

/* Correlation matrix accessor */
void Normal::setCorrelation(const CorrelationMatrix & R)
{
  // Perform checks at the upper level
  // This call set also the range
  EllipticalDistribution::setCorrelation(R);
  // Then check for independence
  checkIndependentCopula();
}

/* Compute the numerical range of the distribution given the parameters values */
void Normal::computeRange()
{
  const UnsignedInteger dimension = getDimension();
  const Scalar qNorm = DistFunc::qNormal(cdfEpsilon_);
  // qNorm is negative as cdfEpsilon_ < 0
  setRange(Interval(mean_ + qNorm * sigma_, mean_ - qNorm * sigma_, Interval::BoolCollection(dimension, false), Interval::BoolCollection(dimension, false)));
}

/* Tell if the distribution has independent copula */
Bool Normal::hasIndependentCopula() const
{
  return hasIndependentCopula_;
}

/* Check if the distribution has independent copula */
void Normal::checkIndependentCopula()
{
  hasIndependentCopula_ = true;
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1) return;
  for (UnsignedInteger i = 0; i < dimension; i++)
  {
    for (UnsignedInteger j = 0; j < i; j++)
    {
      if (R_(i, j) != 0.0)
      {
        hasIndependentCopula_ = false;
        return;
      }
    }
  }
} // checkIndependentCopula

/* Method save() stores the object through the StorageManager */
void Normal::save(Advocate & adv) const
{
  EllipticalDistribution::save(adv);
  adv.saveAttribute( "logNormalizationFactor", logNormalizationFactor_ );
  adv.saveAttribute( "hasIndependentCopula_", hasIndependentCopula_ );
}

/* Method load() reloads the object from the StorageManager */
void Normal::load(Advocate & adv)
{
  EllipticalDistribution::load(adv);
  adv.loadAttribute( "logNormalizationFactor", logNormalizationFactor_ );
  adv.loadAttribute( "hasIndependentCopula_", hasIndependentCopula_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
