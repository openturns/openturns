//                                               -*- C++ -*-
/**
 *  @brief The Normal distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cstdlib>
#include <cmath>

#include "Normal.hxx"
#include "Distribution.hxx"
#include "ChiSquare.hxx"
#include "SpecFunc.hxx"
#include "Log.hxx"
#include "OSS.hxx"
#include "DistFunc.hxx"
#include "PersistentObjectFactory.hxx"
#include "Matrix.hxx"
#include "MatrixImplementation.hxx"
#include "IdentityMatrix.hxx"
#include "NormalCopula.hxx"
#include "ResourceMap.hxx"
#include "RandomGenerator.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Normal);

static Factory<Normal> RegisteredFactory("Normal");

/* Constructor for multiD standard normal distribution */
Normal::Normal(const UnsignedInteger dimension)
  : EllipticalDistribution(NumericalPoint(dimension, 0.0)
                           , NumericalPoint(dimension, 1.0)
                           , CorrelationMatrix(dimension),
                           1.0)
  , normalizationFactor_(1.0 / std::sqrt(std::pow(2.0 * M_PI, static_cast<int>(dimension))))
  , hasIndependentCopula_(true)
{
  setName("Normal");
  // Compute the range, the upper class cannot do it.
  computeRange();
}

/* Constructor for 1D normal distribution */
Normal::Normal(const NumericalScalar mu,
               const NumericalScalar sd)
  : EllipticalDistribution(NumericalPoint(1, mu)
                           , NumericalPoint(1, sd)
                           , CorrelationMatrix(1)
                           , 1.0)
  , normalizationFactor_(1.0 / std::sqrt(2 * M_PI))
  , hasIndependentCopula_(true)
{
  setName("Normal");
  // Compute the range, the upper class cannot do it.
  computeRange();
}

/* Constructor for multiD normal distribution */
Normal::Normal(const NumericalPoint & mean,
               const NumericalPoint & sigma,
               const CorrelationMatrix & R)
  : EllipticalDistribution(mean
                           , sigma
                           , R
                           , 1.0)
  , normalizationFactor_(1.0 / std::sqrt(std::pow(2.0 * M_PI, static_cast<int>(mean.getDimension()))))
  , hasIndependentCopula_(false)
{
  setName("Normal");
  // Compute the range, the upper class cannot do it.
  computeRange();
  checkIndependentCopula();
}

Normal::Normal(const NumericalPoint & mean,
               const CovarianceMatrix & C)
  : EllipticalDistribution(mean
                           , NumericalPoint(mean.getDimension(), 1.0)
                           , IdentityMatrix(mean.getDimension())
                           , 1.0)
  , normalizationFactor_(1.0 / std::sqrt(std::pow(2.0 * M_PI, static_cast<int>(mean.getDimension()))))
  , hasIndependentCopula_(false)
{
  setName("Normal");
  UnsignedInteger dimension(mean.getDimension());
  if (C.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the mean vector and the covariance matrix have incompatible dimensions";
  if (!const_cast<CovarianceMatrix*>(&C)->isPositiveDefinite()) throw InvalidArgumentException(HERE) << "Error: the covariance matrix is not positive definite";
  NumericalPoint sigma(dimension);
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
  oss << offset << getClassName();
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
NumericalPoint Normal::getRealization() const
{
  const UnsignedInteger dimension(getDimension());
  if (dimension == 1) return NumericalPoint(1, mean_[0] + sigma_[0] * DistFunc::rNormal());
  NumericalPoint value(dimension);
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

NumericalSample Normal::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger dimension(getDimension());
  NumericalSample result(size, dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j) result[i][j] = DistFunc::rNormal();
  if (hasIndependentCopula_) result *= sigma_;
  else result *= cholesky_;
  result += mean_;
  result.setName(getName());
  result.setDescription(getDescription());
  return result;
}

/* Compute the density generator of the ellipticalal generator, i.e.
 *  the function phi such that the density of the distribution can
 *  be written as p(x) = phi(t(x-mu)S^(-1)(x-mu))                      */
NumericalScalar Normal::computeDensityGenerator(const NumericalScalar betaSquare) const
{
  return normalizationFactor_ * std::exp(-0.5 * betaSquare);
}

/* Compute the derivative of the density generator */
NumericalScalar Normal::computeDensityGeneratorDerivative(const NumericalScalar betaSquare) const
{
  return -0.5 * normalizationFactor_ * std::exp(-0.5 * betaSquare);
}

/* Compute the seconde derivative of the density generator */
NumericalScalar Normal::computeDensityGeneratorSecondDerivative(const NumericalScalar betaSquare) const
{
  return 0.25 * normalizationFactor_ * std::exp(-0.5 * betaSquare);
}

/* Get the CDF of the distribution */
NumericalScalar Normal::computeCDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point has a dimension incompatible with the distribution.";
  // Special case for dimension 1
  if (dimension == 1) return DistFunc::pNormal((point[0] - mean_[0]) / sigma_[0]);
  // Normalize the point to use the standard form of the multivariate normal distribution
  NumericalPoint u(normalize(point));
  /* Special treatment for independent components */
  if (hasIndependentCopula_)
  {
    NumericalScalar value(DistFunc::pNormal(u[0]));
    for (UnsignedInteger i = 1; i < dimension; ++i) value *= DistFunc::pNormal(u[i]);
    return value;
  }
  /* General case */
  // For the bidimensional case, use specialized high precision routine
  if (dimension == 2) return DistFunc::pNormal2D(u[0], u[1], R_(0, 1));
  // For the tridimensional case, use specialized high precision routine
  if (dimension == 3) return DistFunc::pNormal3D(u[0], u[1], u[2], R_(0, 1), R_(0, 2), R_(1, 2));
  // For moderate dimension, use a Gauss-Legendre integration
  if (dimension <= ResourceMap::GetAsUnsignedInteger("Normal-SmallDimension"))
  {
    // Reduce the default integration point number for CDF computation in the range 3 < dimension <= Normal-SmallDimension
    const UnsignedInteger maximumNumber(static_cast< UnsignedInteger > (round(std::pow(ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ), 1.0 / getDimension()))));
    const UnsignedInteger candidateNumber(ResourceMap::GetAsUnsignedInteger( "Normal-MarginalIntegrationNodesNumber" ));
    if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of PDF evaluations. It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"Normal-MaximumNumberOfPoints\"");
    setIntegrationNodesNumber(std::min(maximumNumber, candidateNumber));
    return ContinuousDistribution::computeCDF(point);
  }
  // For very large dimension, use a MonteCarlo algorithm
  LOGWARN(OSS() << "Warning, in Normal::computeCDF(), the dimension is very high. We will use a Monte Carlo method for the computation with a relative precision of 0.1% at 99% confidence level and a maximum of " << 10 * ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ) << " realizations. Expect a long running time and a poor accuracy for small values of the CDF...");
  RandomGeneratorState initialState(RandomGenerator::GetState());
  RandomGenerator::SetSeed(ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ));
  NumericalScalar value(0.0);
  NumericalScalar variance(0.0);
  NumericalScalar a99(DistFunc::qNormal(0.995));
  UnsignedInteger outerMax(10 * ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ) / ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ));
  NumericalScalar precision(0.0);
  for (UnsignedInteger indexOuter = 0; indexOuter < outerMax; ++indexOuter)
  {
    NumericalScalar valueBlock(0.0);
    NumericalScalar varianceBlock(0.0);
    for (UnsignedInteger indexSample = 0; indexSample < ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ); ++indexSample)
    {
      Bool inside(true);
      NumericalPoint realization(getRealization());
      // Check if the realization is in the integration domain
      for (UnsignedInteger i = 0; i < dimension; ++i)
      {
        inside = realization[i] <= point[i];
        if (!inside) break;
      }
      // ind value is 1.0 if the realization is inside of the integration domain, 0.0 else.
      NumericalScalar ind(inside);
      NumericalScalar norm(1.0 / (indexSample + 1.0));
      varianceBlock = (varianceBlock * indexSample + (1.0 - norm) * (valueBlock - ind) * (valueBlock - ind)) * norm;
      valueBlock = (valueBlock * indexSample + ind) * norm;
    }
    NumericalScalar norm(1.0 / (indexOuter + 1.0));
    variance = (varianceBlock + indexOuter * variance + (1.0 - norm) * (value - valueBlock) * (value - valueBlock)) * norm;
    value = (value * indexOuter + valueBlock) * norm;
    // Quick return for value = 1
    if ((value >= 1.0 - ResourceMap::GetAsNumericalScalar("DistributionImplementation-DefaultQuantileEpsilon")) && (variance == 0.0)) return 1.0;
    precision = a99 * std::sqrt(variance / (indexOuter + 1.0) / ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ));
    if (precision < ResourceMap::GetAsNumericalScalar( "Normal-MinimumCDFEpsilon" ) * value) return value;
    // 0.1 * ((1000 * indexOuter) / outerMax) is to print percents with one figure after the decimal point
    LOGINFO(OSS() << 0.1 * ((1000 * indexOuter) / outerMax) << "% value=" << value << " absolute precision(99%)=" << precision << " relative precision(99%)=" << ((value > 0.0) ? precision / value : -1.0));
  }
  RandomGenerator::SetState(initialState);
  cdfEpsilon_ = precision;
  return value;
} // computeCDF

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Normal::computeCharacteristicFunction(const NumericalScalar x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex Normal::computeCharacteristicFunction(const NumericalPoint & x) const
{
  return std::exp(computeLogCharacteristicFunction(x));
}

NumericalComplex Normal::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  return NumericalComplex(-0.5 * sigma_[0] * sigma_[0] * x * x, mean_[0] * x);
}

NumericalComplex Normal::computeLogCharacteristicFunction(const NumericalPoint & x) const
{
  if (x.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << getDimension() << ", here dimension=" << x.getDimension();
  return NumericalComplex(-0.5 * dot(x, getCovariance() * x), dot(x, mean_));
}

/* Compute the probability content of an interval */
NumericalScalar Normal::computeProbability(const Interval & interval) const
{
  if (interval.isNumericallyEmpty()) return 0.0;
  const UnsignedInteger dimension(getDimension());
  // The generic implementation provided by the DistributionImplementation upper class is more accurate than the generic implementation provided by the ContinuousDistribution upper class for dimension = 1
  if (dimension == 1) return DistributionImplementation::computeProbability(interval);
  // Decompose and normalize the interval
  NumericalPoint lower(normalize(interval.getLowerBound()));
  NumericalPoint upper(normalize(interval.getUpperBound()));
  Interval::BoolCollection finiteLower(interval.getFiniteLowerBound());
  Interval::BoolCollection finiteUpper(interval.getFiniteUpperBound());
  /* Special treatment for independent components */
  if (hasIndependentCopula_)
  {
    NumericalScalar lowerCDF(0.0);
    if (finiteLower[0]) lowerCDF = DistFunc::pNormal(lower[0]);
    NumericalScalar upperCDF(1.0);
    if (finiteUpper[0]) upperCDF = DistFunc::pNormal(upper[0]);
    NumericalScalar value(upperCDF - lowerCDF);
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
    const UnsignedInteger maximumNumber(static_cast< UnsignedInteger > (round(std::pow(ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ), 1.0 / getDimension()))));
    const UnsignedInteger candidateNumber(ResourceMap::GetAsUnsignedInteger( "Normal-MarginalIntegrationNodesNumber" ));
    if (candidateNumber > maximumNumber) LOGWARN(OSS() << "Warning! The requested number of marginal integration nodes=" << candidateNumber << " would lead to an excessive number of PDF evaluations. It has been reduced to " << maximumNumber << ". You should increase the ResourceMap key \"Normal-MaximumNumberOfPoints\"");
    setIntegrationNodesNumber(std::min(maximumNumber, candidateNumber));
    return ContinuousDistribution::computeProbability(interval);
  }
  // For very large dimension, use a MonteCarlo algorithm
  LOGWARN(OSS() << "Warning, in Normal::computeProbability(), the dimension is very high. We will use a Monte Carlo method for the computation with a relative precision of 0.1% at 99% confidence level and a maximum of " << 10 * ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ) << " realizations. Expect a long running time and a poor accuracy for low values of the CDF...");
  NumericalScalar value(0.0);
  NumericalScalar variance(0.0);
  NumericalScalar a99(DistFunc::qNormal(0.995));
  UnsignedInteger outerMax(10 * ResourceMap::GetAsUnsignedInteger( "Normal-MaximumNumberOfPoints" ) / ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ));
  NumericalScalar precision(0.0);
  for (UnsignedInteger indexOuter = 0; indexOuter < outerMax; ++indexOuter)
  {
    NumericalScalar valueBlock(0.0);
    NumericalScalar varianceBlock(0.0);
    for (UnsignedInteger indexSample = 0; indexSample < ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ); ++indexSample)
    {
      // ind value is 1.0 if the realization is inside of the integration domain, 0.0 else.
      NumericalScalar ind(interval.numericallyContains(getRealization()));
      NumericalScalar norm(1.0 / (indexSample + 1.0));
      varianceBlock = (varianceBlock * indexSample + (1.0 - norm) * (valueBlock - ind) * (valueBlock - ind)) * norm;
      valueBlock = (valueBlock * indexSample + ind) * norm;
    }
    NumericalScalar norm(1.0 / (indexOuter + 1.0));
    variance = (varianceBlock + indexOuter * variance + (1.0 - norm) * (value - valueBlock) * (value - valueBlock)) * norm;
    value = (value * indexOuter + valueBlock) * norm;
    // Quick return for value = 1
    if ((value >= 1.0 - ResourceMap::GetAsNumericalScalar("DistributionImplementation-DefaultQuantileEpsilon")) && (variance == 0.0)) return 1.0;
    precision = a99 * std::sqrt(variance / (indexOuter + 1.0) / ResourceMap::GetAsUnsignedInteger( "Normal-MinimumNumberOfPoints" ));
    if (precision < ResourceMap::GetAsNumericalScalar( "Normal-MinimumCDFEpsilon" ) * value) return value;
    // 0.1 * ((1000 * indexOuter) / outerMax) is to print percents with one figure after the decimal point
    LOGINFO(OSS() << 0.1 * ((1000 * indexOuter) / outerMax) << "% value=" << value << " absolute precision(99%)=" << precision << " relative precision(99%)=" << ((value > 0.0) ? precision / value : -1.0));
  }
  return value;
}

/* Get the CDF gradient of the distribution */
NumericalPoint Normal::computeCDFGradient(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  NumericalPoint gradientCDF(2 * dimension);
  if (dimension == 1)
  {
    NumericalScalar pdf(computePDF(point));
    gradientCDF[0] = -pdf;
    gradientCDF[1] = -pdf * (point[0] - mean_[0]) / sigma_[0];
    return gradientCDF;
  }
  // To be implemented
  return gradientCDF;
}

/* Compute the scalar quantile of the 1D normal distribution */
NumericalScalar Normal::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
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
NumericalScalar Normal::computeConditionalPDF(const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return Distribution(getMarginal(conditioningDimension)).computePDF(x);
  // General case
  NumericalScalar meanRos(0.0);
  const NumericalScalar sigmaRos(1.0 / inverseCholesky_(conditioningDimension, conditioningDimension));
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    meanRos += inverseCholesky_(conditioningDimension, i) / std::sqrt(sigma_[i]) * (y[i] - mean_[i]);
  }
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  return std::exp(-0.5 * std::pow(x - meanRos, 2.0) / (sigmaRos * sigmaRos)) / (sigmaRos * std::sqrt(2.0 * M_PI));
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar Normal::computeConditionalCDF(const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if ((conditioningDimension == 0) || (hasIndependentCopula())) return Distribution(getMarginal(conditioningDimension)).computeCDF(x);
  // General case
  NumericalScalar meanRos(0.0);
  const NumericalScalar sigmaRos(1.0 / inverseCholesky_(conditioningDimension, conditioningDimension));
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    meanRos += inverseCholesky_(conditioningDimension, i) / std::sqrt(sigma_[i]) * (y[i] - mean_[i]);
  }
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  return DistFunc::pNormal((x - meanRos) / sigmaRos);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar Normal::computeConditionalQuantile(const NumericalScalar q,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case when no contitioning or independent copula
  if ((conditioningDimension == 0) || hasIndependentCopula()) return mean_[conditioningDimension] + sigma_[conditioningDimension] * DistFunc::qNormal(q);
  // General case
  NumericalScalar meanRos(0.0);
  const NumericalScalar sigmaRos(1.0 / inverseCholesky_(conditioningDimension, conditioningDimension));
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    meanRos += inverseCholesky_(conditioningDimension, i) / std::sqrt(sigma_[i]) * (y[i] - mean_[i]);
  }
  meanRos = mean_[conditioningDimension] - sigmaRos * std::sqrt(sigma_[conditioningDimension]) * meanRos;
  if (q == 0.0) return meanRos - 0.5 * sigmaRos * std::sqrt(4.0 * (std::log(SpecFunc::ISQRT2PI / sigmaRos) - SpecFunc::LogMinNumericalScalar));
  if (q == 1.0) return meanRos + 0.5 * sigmaRos * std::sqrt(4.0 * (std::log(SpecFunc::ISQRT2PI / sigmaRos) - SpecFunc::LogMinNumericalScalar));
  return meanRos + sigmaRos * DistFunc::qNormal(q);
}

/* Get the i-th marginal distribution */
Normal::Implementation Normal::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension(getDimension());
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  CorrelationMatrix R(1);
  NumericalPoint sigma(1, sigma_[i]);
  NumericalPoint mean(1, mean_[i]);
  return new Normal(mean, sigma, R);
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Normal::Implementation Normal::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension(getDimension());
  if (!indices.check(dimension - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and  must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension(indices.getSize());
  CorrelationMatrix R(outputDimension);
  NumericalPoint sigma(outputDimension);
  NumericalPoint mean(outputDimension);
  // Extract the correlation matrix, the marginal standard deviations and means
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i(indices[i]);
    sigma[i] = sigma_[index_i];
    mean[i] = mean_[index_i];
    for (UnsignedInteger j = 0; j <= i; ++j)
    {
      R(i, j) = R_(index_i, indices[j]);
    }
  }
  return new Normal(mean, sigma, R);
} // getMarginal(Indices)

/* Get the skewness of the distribution */
NumericalPoint Normal::getSkewness() const
{
  return NumericalPoint(getDimension(), 0.0);
}

/* Get the moments of the standardized distribution */
NumericalPoint Normal::getStandardMoment(const UnsignedInteger n) const
{
  if (n % 2 == 1) return NumericalPoint(1, 0.0);
  NumericalScalar moment(1.0);
  for (UnsignedInteger i = 1; i < n / 2; ++i)
    moment *= 2.0 * i + 1.0;
  return NumericalPoint(1, moment);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Normal::Implementation Normal::getStandardRepresentative() const
{
  return Normal(0.0, 1.0).clone();
}

/* Get the roughness, i.e. the L2-norm of the PDF */
NumericalScalar Normal::getRoughness() const
{
  // 0.2820947917738781434740398 = 1 / (2 * sqrt(Pi))
  return 0.2820947917738781434740398 / getSigma()[0];
}

/* Get the kurtosis of the distribution */
NumericalPoint Normal::getKurtosis() const
{
  return NumericalPoint(getDimension(), 3.0);
}

/* Get the copula of the distribution */
Normal::Implementation Normal::getCopula() const
{
  return new NormalCopula(R_);
}

/* Compute the radial distribution CDF */
NumericalScalar Normal::computeRadialDistributionCDF(const NumericalScalar radius,
    const Bool tail) const
{
  // It should be a chi-squared distribution with dimension degrees of freedom
  // We only have a gamma distribution available for this time (OpenTURNS rc1)
  // so we use the relation chisq(n) = gamma(n/2, 1/2)
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
  const UnsignedInteger dimension(getDimension());
  const NumericalScalar qNorm(DistFunc::qNormal(cdfEpsilon_));
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
  const UnsignedInteger dimension(getDimension());
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
  adv.saveAttribute( "normalizationFactor_duplicate", normalizationFactor_ );
  adv.saveAttribute( "hasIndependentCopula_", hasIndependentCopula_ );
}

/* Method load() reloads the object from the StorageManager */
void Normal::load(Advocate & adv)
{
  EllipticalDistribution::load(adv);
  adv.loadAttribute( "normalizationFactor_duplicate", normalizationFactor_ );
  adv.loadAttribute( "hasIndependentCopula_", hasIndependentCopula_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
