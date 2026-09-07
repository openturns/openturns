//                                               -*- C++ -*-
/**
 *  @brief The MultivariatePolya distribution
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
#include "openturns/MultivariatePolya.hxx"
#include "openturns/Multinomial.hxx"
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

CLASSNAMEINIT(MultivariatePolya)

static const Factory<MultivariatePolya> Factory_MultivariatePolya;

/* Default constructor */
MultivariatePolya::MultivariatePolya()
  : DistributionImplementation()
  , n_(0)
  , alpha_(0)
  , c_(0.0)
  , q_(0.0)
  , u_(0)
  , U_(0.0)
  , t_(0.0)
  , normalizationCDF_(0.0)
  , r_(0.0)
  , smallA_(ResourceMap::GetAsScalar("MultivariatePolya-smallA"))
  , eta_(ResourceMap::GetAsScalar("MultivariatePolya-eta"))
{
  setName("MultivariatePolya");
  // This method compute the range also
  setN(1);
  setAlpha( Point(1, 1.0) );
  setC(1.0);
}

/* Parameters constructor */
MultivariatePolya::MultivariatePolya(const UnsignedInteger n,
                                     const Point & alpha,
                                     const Scalar c)
  : DistributionImplementation()
  , n_(0)
  , alpha_(0)
  , c_(0.0)
  , q_(0.0)
  , u_(0)
  , U_(0.0)
  , t_(0.0)
  , normalizationCDF_(0.0)
  , r_(0.0)
  , smallA_(ResourceMap::GetAsScalar("MultivariatePolya-smallA"))
  , eta_(ResourceMap::GetAsScalar("MultivariatePolya-eta"))
{
  setName("MultivariatePolya");
  // This method compute the range also
  setN( n );
  setAlpha( alpha );
  setC( c );
  computeRange();
}

/* Tell if the distribution is continuous */
Bool MultivariatePolya::isContinuous() const
{
  return false;
}

/* Tell if the distribution is discrete */
Bool MultivariatePolya::isDiscrete() const
{
  return true;
}

/* Tell if the distribution is integer valued */
Bool MultivariatePolya::isIntegral() const
{
  return true;
}

/* Comparison operator */
Bool MultivariatePolya::operator ==(const MultivariatePolya & other) const
{
  if (this == &other) return true;
  return hasEqualBase(other) && (n_ == other.n_) && (alpha_ == other.alpha_) && (c_ == other.c_);
}

/* Comparison operator */
Bool MultivariatePolya::equals(const DistributionImplementation & other) const
{
  const MultivariatePolya* p_other = dynamic_cast<const MultivariatePolya*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String MultivariatePolya::__repr__() const
{
  OSS oss;
  oss << "class=" << MultivariatePolya::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " alpha=" << alpha_
      << " c=" << c_
      << " n=" << n_;
  return oss;
}

String MultivariatePolya::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(n = " << n_ << ", alpha = " << alpha_.__str__() << ", c = " << c_ << ")";
  return oss;
}

/* Virtual constructor */
MultivariatePolya * MultivariatePolya::clone() const
{
  return new MultivariatePolya(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void MultivariatePolya::computeRange()
{
  const UnsignedInteger dimension = getDimension();
  Point lowerBound(dimension);
  Point upperBound(dimension, 1.0 * n_);
  const Interval::BoolCollection finiteLowerBound(dimension, true);
  const Interval::BoolCollection finiteUpperBound(dimension, true);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Update the spectral parameters given the current parameters values */
void MultivariatePolya::updateSpectral()
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 0) return;
  if (c_ == 0.0) return;
  u_ = alpha_ / c_;
  U_ = q_ / c_;
  t_ = U_ / (U_ + n_);
  // Best overall performance for Poisson's formula, see reference
  if ((eta_ > 0.0) && (n_ > 0))
  {
    r_ = std::pow(eta_, 1.0 / (2.0 * n_));
    normalizationCDF_ = std::exp(-(1.0 * n_) * std::log(r_) - std::log(2.0 * n_) - (SpecFunc::LogGamma(U_ + n_) - SpecFunc::LogGamma(U_) - SpecFunc::LogGamma(1.0 * n_ + 1.0) + U_ * std::log(t_) + n_ * std::log(1.0 - t_)));
  }
}

/* Draw one realization of a multinomial distribution with probabilities p */
Point MultivariatePolya::drawMultinomial(const UnsignedInteger n,
                                         const Point & p)
{
  const UnsignedInteger dimension = p.getDimension();
  Point realization(dimension);
  // Sequential conditional sampling: X_1 ~ Binomial(n, p_1), then
  // X_j | X_1...X_(j-1) ~ Binomial(remaining draws, p_j / sum of remaining probabilities)
  UnsignedInteger remainingN = n;
  Scalar pRemaining = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (i == dimension - 1)
    {
      realization[i] = remainingN;
      continue;
    }
    const Scalar pI = (pRemaining > 0.0 ? std::min(1.0, p[i] / pRemaining) : 0.0);
    const UnsignedInteger xI = DistFunc::rBinomial(remainingN, pI);
    realization[i] = 1.0 * xI;
    remainingN -= xI;
    pRemaining -= p[i];
  }
  return realization;
}

/* Get one realization of the distribution */
Point MultivariatePolya::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  // In the degenerate multinomial case the probabilities are fixed
  const Point p((c_ == 0.0) ? (alpha_ / q_) : Point(dimension));
  Point pTilde(dimension);
  if (c_ > 0.0)
  {
    // Dirichlet-Multinomial, draw the probabilities from a Dirichlet distribution
    Scalar gammaSum = 0.0;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const Scalar gammaI = DistFunc::rGamma(u_[i]);
      gammaSum += gammaI;
      pTilde[i] = gammaI;
    }
    for (UnsignedInteger i = 0; i < dimension; ++i) pTilde[i] /= gammaSum;
  }
  else
  {
    pTilde = p;
  }
  return drawMultinomial(n_, pTilde);
}

/* Get a sample of the distribution */
Sample MultivariatePolya::getSample(const UnsignedInteger size) const
{
  const UnsignedInteger dimension = getDimension();
  SampleImplementation sample(size, dimension);
  const Point p((c_ == 0.0) ? (alpha_ / q_) : Point(dimension));
  Point pTilde(dimension);
  for (UnsignedInteger k = 0; k < size; ++k)
  {
    if (c_ > 0.0)
    {
      // Dirichlet-Multinomial, draw the probabilities from a Dirichlet distribution
      Scalar gammaSum = 0.0;
      for (UnsignedInteger i = 0; i < dimension; ++i)
      {
        const Scalar gammaI = DistFunc::rGamma(u_[i]);
        gammaSum += gammaI;
        pTilde[i] = gammaI;
      }
      for (UnsignedInteger i = 0; i < dimension; ++i) pTilde[i] /= gammaSum;
    }
    else
    {
      pTilde = p;
    }
    const Point realization(drawMultinomial(n_, pTilde));
    for (UnsignedInteger i = 0; i < dimension; ++i) sample(k, i) = realization[i];
  }
  sample.setName(getName());
  sample.setDescription(getDescription());
  return sample;
}

/* Get the PDF of the distribution */
Scalar MultivariatePolya::computePDF(const Point & point) const
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
    sumX += x[i];
  }
  // The sum of the components must be exactly n_
  if (sumX != n_) return 0.0;
  Scalar logPDF = 0.0;
  if (c_ > 0.0)
  {
    // P(X = x) = n! / Po(q, n; c) * prod_j Po(alpha_j, x_j; c) / x_j!
    // with Po(x, m; c) = c^m Gamma(x/c + m) / Gamma(x/c), so that
    // log P(X = x) = lgamma(n+1) - sum_j lgamma(x_j+1)
    //     + sum_j [lgamma(u_j + x_j) - lgamma(u_j)] - [lgamma(U + n) - lgamma(U)]
    logPDF = SpecFunc::LogGamma(1.0 * n_ + 1.0);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const Scalar xI = 1.0 * x[i];
      logPDF -= SpecFunc::LogGamma(xI + 1.0);
      logPDF += SpecFunc::LogGamma(u_[i] + xI) - SpecFunc::LogGamma(u_[i]);
    }
    logPDF -= SpecFunc::LogGamma(U_ + n_) - SpecFunc::LogGamma(U_);
  }
  else
  {
    // c = 0, the distribution degenerates into the multinomial with probabilities alpha / q
    logPDF = SpecFunc::LogGamma(1.0 * n_ + 1.0);
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      const Scalar xI = 1.0 * x[i];
      logPDF -= SpecFunc::LogGamma(xI + 1.0);
      logPDF += xI * std::log(alpha_[i] / q_);
    }
  }
  return std::exp(logPDF);
}

/* Compute the generating function of a sum of truncated negative binomial distributions as needed in the computeCDF() method */
Complex MultivariatePolya::computeGlobalPhi(const Complex & z,
                                            const Indices & x) const
{
  Complex value(1.0);
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    value *= computeLocalPhi(z, u_[i], t_, x[i]);
    if (std::abs(value) == 0.0)
    {
      LOGWARN("Underflow in MultivariatePolya::computeGlobalPhi");
      return 0.0;
    }
  }
  return value;
}

/* Compute the generating function of a sum of truncated negative binomial distributions as needed in the computeProbability() method */
Complex MultivariatePolya::computeGlobalPhi(const Complex & z,
                                            const Indices & a,
                                            const Indices & b) const
{
  Complex value(1.0);
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    value *= computeLocalPhi(z, u_[i], t_, a[i], b[i]);
    if (std::abs(value) == 0.0)
    {
      LOGWARN("Underflow in MultivariatePolya::computeGlobalPhi");
      return 0.0;
    }
  }
  return value;
}

/* Compute the generating function of a truncated negative binomial distribution as needed in the computeCDF() method */
Complex MultivariatePolya::computeLocalPhi(const Complex & z,
                                           const Scalar u,
                                           const Scalar t,
                                           const UnsignedInteger a) const
{
  return PoissonFormula::localPhi0(z, PoissonFormula::NegativeBinomialKernel(u, t, z), a, smallA_);
}

/* Compute the generating function of a truncated negative binomial distribution as needed in the computeProbability() method */
Complex MultivariatePolya::computeLocalPhi(const Complex & z,
                                           const Scalar u,
                                           const Scalar t,
                                           const UnsignedInteger a,
                                           const UnsignedInteger b) const
{
  return PoissonFormula::localPhi(z, PoissonFormula::NegativeBinomialKernel(u, t, z), a, b, smallA_);
}

/* Get the CDF of the distribution
   Algorithm described in:
   R. Lebrun, "Efficient time/space algorithm to compute rectangular probabilities of multinomial, multivariate hypergeometric and multivariate Polya distributions", Statistics and Computing, submitted (2011).
*/
Scalar MultivariatePolya::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  // In the degenerate multinomial case, rely on the multinomial implementation
  if (c_ == 0.0) return computeDegenerateDistribution().computeCDF(point);
  // Early exit for 1D case, the single component is equal to n_
  if (dimension == 1) return (point[0] >= n_ - supportEpsilon_ ? 1.0 : 0.0);
  // First, check the trivial cases
  Bool allZero = true;
  UnsignedInteger sumX = 0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If the given point does not cover any point of the support, return 0.0
    if (point[i] < 0.0) return 0.0;
    allZero = allZero && (point[i] <= supportEpsilon_);
  }
  // Clamp each component to [0, n_] to avoid overflow on UnsignedInteger cast
  Point clamped(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    clamped[i] = std::min(point[i], 1.0 * n_);
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
    fullCoverage = fullCoverage && (kPoint[i] >= n_);
    if (!fullCoverage) break;
  }
  if (fullCoverage) return 1.0;
  // Evaluation of P(W=n) using Poisson's formula
  return PoissonFormula::computeCDF([&](const Complex & z) { return computeGlobalPhi(z, kPoint); },
                                    n_, r_, normalizationCDF_, getClassName());
}

/* Compute the probability content of an interval */
Scalar MultivariatePolya::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();
  // In the degenerate multinomial case, rely on the multinomial implementation
  if (c_ == 0.0) return computeDegenerateDistribution().computeProbability(interval);

  // Early exit for 1D case, the single component is equal to n_
  if (dimension == 1)
  {
    const Scalar a = interval.getLowerBound()[0];
    const Scalar b = interval.getUpperBound()[0];
    return (a <= n_ + supportEpsilon_ && b >= n_ - supportEpsilon_) ? 1.0 : 0.0;
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
    const Scalar upperI = std::min(upper[i], 1.0 * n_);
    aP[i] = std::max(0.0, std::ceil(lower[i]));
    bP[i] = floor(upperI);
    if (bP[i] < -supportEpsilon_) return 0.0;
    a[i] = static_cast<UnsignedInteger>(aP[i]);
    b[i] = static_cast<UnsignedInteger>(bP[i]);
    if (a[i] > b[i]) return 0.0;
    sigmaA += a[i];
    sigmaB += b[i];
    fullCoverage = fullCoverage && (a[i] == 0) && (b[i] == n_);
  }
  if (sigmaA > n_) return 0.0;
  if (sigmaB < n_) return 0.0;
  if (fullCoverage) return 1.0;
  if (sigmaA == n_) return computePDF(aP);
  if (sigmaB == n_) return computePDF(bP);
  // Here we know that 0 <= a[j] < b[j] <= n
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
  const Scalar logBinomPDF = SpecFunc::LogGamma(U_ + n_) - SpecFunc::LogGamma(U_) - SpecFunc::LogGamma(1.0 * n_ + 1.0) + U_ * std::log(t_) + n_ * std::log(1.0 - t_);
  return PoissonFormula::computeProbability([&](const Complex & z) { return computeGlobalPhi(z, a, b); },
                                            nA, r, logCoefNorm, logBinomPDF);
}

/* Get the survival function of the distribution */
Scalar MultivariatePolya::computeSurvivalFunction(const Point & point) const
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
Distribution MultivariatePolya::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // In the degenerate multinomial case, rely on the multinomial implementation
  if (c_ == 0.0) return computeDegenerateDistribution().getMarginal(i);
  return getMarginal(Indices(1, i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution MultivariatePolya::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // Special case for a full marginal in any order: the joint distribution of
  // all the components is the multivariate Polya itself, with reordered alpha
  if (indices.getSize() == dimension)
  {
    Bool isIdentity = true;
    for (UnsignedInteger i = 0; isIdentity && (i < dimension); ++i)
      isIdentity = (indices[i] == i);
    if (isIdentity) return clone();
    MultivariatePolya marginal(n_, alpha_.select(indices), c_);
    marginal.setDescription(getDescription().select(indices));
    return marginal;
  }
  // In the degenerate multinomial case, rely on the multinomial implementation
  if (c_ == 0.0) return computeDegenerateDistribution().getMarginal(indices);
  const UnsignedInteger sizeJ = indices.getSize();
  // The indices of the complementary categories, aggregated into a rest category
  const Indices restIndices(indices.complement(dimension));
  Scalar alphaRest = 0.0;
  for (UnsignedInteger i = 0; i < restIndices.getSize(); ++i)
    alphaRest += alpha_[restIndices[i]];
  const Scalar uRest = alphaRest / c_;
  // The joint probability of a margin point (x_j), aggregating the complementary categories,
  // P(X_J = x) = n! / [Po(q, n; c) prod_j x_j!] prod_j Po(alpha_j, x_j; c) Po(alpha_rest, n - sum x; c)
  // where Po(x, m; c) = c^m Gamma(x/c + m) / Gamma(x/c)
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
      // Skip the atoms whose total number of draws exceeds n
      if (norm > n_) return;
      const UnsignedInteger draws = n_ - norm;
      const Scalar drawsP = 1.0 * draws;
      Scalar logWeight = SpecFunc::LogGamma(1.0 * n_ + 1.0) - SpecFunc::LogGamma(drawsP + 1.0);
      logWeight += SpecFunc::LogGamma(uRest + drawsP) - SpecFunc::LogGamma(uRest);
      logWeight -= SpecFunc::LogGamma(U_ + n_) - SpecFunc::LogGamma(U_);
      atoms.add(atom);
      weights.add(std::exp(logWeight + logProd));
      return;
    }
    const Scalar uJ = alpha_[indices[depth]] / c_;
    for (UnsignedInteger x = 0; x <= n_; ++x)
    {
      if (norm + x > n_) break;
      const Scalar xP = 1.0 * x;
      const Scalar logFactor = SpecFunc::LogGamma(uJ + xP) - SpecFunc::LogGamma(uJ) - SpecFunc::LogGamma(xP + 1.0);
      atom[depth] = xP;
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
Sample MultivariatePolya::getSupport(const Interval & interval) const
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
      allInside = allInside && (lowerBound[i] <= supportEpsilon_) && (upperBound[i] >= n_ - supportEpsilon_);
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
Sample MultivariatePolya::getSupport() const
{
  const UnsignedInteger dimension = getDimension();
  LinearEnumerateFunction enumerate(dimension);
  const UnsignedInteger start = enumerate.getStrataCumulatedCardinal(n_ - 1);
  const UnsignedInteger stop = enumerate.getStrataCumulatedCardinal(n_);
  Sample support(0, dimension);
  for (UnsignedInteger i = start; i < stop; ++i)
  {
    const Indices multi(enumerate(i));
    Point point(dimension);
    for (UnsignedInteger j = 0; j < dimension; ++j) point[j] = 1.0 * multi[j];
    support.add(point);
  }
  return support;
}

/* Compute the mean of the distribution */
void MultivariatePolya::computeMean() const
{
  mean_ = n_ * (alpha_ / q_);
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void MultivariatePolya::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  // To insure a zero initialization
  covariance_ = CovarianceMatrix(dimension);
  const Scalar overdispersion = (c_ == 0.0 ? 1.0 : (n_ + q_ / c_) / (1.0 + q_ / c_));
  const Scalar draws = n_;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar pI = alpha_[i] / q_;
    covariance_(i, i) = draws * pI * (1.0 - pI) * overdispersion;
    for (UnsignedInteger j = 0; j < i; ++j) covariance_(i, j) = -draws * pI * (alpha_[j] / q_) * overdispersion;
  }
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
MultivariatePolya::PointWithDescriptionCollection MultivariatePolya::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters((dimension == 1 ? 1 : dimension + 1));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription point(3);
    point[0] = n_;
    point[1] = c_;
    point[2] = alpha_[i];
    Description description(3);
    description[0] = "n";
    description[1] = "c";
    OSS oss;
    oss << "alpha_" << i;
    description[2] = oss;
    point.setDescription(description);
    point.setName(getDescription()[i]);
    parameters[i] = point;
  }
  if (dimension > 1)
  {
    PointWithDescription point(dimension + 2);
    Description description(dimension + 2);
    point[0] = n_;
    description[0] = "n";
    point[1] = c_;
    description[1] = "c";
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      point[i + 2] = alpha_[i];
      OSS oss;
      oss << "alpha_" << i;
      description[i + 2] = oss;
    }
    point.setDescription(description);
    point.setName("dependence");
    parameters[dimension] = point;
  }
  return parameters;
}

/* Parameters value accessor */
Point MultivariatePolya::getParameter() const
{
  Point parameter(1, n_);
  parameter.add(Point(1, c_));
  parameter.add(alpha_);
  return parameter;
}

/* Parameters description accessor */
Description MultivariatePolya::getParameterDescription() const
{
  Description description(2);
  description[0] = "n";
  description[1] = "c";
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    description.add(OSS() << "alpha_" << i);
  return description;
}

/* Parameters value accessor */
void MultivariatePolya::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getDimension();
  if (parameter.getDimension() != (dimension + 2))
    throw InvalidArgumentException(HERE) << "MultivariatePolya expected " << (dimension + 2) << " parameters, got " << parameter.getDimension();
  if (parameter[0] != std::round(parameter[0])) throw InvalidArgumentException(HERE) << "the MultivariatePolya first parameter n must be an integer, got " << parameter[0];
  if (parameter[0] < 1.0) throw InvalidArgumentException(HERE) << "the MultivariatePolya first parameter n must be positive, got " << parameter[0];
  setN(parameter[0]);
  setC(parameter[1]);
  Point alpha(dimension);
  std::copy(parameter.begin() + 2, parameter.end(), alpha.begin());
  setAlpha(alpha);
}

/* Check if the distribution is elliptical */
Bool MultivariatePolya::isElliptical() const
{
  return false;
}

/* Build the multinomial distribution corresponding to the degenerate case c = 0 */
Multinomial MultivariatePolya::computeDegenerateDistribution() const
{
  return Multinomial(n_, alpha_ / q_);
}

/* Alpha accessor */
void MultivariatePolya::setAlpha(const Point & alpha)
{
  // We check that the elements are all strictly positive
  const UnsignedInteger dimension = alpha.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "alpha must have a positive dimension.";
  Scalar q = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar alphaI = alpha[i];
    if (!(alphaI > 0.0)) throw InvalidArgumentException(HERE) << "alpha elements MUST be strictly positive";
    q += alphaI;
  }
  alpha_ = alpha;
  q_ = q;
  setDimension(dimension);
  updateSpectral();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
  computeRange();
}

/* Alpha accessor */
Point MultivariatePolya::getAlpha() const
{
  return alpha_;
}

/* C accessor */
void MultivariatePolya::setC(const Scalar c)
{
  if (!(c >= 0.0)) throw InvalidArgumentException(HERE) << "the reinforcement parameter c must be nonnegative";
  c_ = c;
  updateSpectral();
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
}

/* C accessor */
Scalar MultivariatePolya::getC() const
{
  return c_;
}

/* N accessor */
void MultivariatePolya::setN(const UnsignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0.";
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
UnsignedInteger MultivariatePolya::getN() const
{
  return n_;
}

/* SmallA accessor */
void MultivariatePolya::setSmallA(const Scalar smallA)
{
  smallA_ = smallA;
}

/* SmallA accessor */
Scalar MultivariatePolya::getSmallA() const
{
  return smallA_;
}

/* Eta accessor */
void MultivariatePolya::setEta(const Scalar eta)
{
  if (!(eta >= 0.0)) throw InvalidArgumentException(HERE) << "Error: eta must be non-negative, a value of 0 turns the error control off.";
  eta_ = eta;
  updateSpectral();
}

/* Eta accessor */
Scalar MultivariatePolya::getEta() const
{
  return eta_;
}

/* Compute the scalar quantile of the 1D multivariate Polya distribution */
Scalar MultivariatePolya::computeScalarQuantile(const Scalar prob,
                                                const Bool) const
{
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "computeScalarQuantile expected prob to belong to [0,1], but is " << prob;
  // The single component of a 1D multivariate Polya distribution is equal to n_
  return 1.0 * n_;
}

/* Method save() stores the object through the StorageManager */
void MultivariatePolya::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "c_", c_ );
  adv.saveAttribute( "q_", q_ );
  adv.saveAttribute( "u_", u_ );
  adv.saveAttribute( "U_", U_ );
  adv.saveAttribute( "t_", t_ );
  adv.saveAttribute( "normalizationCDF_", normalizationCDF_ );
  adv.saveAttribute( "r_", r_ );
  adv.saveAttribute( "smallA_", smallA_ );
  adv.saveAttribute( "eta_", eta_ );
}

/* Method load() reloads the object from the StorageManager */
void MultivariatePolya::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "c_", c_ );
  adv.loadAttribute( "q_", q_ );
  adv.loadAttribute( "u_", u_ );
  adv.loadAttribute( "U_", U_ );
  adv.loadAttribute( "t_", t_ );
  adv.loadAttribute( "normalizationCDF_", normalizationCDF_ );
  adv.loadAttribute( "r_", r_ );
  adv.loadAttribute( "smallA_", smallA_ );
  adv.loadAttribute( "eta_", eta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS