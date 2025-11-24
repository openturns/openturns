//                                               -*- C++ -*-
/**
 *  @brief The CombinationsDistribution distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/UserDefined.hxx"
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
  : DistributionImplementation()
{
  setName("CombinationsDistribution");
  setKN(1, 1);
}

/* Parameters constructor */
CombinationsDistribution::CombinationsDistribution(const UnsignedInteger k,
    const UnsignedInteger n)
  : DistributionImplementation()
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
  // Did I built the complementary set?
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
  if (point.getDimension() != k_) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << k_ << ", here dimension=" << point.getDimension();
  Indices x(k_);
  for (UnsignedInteger i = 0; i < k_; ++i)
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

Scalar CombinationsDistribution::exploreTree(const Scalar j,
    const UnsignedInteger lower,
    const UnsignedInteger upper,
    const UnsignedInteger count,
    const Point & xReduced) const
{
  const Scalar normalizationProba = (k_ - j) / (n_ - j);
  // Upper branch of the tree
  const UnsignedInteger lower1 = lower;
  const UnsignedInteger upper1 = upper;
  // The value lower1 == dimension is a guard, telling us that the lower bound is 0 and not a component of xReduced
  const Scalar a1 = (lower1 < xReduced.getDimension()) ? xReduced[lower1] : -1.0;
  const Scalar b1 = xReduced[upper1];
  const UnsignedInteger count1 = count;
  const Scalar f1 = (b1 - a1 - count1) / (count1 + 1);
  // Lower branch of the tree
  const UnsignedInteger lower2 = upper;
  const UnsignedInteger upper2 = j;
  const Scalar a2 = xReduced[lower2];
  const Scalar b2 = xReduced[upper2];
  const UnsignedInteger count2 = 1;
  const Scalar f2 = b2 - a2;
  if (j == k_ - 1)
    return (f1 + f2) * normalizationProba;
  Scalar value = 0.0;
  // This test allows one to cut upper parts of the tree
  if (f1 > 0.0)
    value += f1 * exploreTree(j + 1, lower1, upper1, count1 + 1, xReduced);
  // This test allows one to cut lower parts of the tree
  if (f2 > 0.0)
    value += f2 * exploreTree(j + 1, lower2, upper2, count2, xReduced);
  return value * normalizationProba;
}

/* Get the CDF of the distribution */
Scalar CombinationsDistribution::computeCDF(const Point & point) const
{
  if (point.getDimension() != k_) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << k_ << ", here dimension=" << point.getDimension();

  Point sortedPoint(k_);
  Scalar oldX = n_ - 1.0;
  if (sortedPoint[k_ - 1] < -supportEpsilon_) return 0.0;
  for (SignedInteger i = k_ - 1; i >= 0; --i)
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
  return ((sortedPoint[0] + 1.0) * k_) / n_ * exploreTree(1, k_, 0, 1, sortedPoint);
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
Distribution CombinationsDistribution::getMarginal(const UnsignedInteger index) const
{
  if (index >= k_) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  Sample support(n_ - k_ + 1, 1);
  // First, compute the probabilities on a log scale
  Point probabilities(n_ - k_ + 1, -SpecFunc::LogBinomialCoefficient(n_, k_));
  for (UnsignedInteger x = index; x <= n_ - k_ + index; ++x)
  {
    support(x - index, 0) = x;
    probabilities[x - index] += SpecFunc::LogBinomialCoefficient(x, index) + SpecFunc::LogBinomialCoefficient(n_ - 1 - x, k_ - 1 - index);
  }
  // Then, go back to the [0, 1] interval
  for (UnsignedInteger j = 0; j <= n_ - k_; ++j)
    probabilities[j] = SpecFunc::Clip01(std::exp(probabilities[j]));
  UserDefined marginal(support, probabilities);
  marginal.setDescription(Description(1, getDescription()[index]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution CombinationsDistribution::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // Special case for indices of length 1
  if (indices.getSize() == 1) return getMarginal(indices[0]);
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  const Sample support(getSupport().getMarginal(indices));
  Point probabilities(support.getSize());
  for (UnsignedInteger j = 0; j < support.getSize(); ++j)
  {
    const UnsignedInteger j0 = indices[0];
    const UnsignedInteger x0 = static_cast<UnsignedInteger>(support(j, 0));
    const UnsignedInteger je = indices[outputDimension - 1];
    const UnsignedInteger xe = static_cast<UnsignedInteger>(support(j, outputDimension - 1));
    probabilities[j] += SpecFunc::LogBinomialCoefficient(x0, j0) + SpecFunc::LogBinomialCoefficient(n_ - xe - 1, k_ - je - 1);
    for (UnsignedInteger i = 1; i < outputDimension - 1; ++i)
      probabilities[j] += SpecFunc::LogBinomialCoefficient(xe, je);
    probabilities[j] = SpecFunc::Clip01(std::exp(probabilities[j]));
  }
  UserDefined marginal(support, probabilities);
  marginal.setDescription(getDescription().select(indices));
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
  mean_ = Point(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    mean_[i] = getMarginal(i).getMean()[0];
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void CombinationsDistribution::computeCovariance() const
{
  covariance_ = CovarianceMatrix(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
  {
    for (UnsignedInteger i = 0; i < j; ++i)
    {
      const CovarianceMatrix covIJ(getMarginal({i, j}).getCovariance());
      covariance_(i, j) = covIJ(0, 1);
      covariance_(j, j) = covIJ(1, 1);
      covariance_(i, i) = covIJ(0, 0);
    }
  }
  isAlreadyComputedCovariance_ = true;
}

/* Compute the Spearman correlation of the distribution */
CorrelationMatrix CombinationsDistribution::getSpearmanCorrelation() const
{
  CorrelationMatrix spearman(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    for (UnsignedInteger i = 0; i < j; ++i)
      spearman(i, j) = getMarginal({i, j}).getSpearmanCorrelation()(0, 1);
  return spearman;
}

/* Compute the Kendall concordance of the distribution */
CorrelationMatrix CombinationsDistribution::getKendallTau() const
{
  CorrelationMatrix kendall(dimension_);
  for (UnsignedInteger j = 0; j < dimension_; ++j)
    for (UnsignedInteger i = 0; i < j; ++i)
      kendall(i, j) = getMarginal({i, j}).getKendallTau()(0, 1);
  return kendall;
}

/* Check if the distribution is continuous */
Bool CombinationsDistribution::isContinuous() const
{
  return false;
}

/* Check if the distribution is discrete */
Bool CombinationsDistribution::isDiscrete() const
{
  return true;
}

/* Tell if the distribution is integer valued */
Bool CombinationsDistribution::isIntegral() const
{
  return true;
}

/* Parameters value and description accessor */
CombinationsDistribution::PointWithDescriptionCollection CombinationsDistribution::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters((dimension == 1 ? 1 : dimension + 1));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription point({static_cast<Scalar>(n_)});
    point.setDescription({"n"});
    point.setName(getDescription()[i]);
    parameters[i] = point;
  }
  if (dimension > 1)
  {
    PointWithDescription point(getParameter());
    point.setDescription(getParameterDescription());
    point.setName("dependence");
    parameters[dimension] = point;
  }
  return parameters;
}

/* Parameters value accessor */
Point CombinationsDistribution::getParameter() const
{
  return {static_cast<Scalar>(k_), static_cast<Scalar>(n_)};
}

void CombinationsDistribution::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "CombinationsDistribution expected 2 parameters, got " << parameter.getSize();
  if (parameter[0] != std::round(parameter[0])) throw InvalidArgumentException(HERE) << "CombinationsDistribution first parameter k must be an integer, got " << parameter[0];
  if (parameter[1] != std::round(parameter[1])) throw InvalidArgumentException(HERE) << "CombinationsDistribution second parameter n must be an integer, got " << parameter[1];
  const Scalar w = getWeight();
  *this = CombinationsDistribution(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description CombinationsDistribution::getParameterDescription() const
{
  return {"k", "n"};
}

void CombinationsDistribution::update()
{
  logPDFValue_ = -1.0 * SpecFunc::LogBinomialCoefficient(n_, k_);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
}

/* K accessor */
void CombinationsDistribution::setK(const UnsignedInteger k)
{
  if (k == 0) throw InvalidArgumentException(HERE) << "Error: k must be > 0.";
  if (k > n_) throw InvalidArgumentException(HERE) << "Error: k must be less or equal to n, here k=" << k << " and n=" << n_;
  if (k != k_)
  {
    k_ = k;
    setDimension(k);
    update();
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
    update();
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
  update();
}

/* Method save() stores the object through the StorageManager */
void CombinationsDistribution::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "k_", k_ );
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "logPDFValue_", logPDFValue_ );
}

/* Method load() reloads the object from the StorageManager */
void CombinationsDistribution::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "k_", k_ );
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "logPDFValue_", logPDFValue_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
