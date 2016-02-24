//                                               -*- C++ -*-
/**
 *  @brief The maximum entropy order statistics distribution
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
#include <cmath>
#include "openturns/MaximumEntropyOrderStatisticsDistribution.hxx"
#include "openturns/MaximumEntropyOrderStatisticsCopula.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/OrderStatisticsMarginalChecker.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/MethodBoundNumericalMathEvaluationImplementation.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/Brent.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MaximumEntropyOrderStatisticsDistribution);

static const Factory<MaximumEntropyOrderStatisticsDistribution> RegisteredFactory;


/* Default constructor */
MaximumEntropyOrderStatisticsDistribution::MaximumEntropyOrderStatisticsDistribution()
  : ContinuousDistribution()
{
  setName("MaximumEntropyOrderStatisticsDistribution");
  DistributionCollection coll(2);
  coll[0] = Uniform(-1.0, 0.5);
  coll[1] = Uniform(-0.5, 1.0);
  integrator_ = GaussKronrod(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization"), ResourceMap::GetAsNumericalScalar("GaussKronrod-MaximumError"), GaussKronrodRule(GaussKronrodRule::G7K15));
  // This call set also the range. Use approximation but don't check marginals.
  setDistributionCollection(coll, true, false);
  setIntegrationNodesNumber(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-CDFIntegrationNodesNumber"));
  // To insure that the nodes will be already computed when calling computeCDF() in parallel
  NumericalPoint weights;
  NumericalPoint nodes(getGaussNodesAndWeights(weights));
}

/* Parameters constructor */
MaximumEntropyOrderStatisticsDistribution::MaximumEntropyOrderStatisticsDistribution(const DistributionCollection & coll,
    const Bool useApprox,
    const Bool checkMarginals)
  : ContinuousDistribution()
  , distributionCollection_(coll)
{
  setName("MaximumEntropyOrderStatisticsDistribution");
  integrator_ = GaussKronrod(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization"), ResourceMap::GetAsNumericalScalar("GaussKronrod-MaximumError"), GaussKronrodRule(GaussKronrodRule::G7K15));
  // This call set also the range.
  setDistributionCollection(coll, useApprox, checkMarginals);
  setIntegrationNodesNumber(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-CDFIntegrationNodesNumber"));
  // To insure that the nodes will be already computed when calling computeCDF() in parallel
  NumericalPoint weights;
  NumericalPoint nodes(getGaussNodesAndWeights(weights));
}

/* Parameters constructor */
MaximumEntropyOrderStatisticsDistribution::MaximumEntropyOrderStatisticsDistribution(const DistributionCollection & coll,
    const Indices & partition,
    const Bool useApprox,
    const Collection<PiecewiseHermiteEvaluationImplementation> & exponentialFactorApproximation,
    const Description & description)
  : ContinuousDistribution()
  , distributionCollection_(coll)
  , partition_(partition)
  , useApproximation_(useApprox)
  , exponentialFactorApproximation_(exponentialFactorApproximation)
  , integrator_(GaussKronrod(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization"), ResourceMap::GetAsNumericalScalar("GaussKronrod-MaximumError"), GaussKronrodRule(GaussKronrodRule::G7K15)))
{
  isParallel_ = false;
  // Initialize the distribution manually in order to avoid costly checks that are not needed here
  const UnsignedInteger size(coll.getSize());
  setDimension(size);
  computeRange();
  setDescription(description);
  setIntegrationNodesNumber(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-CDFIntegrationNodesNumber"));
  // To insure that the nodes will be already computed when calling computeCDF() in parallel
  NumericalPoint weights;
  NumericalPoint nodes(getGaussNodesAndWeights(weights));
}

/* Comparison operator */
Bool MaximumEntropyOrderStatisticsDistribution::operator ==(const MaximumEntropyOrderStatisticsDistribution & other) const
{
  if (this == &other) return true;
  return (distributionCollection_ == other.distributionCollection_);
}

/* String converter */
String MaximumEntropyOrderStatisticsDistribution::__repr__() const
{
  OSS oss(true);
  oss << "class=" << MaximumEntropyOrderStatisticsDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " collection=" << distributionCollection_;
  return oss;
}

String MaximumEntropyOrderStatisticsDistribution::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(collection = " << distributionCollection_ << ")";
  return oss;
}

/* Virtual constructor */
MaximumEntropyOrderStatisticsDistribution * MaximumEntropyOrderStatisticsDistribution::clone() const
{
  return new MaximumEntropyOrderStatisticsDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void MaximumEntropyOrderStatisticsDistribution::computeRange()
{
  const UnsignedInteger dimension(getDimension());
  NumericalPoint lowerBound(dimension);
  NumericalPoint upperBound(dimension);
  Interval::BoolCollection finiteLowerBound(dimension);
  Interval::BoolCollection finiteUpperBound(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Interval atomRange(distributionCollection_[i].getRange());
    lowerBound[i] = atomRange.getLowerBound()[0];
    upperBound[i] = atomRange.getUpperBound()[0];
    finiteLowerBound[i] = atomRange.getFiniteLowerBound()[0];
    finiteUpperBound[i] = atomRange.getFiniteUpperBound()[0];
  }
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

struct MaximumEntropyOrderStatisticsDistributionWrapper
{
  MaximumEntropyOrderStatisticsDistributionWrapper(const MaximumEntropyOrderStatisticsDistribution & distribution,
      const UnsignedInteger lower,
      const UnsignedInteger upper,
      const NumericalScalar lowerBound)
    : distribution_(distribution)
    , lower_(lower)
    , upper_(upper)
    , lowerBound_(lowerBound)
  {
    // Nothing to do
  }

  NumericalPoint computePhi(const NumericalPoint & point) const
  {
    const NumericalScalar pdfUpper(distribution_.distributionCollection_[upper_].computePDF(point));
    NumericalScalar value(0.0);
    if (pdfUpper > 0.0)
    {
      // First, try to use complementary CDF
      NumericalScalar a(distribution_.distributionCollection_[lower_].computeComplementaryCDF(point));
      NumericalScalar b;
      // If the smallest complementary CDF is less than 1/2 it is better to use complementary CDF
      if (a < 0.5) b = distribution_.distributionCollection_[upper_].computeComplementaryCDF(point);
      // Else use CDF
      else
      {
        a = distribution_.distributionCollection_[upper_].computeCDF(point);
        b = distribution_.distributionCollection_[lower_].computeCDF(point);
      }
      if (b > a) value = pdfUpper / (b - a);
    }
    return NumericalPoint(1, value);
  }

  NumericalPoint computePartialFactor(const NumericalPoint & point) const
  {
    return NumericalPoint(1, distribution_.computeFactor(upper_, lowerBound_, point[0]));
  }

  NumericalPoint computePartialExponentialFactor(const NumericalPoint & point) const
  {
    return NumericalPoint(1, distribution_.computeExponentialFactor(upper_, lowerBound_, point[0]));
  }

  const MaximumEntropyOrderStatisticsDistribution & distribution_;
  const UnsignedInteger lower_;
  const UnsignedInteger upper_;
  const NumericalScalar lowerBound_;
}; // struct MaximumEntropyOrderStatisticsDistributionWrapper


/* Compute the exponential factor */
NumericalScalar MaximumEntropyOrderStatisticsDistribution::computeExponentialFactor(const UnsignedInteger k,
    const NumericalScalar x,
    const NumericalScalar y) const
{
  if (y < x)
  {
    const NumericalScalar value(computeExponentialFactor(k, y, x));
    if (value == 0.0) return SpecFunc::MaxNumericalScalar;
    return 1.0 / value;
  }
  // Generic part, no approximation here
  if (x == y)
  {
    const NumericalScalar value(1.0);
    return value;
  }
  const NumericalScalar a(distributionCollection_[k].getRange().getLowerBound()[0]);
  if (y <= a) return 1.0;
  const NumericalScalar b(distributionCollection_[k].getRange().getUpperBound()[0]);
  if (y >= b) return 0.0;
  const NumericalScalar beta(distributionCollection_[k - 1].getRange().getUpperBound()[0]);
  if (x >= beta) return distributionCollection_[k].computeComplementaryCDF(y) / distributionCollection_[k].computeComplementaryCDF(x);
  // Here the computation depends on the use of approximation
  if (!useApproximation_)
  {
    const NumericalScalar factor(computeFactor(k, x, y));
    return std::exp(-factor);
  }
  // Here we know that x < y, y > a, y < b, x < beta
  if (x <= a)
  {
    // x <= a, y > a, y <= beta
    if (y <= beta) return exponentialFactorApproximation_[k - 1](NumericalPoint(1, y))[0];
    // x <= a, y > beta, y < b
    const NumericalScalar ccdfY(distributionCollection_[k].computeComplementaryCDF(y));
    const NumericalScalar ccdfBeta(distributionCollection_[k].computeComplementaryCDF(beta));
    const NumericalScalar rho(ccdfY / ccdfBeta);
    return exponentialFactorApproximation_[k - 1](NumericalPoint(1, beta))[0] * rho;
  }
  // x > a, x < beta
  // y <= beta
  if (y <= beta) return exponentialFactorApproximation_[k - 1](NumericalPoint(1, y))[0] / exponentialFactorApproximation_[k - 1](NumericalPoint(1, x))[0];
  // x > a, y > beta, y < b
  const NumericalScalar ccdfY(distributionCollection_[k].computeComplementaryCDF(y));
  const NumericalScalar ccdfBeta(distributionCollection_[k].computeComplementaryCDF(beta));
  const NumericalScalar rho(ccdfY / ccdfBeta);
  return exponentialFactorApproximation_[k - 1](NumericalPoint(1, beta))[0] / exponentialFactorApproximation_[k - 1](NumericalPoint(1, x))[0] * rho;
}

/* Compute the factor */
NumericalScalar MaximumEntropyOrderStatisticsDistribution::computeFactor(const UnsignedInteger k,
    const NumericalScalar x,
    const NumericalScalar y) const
{
  if (y < x) return -computeFactor(k, y, x);
  // Generic part, no approximation here
  if (x == y)
  {
    const NumericalScalar value(0.0);
    return value;
  }
  const NumericalScalar a(distributionCollection_[k].getRange().getLowerBound()[0]);
  if (y <= a) return 0.0;
  const NumericalScalar b(distributionCollection_[k].getRange().getUpperBound()[0]);
  if (y >= b) return SpecFunc::LogMaxNumericalScalar;
  const NumericalScalar beta(distributionCollection_[k - 1].getRange().getUpperBound()[0]);
  if (x >= beta)
  {
    const NumericalScalar value(std::log(distributionCollection_[k].computeComplementaryCDF(y) / distributionCollection_[k].computeComplementaryCDF(x)));
    return value;
  }
  if (useApproximation_)
  {
    const NumericalScalar exponentialFactor(computeExponentialFactor(k, x, y));
    if (exponentialFactor == 0.0) return SpecFunc::LogMaxNumericalScalar;
    return -std::log(exponentialFactor);
  }
  const MaximumEntropyOrderStatisticsDistributionWrapper phiKWrapper(*this, k - 1, k, a);
  const NumericalMathFunction fPhiK(bindMethod<MaximumEntropyOrderStatisticsDistributionWrapper, NumericalPoint, NumericalPoint>(phiKWrapper, &MaximumEntropyOrderStatisticsDistributionWrapper::computePhi, 1, 1));
  NumericalScalar error;
  // Here we know that x < y, y > a, y < b, x < beta
  if (x <= a)
  {
    // x <= a, y > a, y <= beta
    if (y <= beta)
    {
      const NumericalScalar value(integrator_.integrate(fPhiK, Interval(a, y), error)[0]);
      return value;
    }
    // x <= a, y > beta, y < b
    const NumericalScalar ccdfY(distributionCollection_[k].computeComplementaryCDF(y));
    const NumericalScalar ccdfBeta(distributionCollection_[k].computeComplementaryCDF(beta));
    const NumericalScalar rho(ccdfY / ccdfBeta);
    const NumericalScalar value(integrator_.integrate(fPhiK, Interval(a, beta), error)[0] - std::log(rho));
    return value;
  }
  // x > a, x < beta
  // x > a, y <= beta
  if (y <= beta)
  {
    const NumericalScalar value(integrator_.integrate(fPhiK, Interval(x, y), error)[0]);
    return value;
  }
  // x > a, y > beta, y < b
  const NumericalScalar ccdfY(distributionCollection_[k].computeComplementaryCDF(y));
  const NumericalScalar ccdfBeta(distributionCollection_[k].computeComplementaryCDF(beta));
  const NumericalScalar rho(ccdfY / ccdfBeta);
  const NumericalScalar value(integrator_.integrate(fPhiK, Interval(x, beta), error)[0] - std::log(rho));
  return value;
}

/* Get one realization of the distribution */
NumericalPoint MaximumEntropyOrderStatisticsDistribution::getRealization() const
{
  const UnsignedInteger dimension(getDimension());

  NumericalPoint x(1);
  x[0] = distributionCollection_[0].getRealization()[0];
  for(UnsignedInteger k = 1; k < dimension; ++ k)
  {
    const NumericalScalar xK(computeConditionalQuantile(RandomGenerator::Generate(), x));
    x.add(xK);
  }
  return x;
}

/* Build a C1 interpolation of the exponential factor between the two given marginals */
PiecewiseHermiteEvaluationImplementation MaximumEntropyOrderStatisticsDistribution::interpolateExponentialFactor(const UnsignedInteger lower,
    const UnsignedInteger upper,
    const UnsignedInteger maximumSubdivision,
    const NumericalScalar shift) const
{
  if (lower >= upper) throw InvalidArgumentException(HERE) << "Error: expected lower=" << lower << " to be less than upper=" << upper;
  const NumericalScalar xMin(distributionCollection_[upper].getRange().getLowerBound()[0]);
  const NumericalScalar xMax(distributionCollection_[lower].getRange().getUpperBound()[0]);
  const MaximumEntropyOrderStatisticsDistributionWrapper phiWrapper(*this, lower, upper, xMin);
  const NumericalMathFunction phi(bindMethod<MaximumEntropyOrderStatisticsDistributionWrapper, NumericalPoint, NumericalPoint>(phiWrapper, &MaximumEntropyOrderStatisticsDistributionWrapper::computePartialExponentialFactor, 1, 1));
  NumericalPoint lowerBounds;
  NumericalPoint upperBounds;
  NumericalSample contributions;
  NumericalPoint localErrors;
  NumericalScalar error;
  // We integrate the exponential factor in order to detect all the singularities using polynomial approximations of different order
  const NumericalPoint tmp(GaussKronrod(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-ExponentialFactorDiscretization"), ResourceMap::GetAsNumericalScalar("GaussKronrod-MaximumError"), GaussKronrodRule(GaussKronrodRule::G1K3)).integrate(phi, xMin, xMax, error, lowerBounds, upperBounds, contributions, localErrors));
  // Now, we have to sort the intervals in order to build the approximation
  std::sort(upperBounds.begin(), upperBounds.end());
  // Here we have to subdivide the intervals to take into account the poorer approximation given by Hermite polynomials
  NumericalScalar a(std::abs(xMin) < shift ? shift : xMin + shift * std::abs(xMin));
  NumericalPoint locations(1, a);
  for (UnsignedInteger i = 0; i < upperBounds.getSize(); ++i)
  {
    const NumericalScalar b(upperBounds[i]);
    const NumericalScalar step((b - a) / maximumSubdivision);
    for (UnsignedInteger j = 1; j <= maximumSubdivision; ++j) locations.add(a + j * step);
    a = b;
  }
  const UnsignedInteger size(locations.getSize());
  NumericalPoint values(size);
  NumericalPoint derivatives(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalPoint x(1, locations[i]);
    const NumericalScalar exponentialScalar(phiWrapper.computePartialExponentialFactor(x)[0]);
    values[i] = exponentialScalar;
    derivatives[i] = -phiWrapper.computePhi(x)[0] * exponentialScalar;
  }
  return PiecewiseHermiteEvaluationImplementation(locations, values, derivatives);
}

/* Build a C1 interpolation of the exponential factors in the PDF */
void MaximumEntropyOrderStatisticsDistribution::interpolateExponentialFactors()
{
  // Use exact values to build the approximation
  useApproximation_ = false;
  UnsignedInteger dimension(getDimension());
  exponentialFactorApproximation_ = Collection<PiecewiseHermiteEvaluationImplementation>(dimension - 1);
  const UnsignedInteger maximumSubdivision(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-MaximumApproximationSubdivision"));
  const NumericalScalar shift(ResourceMap::GetAsNumericalScalar("MaximumEntropyOrderStatisticsDistribution-SupportShift"));
  for(UnsignedInteger k = 1; k < dimension; ++k)
  {
    if (!partition_.__contains__(k - 1))
      exponentialFactorApproximation_[k - 1] = interpolateExponentialFactor(k - 1, k, maximumSubdivision, shift);
  } // k
  // Force parallelism here
  isParallel_ = true;
  useApproximation_ = true;
}

/* Get the kth approximation */
PiecewiseHermiteEvaluationImplementation MaximumEntropyOrderStatisticsDistribution::getApproximation(const UnsignedInteger k) const
{
  if (k >= exponentialFactorApproximation_.getSize()) throw InvalidArgumentException(HERE) << "Error: the index=" << k << " must be less than " << exponentialFactorApproximation_.getSize();
  return exponentialFactorApproximation_[k];
}

/* Get the PDF of the distribution */
NumericalScalar MaximumEntropyOrderStatisticsDistribution::computePDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());

  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // Early exit if the point is not in the support
  for (UnsignedInteger k = 1; k < dimension; ++ k)
    if (point[k - 1] > point[k]) return 0.0;
  if (!getRange().numericallyContains(point)) return 0.0;

  // Early exit for the independent case
  if (hasIndependentCopula())
  {
    NumericalScalar pdfValue(distributionCollection_[0].computePDF(point[0]));
    for (UnsignedInteger k = 1; k < dimension; ++k) pdfValue *= distributionCollection_[k].computePDF(point[k]);
    return pdfValue;
  }

  // Here we have to compute something
  NumericalScalar productPDF(distributionCollection_[0].computePDF(point[0]));
  for (UnsignedInteger k = 1; k < dimension; ++k)
  {
    if (!partition_.__contains__(k - 1))
    {
      // Compute the lower bound of the integral. The integrand is zero outside of the range of the kth distribution
      const NumericalScalar xMin(std::max(point[k - 1], distributionCollection_[k].getRange().getLowerBound()[0]));
      // Compute the upper bound of the integral. The integral has a closed form outside of the range of the (k-1)th distribution, but we have still to compute the integral on the intersection with this range
      const NumericalScalar xK(point[k]);
      const NumericalScalar bKm1(distributionCollection_[k - 1].getRange().getUpperBound()[0]);
      NumericalScalar xMax(0.0);
      NumericalScalar cdfKm1(0.0);
      if (bKm1 < xK)
      {
        xMax = bKm1;
        cdfKm1 = 1.0;
      }
      else
      {
        xMax = xK;
        cdfKm1 = distributionCollection_[k - 1].computeCDF(xMax);
      }
      NumericalScalar cdfK(distributionCollection_[k].computeCDF(xMax));
      const NumericalScalar pdfK(distributionCollection_[k].computePDF(point[k]));
      const NumericalScalar exponentialFactor(computeExponentialFactor(k, xMin, xMax));
      productPDF *=  pdfK * exponentialFactor / (cdfKm1 - cdfK);
    } // Partition
  } // Loop over k
  return productPDF;
}


/* Get the log PDF of the distribution */
NumericalScalar MaximumEntropyOrderStatisticsDistribution::computeLogPDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());

  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // Early exit if the point is not in the support
  for (UnsignedInteger k = 1; k < dimension; ++ k)
    if (point[k - 1] > point[k]) return -SpecFunc::MaxNumericalScalar;
  if (!getRange().numericallyContains(point)) return -SpecFunc::MaxNumericalScalar;

  // Early exit for the independent case
  if (hasIndependentCopula())
  {
    NumericalScalar logPDFValue(distributionCollection_[0].computeLogPDF(point[0]));
    for (UnsignedInteger k = 1; k < dimension; ++k) logPDFValue += distributionCollection_[k].computeLogPDF(point[k]);
    return logPDFValue;
  }

  // Here we have to compute something
  NumericalScalar sumLogPDF(distributionCollection_[0].computeLogPDF(point[0]));
  for (UnsignedInteger k = 1; k < dimension; ++k)
  {
    if (!partition_.__contains__(k - 1))
    {
      // Compute the lower bound of the integral. The integrand is zero outside of the range of the kth distribution
      const NumericalScalar xMin(std::max(point[k - 1], distributionCollection_[k].getRange().getLowerBound()[0]));
      // Compute the upper bound of the integral. The integral has a closed form outside of the range of the (k-1)th distribution, but we have still to compute the integral on the intersection with this range
      const NumericalScalar xK(point[k]);
      const NumericalScalar bKm1(distributionCollection_[k - 1].getRange().getUpperBound()[0]);
      NumericalScalar xMax(0.0);
      NumericalScalar cdfKm1(0.0);
      if (bKm1 < xK)
      {
        xMax = bKm1;
        cdfKm1 = 1.0;
      }
      else
      {
        xMax = xK;
        cdfKm1 = distributionCollection_[k - 1].computeCDF(xMax);
      }
      NumericalScalar cdfK(distributionCollection_[k].computeCDF(xMax));
      const NumericalScalar logPDFK(distributionCollection_[k].computeLogPDF(point[k]));
      const NumericalScalar factor(computeFactor(k, xMin, xMax));
      sumLogPDF +=  logPDFK - factor - std::log(cdfKm1 - cdfK);
    } // Partition
  } // Loop over k
  return sumLogPDF;
}


/* Get the CDF of the distribution */
NumericalScalar MaximumEntropyOrderStatisticsDistribution::computeCDF(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  // If there is a miracle: we are in the independent case!
  if (hasIndependentCopula())
  {
    NumericalScalar cdf(distributionCollection_[0].computeCDF(point[0]));
    for (UnsignedInteger k = 1; k < dimension; ++k) cdf *= distributionCollection_[k].computeCDF(point[k]);
    return cdf;
  }
  // Indices of the components to take into account in the computation
  Indices toKeep(0);
  NumericalPoint reducedPoint(0);
  const NumericalPoint lowerBound(getRange().getLowerBound());
  const NumericalPoint upperBound(getRange().getUpperBound());
  for (UnsignedInteger k = 0; k < dimension; ++ k)
  {
    const NumericalScalar xK(point[k]);
    // Early exit if one component is nonpositive
    if (xK <= lowerBound[k]) return 0.0;
    // Keep only the indices for which xK is in (xk_min, xk_max) and xK < xKp1
    // Marginalize the others
    const NumericalScalar bound(k < dimension - 1 ? std::min(point[k + 1], upperBound[k]) : upperBound[k]);
    if (xK < bound)
    {
      toKeep.add(k);
      reducedPoint.add(xK);
    }
  } // k
  // If all the components are greater or equal to their marginal upper bound
  if (toKeep.getSize() == 0)
  {
    return 1.0;
  }
  // If one or more components (but not all) are greater or equal to their marginal upper bound compute a marginal CDF
  if (toKeep.getSize() < dimension)
  {
    const NumericalScalar cdf(getMarginal(toKeep)->computeCDF(reducedPoint));
    return cdf;
  }
  // Else we have to do some work
  // Try to split the work into smaller pieces using potential block-independence
  const UnsignedInteger partitionSize(partition_.getSize());
  if (partitionSize > 0)
  {
    NumericalScalar cdf(1.0);
    UnsignedInteger firstIndex(0);
    for (UnsignedInteger i = 0; i <= partitionSize; ++i)
    {
      const UnsignedInteger lastIndex(i < partitionSize ? partition_[i] + 1 : dimension);
      Indices dependentBlockIndices(lastIndex - firstIndex);
      dependentBlockIndices.fill(firstIndex);
      const UnsignedInteger blockSize(dependentBlockIndices.getSize());
      reducedPoint = NumericalPoint(blockSize);
      for (UnsignedInteger k = 0; k < blockSize; ++k) reducedPoint[k] = point[firstIndex + k];
      // The cdf is obtained by multiplying lower dimensional cdf, which are much more cheaper to compute than a full multidimensional integration
      const Implementation marginal(getMarginal(dependentBlockIndices));
      const NumericalScalar blockCDF(marginal->computeCDF(reducedPoint));
      cdf *= blockCDF;
      firstIndex = lastIndex;
    }
    return cdf;
  }

  // Here we are in the full dependent case. Use Gauss-Legendre integration restricted to the support of the copula.
  // We know that for each k, xk is in (xk_min, xk_max) and for k<dim, xk<xkp1
  NumericalScalar cdf(0.0);

  NumericalPoint gaussWeights;
  const NumericalPoint gaussNodes(getGaussNodesAndWeights(gaussWeights));
  // Perform the integration
  // There are N^{d-1} integration points to compute:
  // I = \int_{lb_1}^{x_1}\int_{\max(t_1, lb_2)}^{x_2}\dots\int_{\max(t_{d-2}, lb_{d-1})}^{x_{d-1}}F(x_d|t_1,\dots,t_{d-1})pdf(t_1,\dots,t_{d-1})dt_1\dots dt_{d-1}
  const UnsignedInteger marginalNodesNumber(getIntegrationNodesNumber());
  const UnsignedInteger size(std::pow(static_cast< NumericalScalar > (marginalNodesNumber), static_cast< NumericalScalar > (dimension - 1)));
  Indices indices(dimension, 0);
  Indices marginalIndices(dimension - 1);
  marginalIndices.fill();
  const NumericalScalar x(point[dimension - 1]);
  Implementation marginal(getMarginal(marginalIndices));
  for (UnsignedInteger linearIndex = 0; linearIndex < size; ++linearIndex)
  {
    NumericalPoint node(dimension - 1);
    const NumericalScalar delta0(0.5 * (point[0] - lowerBound[0]));
    const UnsignedInteger index0(indices[0]);
    node[0] = lowerBound[0] + delta0 * (1.0 + gaussNodes[index0]);
    NumericalScalar weight(delta0 * gaussWeights[index0]);
    for (UnsignedInteger j = 1; j < dimension - 1; ++j)
    {
      const UnsignedInteger indexJ(indices[j]);
      const NumericalScalar aJ(std::max(node[j - 1], distributionCollection_[j].getRange().getLowerBound()[0]));
      const NumericalScalar deltaJ(0.5 * (point[j] - aJ));
      node[j] = aJ + deltaJ * (1.0 + gaussNodes[indexJ]);
      weight *= deltaJ * gaussWeights[indexJ];
    }
    cdf += weight * marginal->computePDF(node) * computeConditionalCDF(x, node);
    /* Update the indices */
    ++indices[0];
    /* Propagate the remainders */
    for (UnsignedInteger j = 0; j < dimension - 2; ++j) indices[j + 1] += (indices[j] == marginalNodesNumber);
    /* Correction of the indices. The last index cannot overflow. */
    for (UnsignedInteger j = 0; j < dimension - 2; ++j) indices[j] = indices[j] % marginalNodesNumber;
  } // Loop over the n-D nodes
  return cdf;
}


NumericalScalar MaximumEntropyOrderStatisticsDistribution::computeCDFOld(const NumericalPoint & point) const
{
  const UnsignedInteger dimension(getDimension());
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  // If there is a miracle: we are in the independent case!
  if (hasIndependentCopula())
  {
    NumericalScalar cdf(distributionCollection_[0].computeCDF(point[0]));
    for (UnsignedInteger k = 1; k < dimension; ++k) cdf *= distributionCollection_[k].computeCDF(point[k]);
    return cdf;
  }
  // Indices of the components to take into account in the computation
  Indices toKeep(0);
  NumericalPoint reducedPoint(0);
  const NumericalPoint lowerBound(getRange().getLowerBound());
  const NumericalPoint upperBound(getRange().getUpperBound());
  for (UnsignedInteger k = 0; k < dimension; ++ k)
  {
    const NumericalScalar xK(point[k]);
    // Early exit if one component is nonpositive
    if (xK <= lowerBound[k]) return 0.0;
    // Keep only the indices for which xK is in (xk_min, xk_max) and xK < xKp1
    // Marginalize the others
    const NumericalScalar bound(k < dimension - 1 ? std::min(point[k + 1], upperBound[k]) : upperBound[k]);
    if (xK < bound)
    {
      toKeep.add(k);
      reducedPoint.add(xK);
    }
  } // k
  // If all the components are greater or equal to their marginal upper bound
  if (toKeep.getSize() == 0)
  {
    return 1.0;
  }
  // If one or more components (but not all) are greater or equal to their marginal upper bound compute a marginal CDF
  if (toKeep.getSize() < dimension)
  {
    const NumericalScalar cdf(getMarginal(toKeep)->computeCDF(reducedPoint));
    return cdf;
  }
  // Else we have to do some work
  // Try to split the work into smaller pieces using potential block-independence
  const UnsignedInteger partitionSize(partition_.getSize());
  if (partitionSize > 0)
  {
    NumericalScalar cdf(1.0);
    UnsignedInteger firstIndex(0);
    for (UnsignedInteger i = 0; i <= partitionSize; ++i)
    {
      const UnsignedInteger lastIndex(i < partitionSize ? partition_[i] + 1 : dimension);
      Indices dependentBlockIndices(lastIndex - firstIndex);
      dependentBlockIndices.fill(firstIndex);
      const UnsignedInteger blockSize(dependentBlockIndices.getSize());
      reducedPoint = NumericalPoint(blockSize);
      for (UnsignedInteger k = 0; k < blockSize; ++k) reducedPoint[k] = point[firstIndex + k];
      // The cdf is obtained by multiplying lower dimensional cdf, which are much more cheaper to compute than a full multidimensional integration
      const Implementation marginal(getMarginal(dependentBlockIndices));
      const NumericalScalar blockCDF(marginal->computeCDF(reducedPoint));
      cdf *= blockCDF;
      firstIndex = lastIndex;
    }
    return cdf;
  }

  // Here we are in the full dependent case. Use Gauss-Legendre integration restricted to the support of the copula.
  // We know that for each k, xk is in (xk_min, xk_max) and for k<dim, xk<xkp1
  NumericalScalar cdf(0.0);

  NumericalPoint gaussWeights;
  const NumericalPoint gaussNodes(getGaussNodesAndWeights(gaussWeights));
  // Perform the integration
  // There are N^{d-1} integration points to compute:
  // I = \int_{lb_1}^{x_1}\int_{\max(t_1, lb_2)}^{x_2}\dots\int_{\max(t_{d-2}, lb_{d-1})}^{x_{d-1}}F(x_d|t_1,\dots,t_{d-1})pdf(t_1,\dots,t_{d-1})dt_1\dots dt_{d-1}
  const UnsignedInteger marginalNodesNumber(getIntegrationNodesNumber());
  const UnsignedInteger size(std::pow(static_cast< NumericalScalar > (marginalNodesNumber), static_cast< NumericalScalar > (dimension)));
  Indices indices(dimension, 0);
  for (UnsignedInteger linearIndex = 0; linearIndex < size; ++linearIndex)
  {
    NumericalPoint node(dimension);
    const NumericalScalar delta0(0.5 * (point[0] - lowerBound[0]));
    const UnsignedInteger index0(indices[0]);
    node[0] = lowerBound[0] + delta0 * (1.0 + gaussNodes[index0]);
    NumericalScalar weight(delta0 * gaussWeights[index0]);
    for (UnsignedInteger j = 1; j < dimension; ++j)
    {
      const UnsignedInteger indexJ(indices[j]);
      const NumericalScalar aJ(std::max(node[j - 1], distributionCollection_[j].getRange().getLowerBound()[0]));
      const NumericalScalar deltaJ(0.5 * (point[j] - aJ));
      node[j] = aJ + deltaJ * (1.0 + gaussNodes[indexJ]);
      weight *= deltaJ * gaussWeights[indexJ];
    }
    cdf += weight * computePDF(node);
    /* Update the indices */
    ++indices[0];
    /* Propagate the remainders */
    for (UnsignedInteger j = 0; j < dimension - 1; ++j) indices[j + 1] += (indices[j] == marginalNodesNumber);
    /* Correction of the indices. The last index cannot overflow. */
    for (UnsignedInteger j = 0; j < dimension - 1; ++j) indices[j] = indices[j] % marginalNodesNumber;
  } // Loop over the n-D nodes
  return cdf;
}


/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar MaximumEntropyOrderStatisticsDistribution::computeConditionalPDF (const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";

  if (conditioningDimension == 0) return distributionCollection_[0].computePDF(x);
  const UnsignedInteger k(conditioningDimension);
  const NumericalScalar aK(getRange().getLowerBound()[k]);
  const NumericalScalar bK(getRange().getUpperBound()[k]);
  // If x is outside of the range of the kth marginal, the conditional PDF is zero
  if ((x <= aK) || (x > bK)) return 0.0;
  // The conditional PDF depends only on the last component of the conditioning vector
  const NumericalScalar xKm1(y[k - 1]);
  // If the conditioning component is greater than the argument the conditional PDF is zero
  if (xKm1 > x) return 0.0;
  // If the conditioning component is outside of the (k-1)th marginal range
  const NumericalScalar aKm1(getRange().getLowerBound()[k - 1]);
  const NumericalScalar bKm1(getRange().getUpperBound()[k - 1]);
  if ((xKm1 <= aKm1) || (xKm1 > bKm1)) return 0.0;
  // Here we have something to do
  // If x is independent of the previous components
  if (partition_.__contains__(k - 1)) return distributionCollection_[k].computePDF(x);
  // Else the difficult case
  // PDF(x|xKm1) = d(1-exp(-\int_{xKm1}^x\phi(s)ds)) / dx
  //             = -d(-\int_{xKm1}^x\phi(s)ds)/dx * exp(-\int_{xKm1}^x\phi(s)ds)
  //             = \phi(x) * exp(-\int_{xKm1}^x\phi(s)ds)
  return distributionCollection_[k].computePDF(x) * computeExponentialFactor(k, xKm1, x) / (distributionCollection_[k - 1].computeCDF(xKm1) - distributionCollection_[k].computeCDF(xKm1));
}


/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
NumericalScalar MaximumEntropyOrderStatisticsDistribution::computeConditionalCDF (const NumericalScalar x,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";

  if (conditioningDimension == 0)
  {
    const NumericalScalar value(distributionCollection_[0].computeCDF(x));
    return value;
  }
  const UnsignedInteger k(conditioningDimension);
  const NumericalScalar aK(getRange().getLowerBound()[k]);
  const NumericalScalar bK(getRange().getUpperBound()[k]);
  // If x is less than the lower bound of its associated marginal, the conditional CDF is zero
  if (x <= aK)
  {
    return 0.0;
  }
  // If x is greater than the upper bound of its associated marginal, the conditional CDF is one
  if (x > bK)
  {
    return 1.0;
  }
  // The conditional CDF depends only on the last component of the conditioning vector
  const NumericalScalar xKm1(y[k - 1]);
  // If the conditioning component is greater than the argument the conditional CDF is zero
  if (xKm1 > x)
  {
    return 1.0;
  }
  // If the conditioning component is outside of the (k-1)th marginal range
  const NumericalScalar aKm1(getRange().getLowerBound()[k - 1]);
  const NumericalScalar bKm1(getRange().getUpperBound()[k - 1]);
  if ((xKm1 <= aKm1) || (xKm1 > bKm1))
  {
    return 0.0;
  }
  // Here we have something to do
  // If x is independent of the previous components
  if (partition_.__contains__(k - 1))
  {
    const NumericalScalar value(distributionCollection_[k].computeCDF(x));
    return value;
  }
  // Else the difficult case
  // CDF(x|xKm1) = 1 - exp(-\int_{xKm1}^x\phi(s)ds)
  const NumericalScalar factor(computeFactor(k, xKm1, x));
  const NumericalScalar value(-expm1(-factor));
  return value;
}


/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
NumericalScalar MaximumEntropyOrderStatisticsDistribution::computeConditionalQuantile(const NumericalScalar q,
    const NumericalPoint & y) const
{
  const UnsignedInteger conditioningDimension(y.getDimension());
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";

  if (conditioningDimension == 0) return distributionCollection_[0].computeQuantile(q)[0];
  const UnsignedInteger k(conditioningDimension);
  if (partition_.__contains__(k - 1))
  {
    return distributionCollection_[k].computeQuantile(q)[0];
  }
  // We have to solve:
  // 1 - exp(-\int_{xKm1}^x\phi(s)ds) = q
  // <->
  // Phi(x) - Phi(xKm1) = -log(1 - q)
  // Factor(x, xKm1) = -log(1 - q)
  const NumericalScalar xKm1(y[k - 1]);
  if (q == 0.0) return xKm1;
  NumericalScalar b(getRange().getUpperBound()[k]);
  if (q == 1.0) return b;
  const NumericalScalar logU(log1p(-q));
  // First, try Newton iterations:
  // Factor(xKm1, x+dx) = -log(1 - q) = Factor(xKm1, x) + f_k(x) / (F_{k-1}(x) - F_k(x)) dk
  // -> dx = (log(1 - q) + Factor(xKm1, x))(F_k(x) - F_{k-1}(x)) / f_k(x)
  NumericalScalar a(xKm1);
  NumericalScalar x(0.5 * (a + b));
  UnsignedInteger iteration(0);
  const UnsignedInteger maximumIteration(ResourceMap::GetAsUnsignedInteger("MaximumEntropyOrderStatisticsDistribution-MaximumQuantileIteration"));
  Bool convergence(false);
  do
  {
    ++iteration;
    const NumericalScalar pdfKX(distributionCollection_[k].computePDF(x));
    if (pdfKX == 0.0) break;
    const NumericalScalar cdfKX(distributionCollection_[k].computeCDF(x));
    const NumericalScalar cdfKm1X(distributionCollection_[k - 1].computeCDF(x));
    const NumericalScalar fX(logU + computeFactor(k, xKm1, x));
    if (fX < 0.0)
    {
      a = x;
    }
    else
    {
      b = x;
    }
    const NumericalScalar delta(fX * (cdfKX - cdfKm1X) / pdfKX);
    x += delta;
    convergence = std::abs(delta) < quantileEpsilon_;
  }
  while (!convergence && (iteration < maximumIteration) && (a <= x) && (x <= b));
  if (convergence) return x;
  // in some cases Newton iteration fails to converge
  const MaximumEntropyOrderStatisticsDistributionWrapper wrapper(*this, k - 1, k, xKm1);
  const NumericalMathFunction f(bindMethod<MaximumEntropyOrderStatisticsDistributionWrapper, NumericalPoint, NumericalPoint>(wrapper, &MaximumEntropyOrderStatisticsDistributionWrapper::computePartialFactor, 1, 1));
  Brent solver(quantileEpsilon_, cdfEpsilon_, cdfEpsilon_, quantileIterations_);
  return solver.solve(f, -logU, a, b);
}


/* Get the i-th marginal distribution */
MaximumEntropyOrderStatisticsDistribution::Implementation MaximumEntropyOrderStatisticsDistribution::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  Distribution marginal(distributionCollection_[i]);
  marginal.setDescription(Description(1, getDescription()[i]));
  return marginal.getImplementation()->clone();
}


/* Get the distribution of the marginal distribution corresponding to indices dimensions */
MaximumEntropyOrderStatisticsDistribution::Implementation MaximumEntropyOrderStatisticsDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger size(indices.getSize());
  if (size == 1) return getMarginal(indices[0]);
  return getMarginalAsMaximumEntropyOrderStatisticsDistribution(indices).clone();
}

MaximumEntropyOrderStatisticsDistribution MaximumEntropyOrderStatisticsDistribution::getMarginalAsMaximumEntropyOrderStatisticsDistribution(const Indices & indices) const
{
  const UnsignedInteger size(indices.getSize());
  if (size < 2) throw InvalidArgumentException(HERE) << "indices must be of size at least 2";
  const UnsignedInteger dimension(getDimension());
  if (!indices.check(dimension - 1)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and  must be different";
  if (!indices.isIncreasing()) throw InvalidArgumentException(HERE) << "Cannot take the marginal distribution of an order statistics distribution with nonincreasing indices.";
  // Here we know that if the size is equal to the dimension, the indices are [0,...,dimension-1]
  if (size == dimension) return *this;
  // This call will check that indices are correct
  DistributionCollection marginalDistributions(size);
  Description marginalDescription(size);
  const Description description(getDescription());
  Collection<PiecewiseHermiteEvaluationImplementation> marginalExponentialFactorApproximation(0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const UnsignedInteger j(indices[i]);
    marginalDistributions[i] = distributionCollection_[j];
    marginalDescription[i] = description[j];
    if (useApproximation_ && (i > 0))
    {
      const UnsignedInteger jPrec(indices[i - 1]);
      if (j == jPrec + 1) marginalExponentialFactorApproximation.add(exponentialFactorApproximation_[j - 1]);
      else
      {
        marginalExponentialFactorApproximation.add(interpolateExponentialFactor(jPrec, j));
      }
    }
  }
  OrderStatisticsMarginalChecker checker(marginalDistributions);
  const Indices marginalPartition(checker.buildPartition());
  MaximumEntropyOrderStatisticsDistribution marginal(marginalDistributions, marginalPartition, useApproximation_, marginalExponentialFactorApproximation, marginalDescription);
  return marginal;
}


/* Distribution collection accessor */
void MaximumEntropyOrderStatisticsDistribution::setDistributionCollection(const DistributionCollection & coll,
    const Bool useApprox,
    const Bool checkMarginals)
{
  OrderStatisticsMarginalChecker checker(coll);
  if (checkMarginals) checker.check();
  partition_ = checker.buildPartition();
  setDimension(coll.getSize());

  // Check if the collection is not empty
  const UnsignedInteger size(coll.getSize());
  if ((getDimension() != 0) && (size != getDimension())) throw InvalidArgumentException(HERE) << "The distribution collection must have a size equal to the distribution dimension";
  Description description(size);
  NumericalPoint lowerBound(size);
  NumericalPoint upperBound(size);
  Interval::BoolCollection finiteLowerBound(size);
  Interval::BoolCollection finiteUpperBound(size);
  if (size == 0) throw InvalidArgumentException(HERE) << "Collection of distributions is empty";
  // First, check that all the marginal distributions are of dimension 1
  isParallel_ = true;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (coll[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "The marginal distribution " << i << " is of dimension " << coll[i].getDimension() << ", which is different from 1.";
    isParallel_ = isParallel_ && coll[i].getImplementation()->isParallel();
    const Interval marginalRange(coll[i].getRange());
    lowerBound[i] = marginalRange.getLowerBound()[0];
    upperBound[i] = marginalRange.getUpperBound()[0];
    finiteLowerBound[i] = marginalRange.getFiniteLowerBound()[0];
    finiteUpperBound[i] = marginalRange.getFiniteUpperBound()[0];
    // The description of the ComposedDistribution is built first by using the marginal description
    // then by using the marginal name if the description is empty, which should never occur
    const String marginalDescription(coll[i].getDescription()[0]);
    if (marginalDescription.size() > 0) description[i] = marginalDescription;
    else
    {
      LOGINFO(OSS() << "Warning: using the name of the marginal " << i << " instead of its description for building the description of the ComposedDistribution, because the marginal description is empty.");
      const String marginalName(coll[i].getName());
      description[i] = marginalName;
    }
  }

  // Everything is ok, store the collection
  distributionCollection_ = coll;
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  setDescription(description);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
  // We must set useApproximation_ to false even if we use approximation, as we need to perform exact computations to build the approximation. The flag is set to the correct value by interpolateExponentialFactors()
  useApproximation_ = false;
  if (useApprox) interpolateExponentialFactors();
}


/* Parameters value and description accessor */
MaximumEntropyOrderStatisticsDistribution::NumericalPointWithDescriptionCollection MaximumEntropyOrderStatisticsDistribution::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  NumericalPointWithDescriptionCollection parameters(dimension);
  const Description description(getDescription());
  // First put the marginal parameters
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
  {
    // Each marginal distribution must output a collection of parameters of size 1, even if it contains an empty NumericalPoint
    const NumericalPointWithDescriptionCollection marginalParameters(distributionCollection_[marginalIndex].getParametersCollection());
    NumericalPointWithDescription point(marginalParameters[0]);
    Description marginalParametersDescription(point.getDescription());
    // Here we must add a unique prefix to the marginal parameters description in order to deambiguate the parameters of different marginals sharing the same description
    for (UnsignedInteger i = 0; i < point.getDimension(); ++i) marginalParametersDescription[i] = (OSS() << marginalParametersDescription[i] << "_marginal_" << marginalIndex);
    point.setDescription(marginalParametersDescription);
    point.setName(description[marginalIndex]);
    parameters[marginalIndex] = point;
  } // marginalIndex
  return parameters;
} // getParametersCollection


void MaximumEntropyOrderStatisticsDistribution::setParametersCollection(const NumericalPointCollection& parametersCollection)
{
  const UnsignedInteger dimension = getDimension();
  if (parametersCollection.getSize() != dimension) throw InvalidArgumentException(HERE) << "The collection is too small(" << parametersCollection.getSize() << "). Expected (" << dimension << ")";

  // set marginal parameters
  for (UnsignedInteger marginalIndex = 0; marginalIndex < dimension; ++marginalIndex)
    distributionCollection_[marginalIndex].setParameter(parametersCollection[marginalIndex]);
}


MaximumEntropyOrderStatisticsDistribution::DistributionCollection MaximumEntropyOrderStatisticsDistribution::getDistributionCollection() const
{
  return distributionCollection_;
}

/* Get the copula of the distribution */
MaximumEntropyOrderStatisticsDistribution::Implementation MaximumEntropyOrderStatisticsDistribution::getCopula() const
{
  return new MaximumEntropyOrderStatisticsCopula(*this);
}

/* Flag to tell if we use approximation for the exponential term */
void MaximumEntropyOrderStatisticsDistribution::useApproximation(const bool flag)
{
  if (flag != useApproximation_)
  {
    useApproximation_ = flag;
    if (flag) interpolateExponentialFactors();
  }
}

/* Tell if the distribution has elliptical copula */
Bool MaximumEntropyOrderStatisticsDistribution::hasEllipticalCopula() const
{
  return hasIndependentCopula();
}

/* Tell if the distribution has independent copula */
Bool MaximumEntropyOrderStatisticsDistribution::hasIndependentCopula() const
{
  return partition_.getSize() == (getDimension() - 1);
}

/* Method save() stores the object through the StorageManager */
void MaximumEntropyOrderStatisticsDistribution::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute("distributionCollection_", distributionCollection_);
}

/* Method load() reloads the object from the StorageManager */
void MaximumEntropyOrderStatisticsDistribution::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  DistributionPersistentCollection coll;
  adv.loadAttribute("distributionCollection_", coll);
  setDistributionCollection(coll);
}


END_NAMESPACE_OPENTURNS
