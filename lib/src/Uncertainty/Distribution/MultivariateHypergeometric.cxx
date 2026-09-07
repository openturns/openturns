//                                               -*- C++ -*-
/**
 *  @brief The MultivariateHypergeometric distribution
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
#include <functional>
#include "openturns/Collection.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/MultivariateHypergeometric.hxx"
#include "openturns/Hypergeometric.hxx"
#include "openturns/FiniteDiscreteDistribution.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearEnumerateFunction.hxx"
#include "PoissonFormula.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<UnsignedInteger>     UnsignedIntegerCollection;

CLASSNAMEINIT(MultivariateHypergeometric)

static const Factory<MultivariateHypergeometric> Factory_MultivariateHypergeometric;

/* Default constructor */
MultivariateHypergeometric::MultivariateHypergeometric()
  : DistributionImplementation()
  , n_(0)
  , colors_(0)
  , total_(0.0)
  , t_(0.0)
  , normalizationCDF_(0.0)
  , r_(0.0)
  , smallA_(ResourceMap::GetAsScalar("MultivariateHypergeometric-smallA"))
  , eta_(ResourceMap::GetAsScalar("MultivariateHypergeometric-eta"))
{
  setName("MultivariateHypergeometric");
  // This method compute the range also
  setN(1);
  setColors( Point(1, 2.0) );
}

/* Parameters constructor */
MultivariateHypergeometric::MultivariateHypergeometric(const UnsignedInteger n,
                                                       const Point & colors)
  : DistributionImplementation()
  , n_(0)
  , colors_(0)
  , total_(0.0)
  , t_(0.0)
  , normalizationCDF_(0.0)
  , r_(0.0)
  , smallA_(ResourceMap::GetAsScalar("MultivariateHypergeometric-smallA"))
  , eta_(ResourceMap::GetAsScalar("MultivariateHypergeometric-eta"))
{
  setName("MultivariateHypergeometric");
  // This method compute the range also
  setN( n );
  setColors( colors );
  computeRange();
}

/* Tell if the distribution is continuous */
Bool MultivariateHypergeometric::isContinuous() const
{
  return false;
}

/* Tell if the distribution is discrete */
Bool MultivariateHypergeometric::isDiscrete() const
{
  return true;
}

/* Tell if the distribution is integer valued */
Bool MultivariateHypergeometric::isIntegral() const
{
  return true;
}

/* Comparison operator */
Bool MultivariateHypergeometric::operator ==(const MultivariateHypergeometric & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (n_ == other.n_) && (colors_ == other.colors_);
}

/* Comparison operator */
Bool MultivariateHypergeometric::equals(const DistributionImplementation & other) const
{
  const MultivariateHypergeometric* p_other = dynamic_cast<const MultivariateHypergeometric*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String MultivariateHypergeometric::__repr__() const
{
  OSS oss;
  oss << "class=" << MultivariateHypergeometric::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " colors=" << colors_
      << " n=" << n_;
  return oss;
}

String MultivariateHypergeometric::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(n = " << n_ << ", colors = " << colors_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
MultivariateHypergeometric * MultivariateHypergeometric::clone() const
{
  return new MultivariateHypergeometric(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void MultivariateHypergeometric::computeRange()
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger colorsSize = colors_.getDimension();
  // The number of draws may be set before the population counts, in which case
  // the range is left empty until setColors fills the colors vector
  Point lowerBound(dimension);
  Point upperBound(dimension);
  const UnsignedInteger breakpoint = std::min(dimension, colorsSize);
  for (UnsignedInteger i = 0; i < breakpoint; ++i)
  {
    lowerBound[i] = std::max(0.0, 1.0 * n_ - (total_ - colors_[i]));
    upperBound[i] = std::min(colors_[i], 1.0 * n_);
  }
  const Interval::BoolCollection finiteLowerBound(dimension, true);
  const Interval::BoolCollection finiteUpperBound(dimension, true);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
Point MultivariateHypergeometric::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  Point realization(dimension);
  /* We use an elementary algorithm based on the definition of the MultivariateHypergeometric
   * distribution: the i-th component is generated using a Hypergeometric distribution */
  UnsignedInteger remainingN = n_;
  Scalar remainingTotal = total_;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    /* The current component follows a hypergeometric distribution with parameters
     * colors_[i] among the remaining balls */
    const UnsignedInteger xI = DistFunc::rHypergeometric(static_cast<UnsignedInteger>(remainingTotal), static_cast<UnsignedInteger>(colors_[i]), remainingN);
    realization[i] = xI;
    remainingN -= xI;
    if (remainingN == 0) break;
    remainingTotal -= colors_[i];
  }
  return realization;
}

/* Get a sample of the distribution */
Sample MultivariateHypergeometric::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger dimension = getDimension();
  SampleImplementation sample(size, dimension);
  UnsignedInteger remainingN = 0;
  Scalar remainingTotal = 0.0;
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    /* We use an elementary algorithm based on the definition of the MultivariateHypergeometric
     * distribution: the i-th component is generated using a Hypergeometric distribution */
    remainingN = n_;
    remainingTotal = total_;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const UnsignedInteger xI = DistFunc::rHypergeometric(static_cast<UnsignedInteger>(remainingTotal), static_cast<UnsignedInteger>(colors_[i]), remainingN);
      sample(k, i) = xI;
      remainingN -= xI;
      if (remainingN == 0) break;
      remainingTotal -= colors_[i];
    }
  }
  sample.setName(getName());
  sample.setDescription(getDescription());
  return sample;
}

/* Get the PDF of the distribution */
Scalar MultivariateHypergeometric::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getSize();

  UnsignedInteger sumX = 0;
  Indices x(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar xRounded = round(point[i]);
    if ((xRounded < 0.0) || (std::abs(xRounded - point[i]) > supportEpsilon_)) return 0.0;
    x[i] = static_cast< UnsignedInteger >(xRounded);
    if (x[i] > colors_[i]) return 0.0;
    sumX += x[i];
  }
  // The sum of the components must be exactly n_
  if (sumX != n_) return 0.0;
  Scalar logPDF = -(SpecFunc::LogGamma(total_ + 1.0) - SpecFunc::LogGamma(n_ + 1.0) - SpecFunc::LogGamma(total_ - n_ + 1.0));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar colorI = colors_[i];
    const Scalar xI = 1.0 * x[i];
    logPDF += SpecFunc::LogGamma(colorI + 1.0) - SpecFunc::LogGamma(xI + 1.0) - SpecFunc::LogGamma(colorI - xI + 1.0);
  }
  return std::exp(logPDF);
}

/* Compute the generating function of a sum of truncated Binomial distributions as needed in the computeCDF() method */
Complex MultivariateHypergeometric::computeGlobalPhi(const Complex & z,
                                                     const Indices & x) const
{
  Complex value(1.0);
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar colorI = colors_[i];
    if (colorI == 0.0) continue;
    value *= computeLocalPhi(z, colorI, t_, x[i]);
    if (std::abs(value) == 0.0)
    {
      LOGWARN("Underflow in MultivariateHypergeometric::computeGlobalPhi");
      return 0.0;
    }
  }
  return value;
}

/* Compute the generating function of a sum of truncated Binomial distributions as needed in the computeProbability() method */
Complex MultivariateHypergeometric::computeGlobalPhi(const Complex & z,
                                                     const Indices & a,
                                                     const Indices & b) const
{
  Complex value(1.0);
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar colorI = colors_[i];
    if (colorI == 0.0) continue;
    value *= computeLocalPhi(z, colorI, t_, a[i], b[i]);
    if (std::abs(value) == 0.0)
    {
      LOGWARN("Underflow in MultivariateHypergeometric::computeGlobalPhi");
      return 0.0;
    }
  }
  return value;
}

/* Compute the generating function of a truncated Binomial distribution as needed in the computeCDF() method */
Complex MultivariateHypergeometric::computeLocalPhi(const Complex & z,
                                                    const Scalar c,
                                                    const Scalar t,
                                                    const UnsignedInteger a) const
{
  return PoissonFormula::localPhi0(z, PoissonFormula::BinomialKernel(c, t, z), a, smallA_);
}

/* Compute the generating function of a truncated Binomial distribution as needed in the computeProbability() method */
Complex MultivariateHypergeometric::computeLocalPhi(const Complex & z,
                                                    const Scalar c,
                                                    const Scalar t,
                                                    const UnsignedInteger a,
                                                    const UnsignedInteger b) const
{
  return PoissonFormula::localPhi(z, PoissonFormula::BinomialKernel(c, t, z), a, b, smallA_);
}

/* Get the CDF of the distribution
   Algorithm described in:
   R. Lebrun, "Efficient time/space algorithm to compute rectangular probabilities of multinomial, multivariate hypergeometric and multivariate Polya distributions", Statistics and Computing, submitted (2011).
*/
Scalar MultivariateHypergeometric::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  // Early exit for 1D case
  if (dimension == 1)
  {
    const Scalar k = point[0];
    const Scalar c = colors_[0];
    // Deterministic case
    if (total_ == n_) return (k >= c - supportEpsilon_ ? 1.0 : 0.0);
    if (c == total_) return (k >= n_ - supportEpsilon_ ? 1.0 : 0.0);
    if (c == 0.0) return (k >= 0.0 ? 1.0 : 0.0);
    // Support of the marginal: [max(0, c + n_ - total_), min(c, n_)]
    if (k < c + n_ - total_ - supportEpsilon_) return 0.0;
    if (k >= std::min(c, 1.0 * n_) + supportEpsilon_) return 1.0;
    if (k < -supportEpsilon_) return 0.0;
    return DistFunc::pHypergeometric(static_cast<UnsignedInteger>(total_), static_cast<UnsignedInteger>(c), n_, static_cast<UnsignedInteger>(std::max(0.0, std::min(floor(k), std::min(c, 1.0 * n_)))), false);
  }
  // First, check the trivial cases
  Bool allZero = true;
  UnsignedInteger sumX = 0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If the given point does not cover any point of the support, return 0.0
    if (point[i] < 0.0) return 0.0;
    allZero = allZero && (point[i] <= supportEpsilon_);
  }
  // Clamp each component to [0, min(colors_[i], n_)] to avoid overflow on UnsignedInteger cast
  Point clamped(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    clamped[i] = std::min(point[i], std::min(colors_[i], 1.0 * n_));
  const Indices kPoint(clamped.begin(), clamped.end());
  for (UnsignedInteger i = 0; i < dimension; ++i)
    sumX += kPoint[i];
  // If we are at the origin, CDF = probability of the zero vector, which is 0 as n_ >= 1
  if (allZero) return 0.0;
  // If the atoms with non zero probability sum to n_
  if (sumX == n_)
  {
    // Only the atom floor(point) lies inside the box, so we return
    // PDF(floor(point)) rather than PDF(point)
    Point integerPoint(dimension);
    for (UnsignedInteger i = 0; i < dimension; ++i)
      integerPoint[i] = kPoint[i];
    return computePDF(integerPoint);
  }
  // If the box upper bounds sum to less than n_, no atom of the simplex lies inside the box
  if (sumX < n_) return 0.0;
  // If the point covers the whole support of the distribution, return 1.0
  Bool fullCoverage = true;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    fullCoverage = fullCoverage && (kPoint[i] >= std::min(colors_[i], 1.0 * n_));
    if (!fullCoverage) break;
  }
  if (fullCoverage) return 1.0;
  // Evaluation of P(W=n) using Poisson's formula
  return PoissonFormula::computeCDF([&](const Complex & z) { return computeGlobalPhi(z, kPoint); },
                                    n_, r_, normalizationCDF_, getClassName());
}

/* Compute the probability content of an interval */
Scalar MultivariateHypergeometric::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  // Early exit for 1D case
  if (dimension == 1)
  {
    const Scalar a = interval.getLowerBound()[0];
    const Scalar b = interval.getUpperBound()[0];
    const Scalar c = colors_[0];
    // Deterministic case
    if (total_ == n_) return (a <= c + supportEpsilon_ && b >= c - supportEpsilon_) ? 1.0 : 0.0;
    if (c == total_) return (a <= n_ + supportEpsilon_ && b >= n_ - supportEpsilon_) ? 1.0 : 0.0;
    if (c == 0.0) return (a <= 0.0 && b >= 0.0) ? 1.0 : 0.0;
    const UnsignedInteger kMin = static_cast< UnsignedInteger >(std::max(0.0, c + n_ - total_));
    const UnsignedInteger kMax = static_cast< UnsignedInteger >(std::min(c, 1.0 * n_));
    if ((a > kMax + supportEpsilon_) || (b < kMin - supportEpsilon_)) return 0.0;
    const Scalar bFloor = std::max(0.0, std::min(floor(b), 1.0 * kMax));
    Scalar probability = DistFunc::pHypergeometric(static_cast<UnsignedInteger>(total_), static_cast<UnsignedInteger>(c), n_, static_cast<UnsignedInteger>(bFloor), false);
    if (a > 0.0)
    {
      const Scalar aFloor = std::ceil(a) - 1.0;
      if (aFloor >= kMin)
        probability -= DistFunc::pHypergeometric(static_cast<UnsignedInteger>(total_), static_cast<UnsignedInteger>(c), n_, static_cast<UnsignedInteger>(aFloor), false);
    }
    return probability;
  }
  Point lower(interval.getLowerBound());
  Point upper(interval.getUpperBound());
  // Box bounds with respect to the support of each component
  Indices a(dimension_);
  Indices b(dimension_);
  Point aP(dimension_);
  Point bP(dimension_);
  UnsignedInteger sigmaA = 0;
  UnsignedInteger sigmaB = 0;
  Bool fullCoverage = true;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    const Scalar upperI = std::min(upper[i], std::min(colors_[i], 1.0 * n_));
    aP[i] = std::max(0.0, std::ceil(lower[i]));
    bP[i] = floor(upperI);
    if (bP[i] < -supportEpsilon_) return 0.0;
    // Compare in Scalar space before unsigned cast to avoid overflow on huge values
    if (aP[i] > bP[i] + supportEpsilon_) return 0.0;
    a[i] = static_cast<UnsignedInteger>(aP[i]);
    b[i] = static_cast<UnsignedInteger>(bP[i]);
    if (a[i] > b[i]) return 0.0;
    sigmaA += a[i];
    sigmaB += b[i];
    fullCoverage = fullCoverage && (a[i] == 0) && (b[i] == std::min(colors_[i], 1.0 * n_));
  }
  if (sigmaA > n_) return 0.0;
  if (sigmaB < n_) return 0.0;
  if (fullCoverage) return 1.0;
  if (sigmaA == n_) return computePDF(aP);
  if (sigmaB == n_) return computePDF(bP);
  // Here we know that 0 <= a[j] < b[j] <= min(c[j], n)
  const UnsignedInteger nA = n_ - sigmaA;
  Scalar r = 1.0;
  Scalar logCoefNorm = 0.0;
  // Here r is not necessarily equal to r_ as nA can allow for a reduction
  if (eta_ > 0.0)
  {
    r = std::pow(eta_, 1.0 / (2.0 * nA));
    logCoefNorm = nA * std::log(r);
  }
  // Diametral term
  const Scalar logBinomPDF = DistFunc::logdBinomial(static_cast<UnsignedInteger>(total_), t_, n_);
  return PoissonFormula::computeProbability([&](const Complex & z) { return computeGlobalPhi(z, a, b); },
                                            nA, r, logCoefNorm, logBinomPDF);
}

/* Get the survival function of the distribution */
Scalar MultivariateHypergeometric::computeSurvivalFunction(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getSize() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getSize();
  // The probability mass function has its support on the integer lattice, so
  // P(X_1 > x_1, ..., X_d > x_d) = P(X_1 >= floor(x_1) + 1, ..., X_d >= floor(x_d) + 1),
  // which is the probability content of a box whose upper bounds are unbounded
  Point lower(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) lower[i] = std::floor(point[i]) + 1.0;
  return computeProbability(Interval(lower, Point(dimension, SpecFunc::Infinity)));
}

/* Get the i-th marginal distribution */
Distribution MultivariateHypergeometric::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  Hypergeometric::Implementation marginal(new Hypergeometric(static_cast<UnsignedInteger>(total_), static_cast<UnsignedInteger>(colors_[i]), n_));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution MultivariateHypergeometric::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // Special case for a full marginal in the natural order
  if (indices.getSize() == dimension)
  {
    Bool isIdentity = true;
    for (UnsignedInteger i = 0; isIdentity && (i < dimension); ++i)
      isIdentity = (indices[i] == i);
    if (isIdentity) return clone();
  }
  const UnsignedInteger sizeJ = indices.getSize();
  // The indices of the complementary colors
  const Indices restIndices(indices.complement(dimension));
  UnsignedInteger restTotal = 0;
  for (UnsignedInteger i = 0; i < restIndices.getSize(); ++i)
    restTotal += static_cast<UnsignedInteger>(colors_[restIndices[i]]);
  // The joint probability of a margin point (x_j), summing over the observed draws,
  // P(X_J = x_J) = prod_j C(c_j, x_j) * C(C - sum_J c, n - sum x) / C(C, n)
  // where the last factor is nonzero only if 0 <= n - sum x <= C - sum_J c
  const Scalar logTotal = SpecFunc::LogGamma(total_ + 1.0) - SpecFunc::LogGamma(n_ + 1.0) - SpecFunc::LogGamma(total_ - n_ + 1.0);
  // Enumerate the integer points of the supported box
  Sample atoms(0, sizeJ);
  Point weights(0);
  UnsignedInteger norm = 0;
  Scalar logProd = 0.0;
  Point atom(sizeJ);
  std::function< void (UnsignedInteger) > visit;
  visit = [&](UnsignedInteger depth)
  {
    if (depth == sizeJ)
    {
      if (norm <= n_)
      {
        const UnsignedInteger draws = n_ - norm;
        // The margin point with sum norm must leave between 0 and restTotal draws
        if (draws <= restTotal)
        {
          atoms.add(atom);
          const Scalar logRest = SpecFunc::LogGamma(1.0 * restTotal + 1.0) - SpecFunc::LogGamma(1.0 * draws + 1.0) - SpecFunc::LogGamma(1.0 * (restTotal - draws) + 1.0);
          weights.add(std::exp(logProd + logRest - logTotal));
        }
      }
      return;
    }
    const UnsignedInteger colorJ = static_cast<UnsignedInteger>(colors_[indices[depth]]);
    // Prune the enumeration by the remaining draw budget, as the norm can only grow along the recursion
    const UnsignedInteger xMax = std::min(colorJ, n_ - norm);
    for (UnsignedInteger x = 0; x <= xMax; ++x)
    {
      const Scalar logFactor = SpecFunc::LogGamma(1.0 * colorJ + 1.0) - SpecFunc::LogGamma(1.0 * x + 1.0) - SpecFunc::LogGamma(1.0 * (colorJ - x) + 1.0);
      atom[depth] = 1.0 * x;
      norm += x;
      logProd += logFactor;
      visit(depth + 1);
      logProd -= logFactor;
      norm -= x;
    }
  };
  visit(0);
  FiniteDiscreteDistribution::Implementation marginal(new FiniteDiscreteDistribution(atoms, weights));
  marginal->setDescription(getDescription().select(indices));
  return marginal;
} // getMarginal(Indices)

/* Get the support of a discrete distribution that intersect a given interval */
Sample MultivariateHypergeometric::getSupport(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  Sample reducedSupport(0, dimension);
  // Quick return if the lower bound of the interval is already outside of the support
  const Point lowerBound(interval.getLowerBound());
  const Point upperBound(interval.getUpperBound());
  Scalar sumLower = 0.0;
  Scalar sumUpper = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // One of the components of the upper bound is negative, so the intersection with the positive quadrant is empty
    if (upperBound[i] <= -supportEpsilon_) return reducedSupport;
    sumLower += lowerBound[i];
    sumUpper += upperBound[i];
  }
  // The given interval is fully disjoint with the support
  if ((sumLower >= n_ + supportEpsilon_) || (sumUpper <= -supportEpsilon_)) return reducedSupport;
  // Here we know that all the components of the upper bound are positive or null
  const Sample support(getSupport());
  // Quick return if the interval contains all the support
  // It cannot be possible if the sum of the components of the upper bound is less than n_
  if ((sumUpper >= n_ + supportEpsilon_) && (sumLower <= -supportEpsilon_))
  {
    Bool allInside = true;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      allInside = allInside && (lowerBound[i] <= supportEpsilon_) && (upperBound[i] >= std::min(colors_[i], 1.0 * n_) - supportEpsilon_);
      if (!allInside) break;
    }
    if (allInside) return support;
  }
  // We have to remove some points
  const UnsignedInteger size = support.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point point(support[i]);
    // Don't use interval.contains() as it does not take into account
    // supportEpsilon_
    Bool isInside = true;
    for (UnsignedInteger j = 0; j < dimension_; ++j)
    {
      isInside = isInside && (point[j] >= lowerBound[j] - supportEpsilon_) && (point[j] <= upperBound[j] + supportEpsilon_);
      if (!isInside) break;
    }
    if (isInside) reducedSupport.add(point);
  }
  return reducedSupport;
}

/* Get the support of a discrete distribution */
Sample MultivariateHypergeometric::getSupport() const
{
  const UnsignedInteger dimension = getDimension();
  LinearEnumerateFunction enumerate(dimension);
  const UnsignedInteger start = enumerate.getStrataCumulatedCardinal(n_ - 1);
  const UnsignedInteger stop = enumerate.getStrataCumulatedCardinal(n_);
  Sample support(0, dimension);
  for (UnsignedInteger i = start; i < stop; ++i)
  {
    const Indices multi(enumerate(i));
    Bool inside = true;
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      inside = inside && (multi[j] <= colors_[j]);
      if (!inside) break;
    }
    if (inside)
    {
      Point point(dimension);
      for (UnsignedInteger j = 0; j < dimension; ++j) point[j] = 1.0 * multi[j];
      support.add(point);
    }
  }
  return support;
}

/* Compute the mean of the distribution */
void MultivariateHypergeometric::computeMean() const
{
  mean_ = n_ * (colors_ / total_);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void MultivariateHypergeometric::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  // To insure a zero initialization
  covariance_ = CovarianceMatrix(dimension);
  if (total_ <= 1.0)
  {
    isAlreadyComputedCovariance_ = true;
    return;
  }
  const Scalar finiteCorrection = (total_ - n_) / (total_ - 1.0);
  const Scalar draws = n_;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar pI = colors_[i] / total_;
    covariance_(i, i) = draws * pI * (1.0 - pI) * finiteCorrection;
    for (UnsignedInteger j = 0; j < i; ++j) covariance_(i, j) = -draws * pI * (colors_[j] / total_) * finiteCorrection;
  }
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
MultivariateHypergeometric::PointWithDescriptionCollection MultivariateHypergeometric::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters((dimension == 1 ? 1 : dimension + 1));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription point(2);
    point[0] = n_;
    point[1] = colors_[i];
    Description description(2);
    description[0] = "n";
    OSS oss;
    oss << "c_" << i;
    description[1] = oss;
    point.setDescription(description);
    point.setName(getDescription()[i]);
    parameters[i] = point;
  }
  if (dimension > 1)
  {
    PointWithDescription point(dimension + 1);
    Description description(dimension + 1);
    point[0] = n_;
    description[0] = "n";
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      point[i + 1] = colors_[i];
      OSS oss;
      oss << "c_" << i;
      description[i + 1] = oss;
    }
    point.setDescription(description);
    point.setName("dependence");
    parameters[dimension] = point;
  }
  return parameters;
}

/* Parameters value accessor */
Point MultivariateHypergeometric::getParameter() const
{
  Point parameter(1, n_);
  parameter.add(colors_);
  return parameter;
}

/* Parameters description accessor */
Description MultivariateHypergeometric::getParameterDescription() const
{
  Description description(1, "n");
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    description.add(OSS() << "c_" << i);
  return description;
}

/* Parameters value accessor */
void MultivariateHypergeometric::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getDimension();
  if (parameter.getDimension() != (dimension + 1))
    throw InvalidArgumentException(HERE) << "MultivariateHypergeometric expected " << (dimension + 1) << " parameters, got " << parameter.getDimension();
  if (parameter[0] != std::round(parameter[0])) throw InvalidArgumentException(HERE) << "the MultivariateHypergeometric first parameter n must be an integer, got " << parameter[0];
  if (parameter[0] < 1.0) throw InvalidArgumentException(HERE) << "the MultivariateHypergeometric first parameter n must be positive, got " << parameter[0];
  // We reconstruct the distribution through the canonical constructor in order to handle
  // the shrink case, eg (5, [10]) -> (4, [5]), where a componentwise reordering fails
  Point colors(dimension);
  std::copy(parameter.begin() + 1, parameter.end(), colors.begin());
  const Scalar eta = eta_;
  const Scalar smallA = smallA_;
  const Scalar w = getWeight();
  const Description d(getDescription());
  *this = MultivariateHypergeometric(static_cast<UnsignedInteger>(parameter[0]), colors);
  setSmallA(smallA);
  setEta(eta);
  setWeight(w);
  setDescription(d);
}

/* Check if the distribution is elliptical */
Bool MultivariateHypergeometric::isElliptical() const
{
  return false;
}

/* Colors accessor */
void MultivariateHypergeometric::setColors(const Point & colors)
{
  // We check that the elements are all integers and nonnegative
  const UnsignedInteger dimension = colors.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "colors must have a positive dimension.";
  Scalar total = 0.0;
  for(UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar colorI = colors[i];
    if (!(colorI >= 0.0)) throw InvalidArgumentException(HERE) << "colors elements MUST be nonnegative";
    if (std::abs(colorI - round(colorI)) > supportEpsilon_) throw InvalidArgumentException(HERE) << "colors elements MUST be integers";
    total += colorI;
  }
  if (total < n_ * (1.0 - supportEpsilon_)) throw InvalidArgumentException(HERE) << "the sum of colors = " << total << " must be greater or equal to the number of draws n = " << n_;
  colors_ = colors;
  total_ = total;
  updateSpectral();
  setDimension(dimension);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
}

/* Colors accessor */
Point MultivariateHypergeometric::getColors() const
{
  return colors_;
}

/* N accessor */
void MultivariateHypergeometric::setN(const UnsignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0.";
  if (total_ > 0.0)
  {
    if (n > total_) throw InvalidArgumentException(HERE) << "Error: the number of draws n = " << n << " must be smaller or equal to the sum of colors = " << total_ << ".";
  }
  if (n != n_)
  {
    n_ = n;
    updateSpectral();
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
    computeRange();
  }
}

/* N accessor */
UnsignedInteger MultivariateHypergeometric::getN() const
{
  return n_;
}

/* SmallA accessor */
void MultivariateHypergeometric::setSmallA(const Scalar smallA)
{
  smallA_ = smallA;
}

/* SmallA accessor */
Scalar MultivariateHypergeometric::getSmallA() const
{
  return smallA_;
}

/* Eta accessor */
void MultivariateHypergeometric::setEta(const Scalar eta)
{
  eta_ = eta;
  updateSpectral();
}

/* Eta accessor */
Scalar MultivariateHypergeometric::getEta() const
{
  return eta_;
}

/* Update the parameters of the spectral decomposition of the Poisson formula */
void MultivariateHypergeometric::updateSpectral()
{
  if ((n_ > 0) && (total_ > 0.0))
  {
    t_ = n_ / total_;
    if (eta_ > 0.0)
    {
      r_ = std::pow(eta_, 1.0 / (2.0 * n_));
      normalizationCDF_ = std::exp(-(1.0 * n_) * std::log(r_) - std::log(2.0 * n_) - DistFunc::logdBinomial(static_cast<UnsignedInteger>(total_), t_, n_));
    }
  }
}

/* Compute the scalar quantile of the 1D multivariate hypergeometric distribution */
Scalar MultivariateHypergeometric::computeScalarQuantile(const Scalar prob,
                                                         const Bool tail) const
{
  return Hypergeometric(static_cast<UnsignedInteger>(total_), static_cast<UnsignedInteger>(colors_[0]), n_).computeQuantile(prob, tail)[0];
}

/* Method save() stores the object through the StorageManager */
void MultivariateHypergeometric::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "colors_", colors_ );
  adv.saveAttribute( "total_", total_ );
  adv.saveAttribute( "t_", t_ );
  adv.saveAttribute( "normalizationCDF_", normalizationCDF_ );
  adv.saveAttribute( "r_", r_ );
  adv.saveAttribute( "smallA_", smallA_ );
  adv.saveAttribute( "eta_", eta_ );
}

/* Method load() reloads the object from the StorageManager */
void MultivariateHypergeometric::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "colors_", colors_ );
  adv.loadAttribute( "total_", total_ );
  adv.loadAttribute( "t_", t_ );
  adv.loadAttribute( "normalizationCDF_", normalizationCDF_ );
  adv.loadAttribute( "r_", r_ );
  adv.loadAttribute( "smallA_", smallA_ );
  adv.loadAttribute( "eta_", eta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS