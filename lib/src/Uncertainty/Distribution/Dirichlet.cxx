//                                               -*- C++ -*-
/**
 *  @brief The Dirichlet distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cmath>
#include "openturns/Indices.hxx"
#include "openturns/Dirichlet.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/JacobiFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Dirichlet)

static const Factory<Dirichlet> Factory_Dirichlet;


/* Default constructor */
Dirichlet::Dirichlet()
  : ContinuousDistribution()
  , theta_(0)
  , sumTheta_(0.0)
  , normalizationFactor_(0.0)
  , isInitializedCDF_(false)
  , integrationNodes_(0)
  , integrationWeights_(0)
{
  setName("Dirichlet");
  setTheta(Point(2, 1.0));
  isParallel_ = false;
}

/* Parameters constructor */
Dirichlet::Dirichlet(const Point & theta)
  : ContinuousDistribution()
  , theta_(0)
  , sumTheta_(0.0)
  , normalizationFactor_(0.0)
  , isInitializedCDF_(false)
  , integrationNodes_(0)
  , integrationWeights_(0)
{
  setName("Dirichlet");
  setTheta(theta);
  isParallel_ = false;
}

/* Comparison operator */
Bool Dirichlet::operator ==(const Dirichlet & other) const
{
  if (this == &other) return true;
  return theta_ == other.theta_;
}

Bool Dirichlet::equals(const DistributionImplementation & other) const
{
  const Dirichlet* p_other = dynamic_cast<const Dirichlet*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Dirichlet::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Dirichlet::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " theta=" << theta_;
  return oss;
}

String Dirichlet::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(theta = " << theta_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
Dirichlet * Dirichlet::clone() const
{
  return new Dirichlet(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Dirichlet::computeRange()
{
  setRange(Interval(Point(theta_.getSize() - 1, 0.0), Point(theta_.getSize() - 1, 1.0)));
}

/* Get one realization of the distribution. We use the representation:
 * X_k = Y_k / (Y_1 + ... + Y_{d+1}) where the Y_i are independent and
 * Y_i has a Gamma(theta_i) distribution
 */
Point Dirichlet::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 1) return Point(1, DistFunc::rBeta(theta_[0], theta_[1]));
  Point realization(dimension);
  Scalar sum = DistFunc::rGamma(theta_[dimension]);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar yI = DistFunc::rGamma(theta_[i]);
    sum += yI;
    realization[i] = yI;
  }
  return realization * (1.0 / sum);
}

/* Get the PDF of the distribution */
Scalar Dirichlet::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  const Scalar logPDF = computeLogPDF(point);
  return std::exp(logPDF);
}

Scalar Dirichlet::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar xI = point[i];
    if (xI <= 0.0) return SpecFunc::LowestScalar;
    sum += xI;
  }
  if (sum >= 1.0) return SpecFunc::LowestScalar;
  Scalar logPDF = normalizationFactor_ + (theta_[dimension] - 1.0) * log1p(-sum);
  for (UnsignedInteger i = 0; i < dimension; ++i) logPDF += (theta_[i] - 1.0) * std::log(point[i]);
  return logPDF;
}

/* Initialize the integration routine */
void Dirichlet::initializeIntegration() const
{
  const UnsignedInteger dimension = getDimension();
  // Initialization at the first call
  static const UnsignedInteger N(ResourceMap::GetAsUnsignedInteger("Dirichlet-DefaultIntegrationSize"));
  // Do we have to initialize the CDF data?
  if (!isInitializedCDF_)
  {
    integrationNodes_ = PointCollection(0);
    integrationWeights_ = PointCollection(0);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      Point marginalWeights;
      Point marginalNodes(JacobiFactory(0, theta_[i] - 1.0).getNodesAndWeights(N, marginalWeights));
      integrationNodes_.add(marginalNodes);
      integrationWeights_.add(marginalWeights);
    }
    isInitializedCDF_ = true;
  } // !isInitialized
}

/* Get the CDF of the distribution */
Scalar Dirichlet::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  if (dimension == 1)
  {
    if (point[0] <= 0.0) return 0.0;
    if (point[0] >= 1.0) return 1.0;
    return DistFunc::pBeta(theta_[0], theta_[1], point[0]);
  }
  // First, the trivial cases
  Bool oneNegative = false;
  Bool allPositive = true;
  Bool allGreaterThanOne = true;
  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar xI = point[i];
    oneNegative = oneNegative || (xI <= 0.0);
    allPositive = allPositive && (xI > 0.0);
    allGreaterThanOne = allGreaterThanOne && (xI >= 1.0);
    sum += xI;
  }
  if (oneNegative) return 0.0;
  if (allGreaterThanOne) return 1.0;
  // The "inside simplex" case: use Gauss integration for now
  if (allPositive && (sum <= 1.0))
  {
    Indices indices(dimension, 0);
    Scalar value = 0.0;
    Scalar logFactor = normalizationFactor_;
    for (UnsignedInteger i = 0; i < dimension; ++i) logFactor += theta_[i] * std::log(point[i]) - std::log(theta_[i]);
    // Initialize the integration data
    initializeIntegration();
    UnsignedInteger size = 1;
    for (UnsignedInteger i = 0; i < dimension; ++i) size *= integrationNodes_[i].getSize();
    // Loop over the integration nodes
    for (UnsignedInteger flatIndex = 0; flatIndex < size; ++flatIndex)
    {
      // The current point has components obtained by tensorization of scaled Jacobi polynomials zeros
      Scalar w = 1.0;
      Scalar sumX = 0.0;
      for (UnsignedInteger i = 0; i < dimension; ++i)
      {
        const UnsignedInteger indexI = indices[i];
        const Scalar lI = 0.5 * point[i];
        sumX += (integrationNodes_[i][indexI] + 1.0) * lI;
        w *= integrationWeights_[i][indexI];
      }
      const Scalar dCDF = w * std::exp(logFactor + (theta_[dimension] - 1.0) * log1p(-sumX));
      value += dCDF;
      // Update the indices
      ++indices[0];
      // Propagate the remainders
      for (UnsignedInteger i = 0; i < dimension - 1; ++i) indices[i + 1] += (indices[i] == integrationNodes_[i].getSize());
      // Correction of the indices. The last index cannot overflow.
      for (UnsignedInteger i = 0; i < dimension - 1; ++i) indices[i] = indices[i] % integrationNodes_[i].getSize();
    } // flatIndex
    return value;
  } // in the simplex
  Point sortedPoint(point);
  std::sort(sortedPoint.begin(), sortedPoint.end());
  Scalar value = 0.0;
  // Can go there only if dimension > 1
  Scalar sortedSum = sortedPoint[0] + sortedPoint[1];
  if (sortedSum >= 1.0)
  {
    value = 1.0 - dimension;
    for (UnsignedInteger i = 0; i < dimension; ++i) value += DistFunc::pBeta(theta_[i], sumTheta_ - theta_[i], point[i]);
    return value;
  }
  // Can go there only if dimension > 2
  sortedSum += sortedPoint[2];
  if (sortedSum >= 1.0)
  {
    value = 0.5 * (dimension - 1);
    for (UnsignedInteger i = 0; i < dimension; ++i) value -= DistFunc::pBeta(theta_[i], sumTheta_ - theta_[i], point[i]);
    value *= dimension - 2;
    Indices marginal2D(2);
    Point marginalPoint(2);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      marginal2D[0] = i;
      marginalPoint[0] = point[i];
      for (UnsignedInteger j = 0; j < i; ++j)
      {
        marginal2D[1] = j;
        marginalPoint[1] = point[j];
        value += Distribution(getMarginal(marginal2D)).computeCDF(marginalPoint);
      }
    }
    return value;
  }
  // Use crude Monte Carlo for now, with a fixed sampling size
  static const UnsignedInteger samplingSize(ResourceMap::GetAsUnsignedInteger("DefaultSamplingSize"));
  RandomGeneratorState initialState(RandomGenerator::GetState());
  RandomGenerator::SetSeed(samplingSize);
  static const Sample sample(getSample(samplingSize));
  UnsignedInteger successNumber = 0;
  for (UnsignedInteger i = 0; i < samplingSize; ++i)
  {
    const Point x(sample[i]);
    Bool success = true;
    for (UnsignedInteger j = 0; j < dimension; ++j)
      if (x[j] > point[j])
      {
        success = false;
        break;
      }
    successNumber += success;
  }
  RandomGenerator::SetState(initialState);
  return Scalar(successNumber) / samplingSize;
}

/* Get the quantile of the distribution */
Scalar Dirichlet::computeScalarQuantile(const Scalar prob,
                                        const Bool tail) const
{
  return DistFunc::qBeta(theta_[0], theta_[1], prob, tail);
}

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Dirichlet::computeConditionalPDF(const Scalar x,
                                        const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  const Scalar r = theta_[conditioningDimension];
  Scalar s = sumTheta_ - r;
  if (conditioningDimension == 0) return std::exp(- SpecFunc::LnBeta(r, s) + (r - 1.0) * std::log(x) + (s - 1.0) * log1p(-x));
  Scalar sumThetaConditioning = 0.0;
  Scalar sumY = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    sumThetaConditioning += theta_[i];
    sumY += y[i];
  }
  if (sumY <= 0.0 || sumY >= 1.0) return 0.0;
  s -= sumThetaConditioning;
  const Scalar z = x / (1.0 - sumY);
  if (z <= 0.0 || z >= 1.0) return 0.0;
  return std::exp(-SpecFunc::LnBeta(r, s) + (r - 1.0) * std::log(z) + (s - 1.0) * log1p(-z)) / (1.0 - sumY);
}

Point Dirichlet::computeSequentialConditionalPDF(const Point & x) const
{
  const UnsignedInteger dimension = getDimension();
  if (x.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional PDF at a point of dimension=" << x.getDimension() << " not equal to the distribution dimension=" << dimension;
  Point result(dimension);
  Scalar sumY = 0.0;
  Scalar r = theta_[0];
  Scalar s = sumTheta_ - r;
  Scalar z = x[0];
  result[0] = std::exp(- SpecFunc::LnBeta(r, s) + (r - 1.0) * std::log(z) + (s - 1.0) * log1p(-z));
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension; ++conditioningDimension)
  {
    sumY += x[conditioningDimension - 1];
    if (sumY <= 0.0 || sumY >= 1.0) return result;
    s -= r;
    r = theta_[conditioningDimension];
    z = x[conditioningDimension] / (1.0 - sumY);
    if (z <= 0.0 || z >= 1.0) break;
    result[conditioningDimension] = std::exp(- SpecFunc::LnBeta(r, s) + (r - 1.0) * std::log(z) + (s - 1.0) * log1p(-z)) / (1.0 - sumY);
  }
  return result;
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Dirichlet::computeConditionalCDF(const Scalar x,
                                        const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  const Scalar r = theta_[conditioningDimension];
  Scalar s = sumTheta_ - r;
  if (conditioningDimension == 0) return DistFunc::pBeta(r, s, x);
  Scalar sumThetaConditioning = 0.0;
  Scalar sumY = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    sumThetaConditioning += theta_[i];
    sumY += y[i];
  }
  if (sumY <= 0.0) return 0.0;
  if (sumY >= 1.0) return 1.0;
  s -= sumThetaConditioning;
  return DistFunc::pBeta(r, s, x / (1.0 - sumY));
}

Point Dirichlet::computeSequentialConditionalCDF(const Point & x) const
{
  const UnsignedInteger dimension = getDimension();
  if (x.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional PDF at a point of dimension=" << x.getDimension() << " not equal to the distribution dimension=" << dimension;
  Point result(dimension);
  Scalar sumY = 0.0;
  Scalar r = theta_[0];
  Scalar s = sumTheta_ - r;
  Scalar z = x[0];
  result[0] = DistFunc::pBeta(r, s, z);
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension; ++conditioningDimension)
  {
    sumY += x[conditioningDimension - 1];
    if (sumY <= 0.0 || sumY >= 1.0) return result;
    s -= r;
    r = theta_[conditioningDimension];
    z = x[conditioningDimension] / (1.0 - sumY);
    result[conditioningDimension] = DistFunc::pBeta(r, s, z);
  }
  return result;
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar Dirichlet::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  Scalar sumThetaConditioning = 0.0;
  Scalar sumY = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    sumThetaConditioning += theta_[i];
    sumY += y[i];
  }
  const Scalar r = theta_[conditioningDimension];
  const Scalar s = sumTheta_ - sumThetaConditioning - r;
  return (1.0 - sumY) * DistFunc::qBeta(r, s, q);
}

Point Dirichlet::computeSequentialConditionalQuantile(const Point & q) const
{
  const UnsignedInteger dimension = getDimension();
  if (q.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: cannot compute a sequential conditional PDF at a quantile level vector of dimension=" << q.getDimension() << " not equal to the distribution dimension=" << dimension;
  Point result(dimension);
  Scalar sumY = 0.0;
  Scalar r = theta_[0];
  Scalar s = sumTheta_ - r;
  result[0] = DistFunc::qBeta(r, s, q[0]);
  for (UnsignedInteger conditioningDimension = 1; conditioningDimension < dimension; ++conditioningDimension)
  {
    sumY += result[conditioningDimension - 1];
    if (sumY <= 0.0 || sumY >= 1.0) return result;
    s -= r;
    r = theta_[conditioningDimension];
    result[conditioningDimension] = (1.0 - sumY) * DistFunc::qBeta(r, s, q[conditioningDimension]);
  }
  return result;
}

/* Compute the mean of the distribution */
void Dirichlet::computeMean() const
{
  const UnsignedInteger dimension = getDimension();
  mean_ = Point(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) mean_[i] = theta_[i] / sumTheta_;
  isAlreadyComputedMean_ = true;
}

/* Compute the entropy of the distribution */
Scalar Dirichlet::computeEntropy() const
{
  const UnsignedInteger dimension = getDimension();
  Scalar entropy = -normalizationFactor_ - (dimension + 1 - sumTheta_) * SpecFunc::Psi(sumTheta_);
  for (UnsignedInteger i = 0; i <= dimension; ++i)
    entropy -= (theta_[i] - 1.0) * SpecFunc::Psi(theta_[i]);
  return entropy;
}

/* Get the standard deviation of the distribution */
Point Dirichlet::getStandardDeviation() const
{
  const UnsignedInteger dimension = getDimension();
  Point sigma(dimension);
  Scalar factor = 1.0 / (sumTheta_ * std::sqrt(1.0 + sumTheta_));
  for (UnsignedInteger i = 0; i < dimension; ++i) sigma[i] = theta_[i] * (sumTheta_ - theta_[i]) * factor;
  return sigma;
}

/* Get the skewness of the distribution */
Point Dirichlet::getSkewness() const
{
  const UnsignedInteger dimension = getDimension();
  Point skewness(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar thetaI = theta_[i];
    skewness[i] = 2.0 * (sumTheta_ - 2.0 * thetaI) / (sumTheta_ + 2.0) * std::sqrt(sumTheta_ + 1.0) / (thetaI * (sumTheta_ - thetaI));
  }
  return skewness;
}

/* Get the kurtosis of the distribution */
Point Dirichlet::getKurtosis() const
{
  const UnsignedInteger dimension = getDimension();
  Point kurtosis(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar thetaI = theta_[i];
    kurtosis[i] = 3.0 * (sumTheta_ + 1.0) * (2.0 * sumTheta_ * sumTheta_ + thetaI * (sumTheta_ - 6.0) * (sumTheta_ - thetaI)) / (thetaI * (sumTheta_ - thetaI) * (3.0 + sumTheta_) * (2.0 + sumTheta_));
  }
  return kurtosis;
}

/* Compute the covariance of the distribution */
void Dirichlet::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  covariance_ = CovarianceMatrix(dimension);
  Scalar factor = 1.0 / (sumTheta_ * sumTheta_ * (1.0 + sumTheta_));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar thetaI = theta_[i];
    covariance_(i, i) = factor * (thetaI * (sumTheta_ - thetaI));
    for (UnsignedInteger j = 0; j < i; ++j) covariance_(i, j) = -factor * thetaI * theta_[j];
  }
  isAlreadyComputedCovariance_ = true;
}

/* Theta accessor */
void Dirichlet::setTheta(const Point & theta)
{
  const UnsignedInteger size = theta.getSize();
  if (size <= 1) throw InvalidArgumentException(HERE) << "Error: the parameter theta must be of size at least 2.";
  for (UnsignedInteger i = 0; i < size; ++i) if (!(theta[i] > 0.0)) throw InvalidArgumentException(HERE) << "Error: all the components of theta must be positive";
  theta_ = theta;
  sumTheta_ = 0.0;
  normalizationFactor_ = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar thetaI = theta[i];
    if (!(thetaI > 0.0)) throw InvalidArgumentException(HERE) << "Error: the vector theta must have positive components, here theta(" << i << ")=" << thetaI;
    normalizationFactor_ -= SpecFunc::LnGamma(thetaI);
    sumTheta_ += thetaI;
  }
  normalizationFactor_ += SpecFunc::LnGamma(sumTheta_);
  setDimension(size - 1);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

Point Dirichlet::getTheta() const
{
  return theta_;
}

/* Get the i-th marginal distribution */
Distribution Dirichlet::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (dimension == 1) return clone();
  Point thetaMarginal(2);
  thetaMarginal[0] = theta_[i];
  thetaMarginal[1] = sumTheta_ - theta_[i];
  Dirichlet::Implementation marginal(new Dirichlet(thetaMarginal));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution Dirichlet::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  if (dimension == 1) return clone();
  const UnsignedInteger outputDimension = indices.getSize();
  Point thetaMarginal(outputDimension + 1);
  Scalar sumMarginal = 0.0;
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i = indices[i];
    const Scalar thetaI = theta_[index_i];
    sumMarginal += thetaI;
    thetaMarginal[i] = thetaI;
  }
  thetaMarginal[outputDimension] = sumTheta_ - sumMarginal;
  Dirichlet* marginal(new Dirichlet(thetaMarginal));
  // Initialize the CDF computation if the data are available
  if (isInitializedCDF_)
  {
    PointCollection marginalIntegrationNodes_(0);
    PointCollection marginalIntegrationWeights_(0);
    for (UnsignedInteger i = 0; i < outputDimension; ++i)
    {
      marginalIntegrationNodes_.add(integrationNodes_[indices[i]]);
      marginalIntegrationWeights_.add(integrationWeights_[indices[i]]);
    }
    marginal->integrationNodes_ = marginalIntegrationNodes_;
    marginal->integrationWeights_ = marginalIntegrationWeights_;
    marginal->isInitializedCDF_ = true;
  }
  marginal->setDescription(getDescription().select(indices));
  return marginal;
} // getMarginal(Indices)

/* Tell if the distribution has independent marginals */
Bool Dirichlet::hasIndependentCopula() const
{
  return getDimension() == 1;
}

/* Tell if the distribution has an elliptical copula */
Bool Dirichlet::hasEllipticalCopula() const
{
  return hasIndependentCopula();
}

/* Get the Spearman correlation of the distribution */
CorrelationMatrix Dirichlet::getSpearmanCorrelation() const
{
  return DistributionImplementation::getSpearmanCorrelation();
#ifdef NEW_IMPLEMENTATION
  const UnsignedInteger dimension = getDimension();
  CorrelationMatrix rho(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const UnsignedInteger nI = integrationNodes_[i].getSize();
    for (UnsignedInteger j = 0; j < i; ++j)
    {
      const UnsignedInteger nJ = integrationNodes_[j].getSize();
      // Perform the numerical integration of the (i,j) correlation
      Scalar rhoIJ = 0.0;
      for (UnsignedLong indexU = 0; indexU < nI; ++indexU)
      {
        const Scalar u = 0.5 * (integrationNodes_(j, indexU) + 1.0);
        for (UnsignedLong indexV = 0; indexV < nJ; ++indexV)
        {
          Scalar v = 0.5 * (integrationNodes_(j, indexU) + 1.0) * (1.0 - u);
        } // indexV
      } // indexU
    } // j
  } // i
  return rho;
#endif
}

/* Get the Kendall concordance of the distribution */
CorrelationMatrix Dirichlet::getKendallTau() const
{
  return DistributionImplementation::getKendallTau();
#ifdef NEW_IMPLEMENTATION
  CorrelationMatrix tau(2);
  tau(0, 1) = 1.0 + 4.0 * (SpecFunc::Debye(theta_, 1) - 1.0) / theta_;
  return tau;
#endif
}

DistributionImplementation::PointWithDescriptionCollection Dirichlet::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters(dimension);
  const Description description(getDescription());
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
  {
    PointWithDescription point(2);
    Description marginalDescription(point.getDimension());
    point[0] = theta_[marginalIndex];
    point[1] = sumTheta_ - theta_[marginalIndex];
    marginalDescription[0] = "theta";
    marginalDescription[1] = "sum theta";
    point.setDescription(marginalDescription);
    point.setName(description[marginalIndex]);
    parameters[marginalIndex] = point;
  } // marginalIndex
  return parameters;
}


void Dirichlet::setParametersCollection(const PointCollection & parametersCollection)
{
  const UnsignedInteger size = parametersCollection.getSize();
  const UnsignedInteger dimension = size;
  Point theta(dimension + 1);
  if ( size == 0 ) throw InvalidArgumentException(HERE) << "The collection is empty.";
  if ( parametersCollection[0].getSize() < 2 ) throw InvalidArgumentException(HERE) << "The collection is too small.";
  Scalar lastTheta = parametersCollection[0][0] + parametersCollection[0][1];// sum of all thetas
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    theta[i] = parametersCollection[i][0];
    lastTheta -= theta[i]; // subtract each theta except the last one
  }
  theta[dimension] = lastTheta;
  setTheta(theta);
}

Point Dirichlet::getParameter() const
{
  return theta_;
}

void Dirichlet::setParameter(const Point & parameter)
{
  const Scalar w = getWeight();
  *this = Dirichlet(parameter);
  setWeight(w);
}

Description Dirichlet::getParameterDescription() const
{
  const UnsignedInteger dimension = getDimension();
  Description description(dimension + 1);
  for (UnsignedInteger i = 0; i <= dimension; ++ i)
  {
    description[i] = (OSS() << "theta_" << i);
  }
  return description;
}

/* Method save() stores the object through the StorageManager */
void Dirichlet::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "theta_", theta_ );
  adv.saveAttribute( "sumTheta_", sumTheta_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void Dirichlet::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "theta_", theta_ );
  adv.loadAttribute( "sumTheta_", sumTheta_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
