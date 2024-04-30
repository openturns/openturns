//                                               -*- C++ -*-
/**
 *  @brief The CombinationsDistribution distribution
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
#include "openturns/Collection.hxx"
#include "openturns/CombinationsDistribution.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<UnsignedInteger>     UnsignedIntegerCollection;

CLASSNAMEINIT(CombinationsDistribution)

static const Factory<CombinationsDistribution> Factory_CombinationsDistribution;

/* Default constructor */
CombinationsDistribution::CombinationsDistribution()
  : DiscreteDistribution()
  , k_(0)
  , n_(0)
{
  setName("CombinationsDistribution");
  setKN(1, 1);
}

/* Parameters constructor */
CombinationsDistribution::CombinationsDistribution(const UnsignedInteger k,
    const UnsignedInteger n)
  : DiscreteDistribution()
  , k_(0)
  , n_(0)
{
  setName("CombinationsDistribution");
  setKN(k, n);
}

/* Comparison operator */
Bool CombinationsDistribution::operator ==(const CombinationsDistribution & other) const
{
  if (this == &other) return true;
  return (k_ == other.k_) && (n_ == other.n_);
}

Bool CombinationsDistribution::equals(const DistributionImplementation & other) const
{
  const CombinationsDistribution* p_other = dynamic_cast<const CombinationsDistribution*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String CombinationsDistribution::__repr__() const
{
  OSS oss;
  oss << "class=" << CombinationsDistribution::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " k=" << k_
      << " n=" << n_;
  return oss;
}

String CombinationsDistribution::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(k = " << k_ << ", n = " << n_ << ")";
  return oss;
}

/* Virtual constructor */
CombinationsDistribution * CombinationsDistribution::clone() const
{
  return new CombinationsDistribution(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void CombinationsDistribution::computeRange()
{
  Point lowerBound(k_);
  Point upperBound(k_);
  for (UnsignedInteger i = 0; i < k_; ++i)
    {
      lowerBound[i] = i;
      upperBound[i] = n_ - k_ + i;
    }
  const Interval::BoolCollection finiteLowerBound(k_, true);
  const Interval::BoolCollection finiteUpperBound(k_, true);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution
   See https://cs.stackexchange.com/questions/104930/efficient-n-choose-k-random-sampling
*/
Point CombinationsDistribution::getRealization() const
{
  Indices flags(n_);
  Indices integralRealization;
  Bool done = false;
  // Build the complementary subset if its cardinal is smaller than k_
  const UnsignedInteger actualK = (k_ > n_ / 2 ? n_ - k_ : k_);
  while (integralRealization.getSize() < actualK)
    {
      const UnsignedInteger i = RandomGenerator::IntegerGenerate(n_);
      if (flags[i] == 0)
        {
          integralRealization.add(i);
          flags[i] = 1;
        }
    } // while
  // Di I built the complementary set?
  if (actualK != k_)
    integralRealization = integralRealization.complement(n_);
  // The realization must be sorted in ascending order
  std::sort(integralRealization.begin(), integralRealization.end());
  Point realization(k_);
  for (UnsignedInteger i = 0; i < k_; ++i)
    realization[i] = integralRealization[i];
  return realization;
}

/* Get the PDF of the distribution */
Scalar CombinationsDistribution::computeLogPDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  Indices x(k_);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar k = point[i];
    if ((k < i - supportEpsilon_) || (k > n_ - k_ + i + supportEpsilon_)) return SpecFunc::LowestScalar;
    const UnsignedInteger ik = static_cast< UnsignedInteger > (round(k));
    if (std::abs(k - ik) > supportEpsilon_) return SpecFunc::LowestScalar;
    x[i] = ik;
  }
  if (!x.isStrictlyIncreasing()) return SpecFunc::LowestScalar;
  return logPDFValue_;
}

Scalar CombinationsDistribution::computePDF(const Point & point) const
{
  const Scalar logPDF = computeLogPDF(point);
  if (logPDF == SpecFunc::LowestScalar) return 0.0;
  return std::exp(logPDF);
}

namespace
{
// The free functions are embedded into an anonymous namespace and are prefixed by the class name,
// see Mesh.cxx
Scalar CombinationsDistribution_exploreTree(const UnsignedInteger k,
                                            const UnsignedInteger lower,
                                            const UnsignedInteger upper,
                                            const UnsignedInteger count,
                                            const Point & xReduced)
{
  const UnsignedInteger n = xReduced.getDimension();
  // Upper branch of the tree
  const UnsignedInteger lower1 = lower;
  const UnsignedInteger upper1 = upper;
  // The value lower1 == dimension is a guard, telling us that the lower bound is 0 and not a component of xReduced
  const Scalar a1 = (lower1 < xReduced.getDimension()) ? xReduced[lower1] : 0.0;
  const Scalar b1 = xReduced[upper1];
  const UnsignedInteger count1 = count + 1;
  const Scalar f1 = ((b1 - a1 - count1 + 1) / count1) * ((k - 1) / (n - k));
  // Lower branch of the tree
  const UnsignedInteger lower2 = upper;
  const UnsignedInteger upper2 = k;
  const Scalar a2 = xReduced[lower2];
  const Scalar b2 = xReduced[upper2];
  const UnsignedInteger count2 = 1;
  const Scalar f2 = b2 - a2;
  if (k == n - 1)
    return (k + 1) * (f1 + f2);
  Scalar value = 0.0;
  // This test allows one to cut upper parts of the tree
  if (f1 > 0.0)
    value += f1 * CombinationsDistribution_exploreTree(k + 1, lower1, upper1, count1, xReduced);
  // This test allows one to cut lower parts of the tree
  if (f2 > 0.0)
    value += f2 * CombinationsDistribution_exploreTree(k + 1, upper, k, count2, xReduced);
  return value * (k + 1);
}
} // Anonymous namespace

/* Get the CDF of the distribution */
Scalar CombinationsDistribution::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  Point sortedPoint(dimension);
  Scalar oldX = n_ - 1.0;
  if (sortedPoint[dimension - 1] < -supportEpsilon_) return 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar x = std::min(oldX, floor(point[i] + supportEpsilon_));
    if (x < -supportEpsilon_) return 0.0;
    sortedPoint[i] = x;
    oldX = x - 1.0;
  }
  // Explore the tree describing the domain of summation
  // k = 1
  // lower = dimension_ (guard value to tell that the current interval is [0, xReduced[0]])
  // upper = 0
  // count = 1
  return sortedPoint[0] * CombinationsDistribution_exploreTree(1, dimension_, 0, 1, sortedPoint);
}

/* Compute the scalar quantile of the 1D CombinationsDistribution distribution */
Scalar CombinationsDistribution::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  const UnsignedInteger i = static_cast< UnsignedInteger >(ceil(prob * (n_ - 1.0)));
  return (tail ? n_ - 1.0 - i : i);
} // computeScalarQuantile

/* Compute the quantile of the CombinationsDistribution distribution */
Point CombinationsDistribution::computeQuantile(const Scalar prob,
    const Bool tail,
    Scalar & marginalProb) const
{
  const Scalar p = (tail ? 1.0 - prob : prob);
  if (p <= 0.0) return Point(k_, 0.0);
  if (p >= 1.0) return Point(k_, n_);
  UnsignedInteger iMin = 0;
  UnsignedInteger iMax = n_;
  while (iMax > iMin + 1)
  {
    const UnsignedInteger iMiddle = (iMax + iMin) / 2;
    const Scalar cdfMiddle = computeCDF(Point(k_, iMiddle));
    if (cdfMiddle < p)
    {
      iMin = iMiddle;
    }
    else
    {
      iMax = iMiddle;
    }
  } // while
  marginalProb = computeScalarQuantile(prob, tail);
  return Point(k_, iMax);
} // computeQuantile

/* Get the i-th marginal distribution */
Distribution CombinationsDistribution::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  CombinationsDistribution::Implementation marginal(new CombinationsDistribution(1, n_));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution CombinationsDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  CombinationsDistribution::Implementation marginal(new CombinationsDistribution(outputDimension, n_));
  marginal->setDescription(getDescription().select(indices));
  return marginal;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample CombinationsDistribution::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  // Convert int values into float
  const IndicesCollection intResult(Combinations(k_, n_).generate());
  const UnsignedInteger size = intResult.getSize();
  if (size == 0) return Sample();
  const Interval inter(interval.intersect(range_));
  // Common case: get the full support
  if (inter == range_)
    {
      Sample result(size, dimension_);
      for (UnsignedInteger i = 0; i < size; ++i)
        for (UnsignedInteger j = 0; j < dimension_; ++j)
            result(i, j) = intResult(i, j);
      return result;
    }
  Sample result(0, dimension_);
  for (UnsignedInteger i = 0; i < size; ++i)
    {
      Point point(dimension_);
      for (UnsignedInteger j = 0; j < dimension_; ++j)
        point[j] = intResult(i, j);
      if (inter.contains(point))
        result.add(point);
    }
  return result;
}

/* Compute the mean of the distribution */
void CombinationsDistribution::computeMean() const
{
  //mean_ = Point(k_, 0.5 * (n_ - 1.0));
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void CombinationsDistribution::computeCovariance() const
{
  //const Scalar var = (n_ * n_ - 1.0) / 12.0;
  //const Scalar cov = -(n_ + 1.0) / 12.0;
  //covariance_ = CovarianceMatrix(k_, Point(k_ * k_, cov));
  //for (UnsignedInteger i = 0; i < k_; ++i) covariance_(i, i) = var;
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
CombinationsDistribution::PointWithDescriptionCollection CombinationsDistribution::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters((dimension == 1 ? 1 : dimension + 1));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription point(1);
    point[0] = n_;
    Description description(1);
    description[0] = "n";
    point.setDescription(description);
    point.setName(getDescription()[i]);
    parameters[i] = point;
  }
  if (dimension > 1)
  {
    PointWithDescription point(2);
    Description description = {"k", "n"};
    point[0] = k_;
    point[1] = n_;
    point.setDescription(description);
    point.setName("dependence");
    parameters[dimension] = point;
  }
  return parameters;
}

/* K accessor */
void CombinationsDistribution::setK(const UnsignedInteger k)
{
  if (k == 0) throw InvalidArgumentException(HERE) << "Error: k must be > 0.";
  if (k > n_) throw InvalidArgumentException(HERE) << "Error: k must be less or equal to n, here k=" << k << " and n=" << n_;
  if (k != k_)
  {
    k_ = k;
    logPDFValue_ = SpecFunc::LogGamma(n_ - k_ + 1) - SpecFunc::LogGamma(n_ + 1) + SpecFunc::LogGamma(k_ + 1);
    setDimension(k);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
  }
}

/* K accessor */
UnsignedInteger CombinationsDistribution::getK() const
{
  return k_;
}

/* N accessor */
void CombinationsDistribution::setN(const UnsignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0.";
  if (n < k_) throw InvalidArgumentException(HERE) << "Error: n must be greater or equal to k, here n=" << n << " and k=" << k_;
  if (n != n_)
  {
    n_ = n;
    logPDFValue_ = SpecFunc::LogGamma(n_ - k_ + 1) - SpecFunc::LogGamma(n_ + 1) + SpecFunc::LogGamma(k_ + 1);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

UnsignedInteger CombinationsDistribution::getN() const
{
  return n_;
}

/* K/N accessor */
void CombinationsDistribution::setKN(const UnsignedInteger k,
                                      const UnsignedInteger n)
{
  if (k == 0) throw InvalidArgumentException(HERE) << "Error: k must be > 0.";
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0.";
  if (k > n) throw InvalidArgumentException(HERE) << "Error: k must be less or equal to n, here k=" << k << " and n=" << n;
  k_ = k;
  setDimension(k);
  n_ = n;
  logPDFValue_ = SpecFunc::LogGamma(n_ - k_ + 1) - SpecFunc::LogGamma(n_ + 1) + SpecFunc::LogGamma(k_ + 1);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

/* Method save() stores the object through the StorageManager */
void CombinationsDistribution::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "k_", k_ );
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "logPDFValue_", logPDFValue_ );
}

/* Method load() reloads the object from the StorageManager */
void CombinationsDistribution::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "k_", k_ );
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "logPDFValue_", logPDFValue_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
