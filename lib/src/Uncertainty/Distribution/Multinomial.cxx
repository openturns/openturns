//                                               -*- C++ -*-
/**
 *  @brief The Multinomial distribution
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/Collection.hxx"
#include "openturns/Multinomial.hxx"
#include "openturns/Binomial.hxx"
#include "openturns/Poisson.hxx"
#include "openturns/TruncatedDistribution.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/LinearEnumerateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

typedef Collection<UnsignedInteger>     UnsignedIntegerCollection;

CLASSNAMEINIT(Multinomial)

static const Factory<Multinomial> Factory_Multinomial;

/* Default constructor */
Multinomial::Multinomial()
  : DiscreteDistribution()
  , n_(0)
  , p_(0)
  , sumP_(0.0)
  , smallA_(ResourceMap::GetAsScalar("Multinomial-smallA"))
  , eta_(ResourceMap::GetAsScalar("Multinomial-eta"))
{
  setName("Multinomial");
  // This method compute the range also
  setN(1);
  setP( Point(1, 0.5) );
}

/* Parameters constructor */
Multinomial::Multinomial(const UnsignedInteger n,
                         const Point & p)
  : DiscreteDistribution()
  , n_(0)
  , p_(0)
  , sumP_(0.0)
  , smallA_(ResourceMap::GetAsScalar("Multinomial-smallA"))
  , eta_(ResourceMap::GetAsScalar("Multinomial-eta"))
{
  setName("Multinomial");
  // This method compute the range also
  setN( n );
  setP( p );
  computeRange();
}

/* Comparison operator */
Bool Multinomial::operator ==(const Multinomial & other) const
{
  if (this == &other) return true;
  return (n_ == other.n_) && (p_ == other.p_);
}

Bool Multinomial::equals(const DistributionImplementation & other) const
{
  const Multinomial* p_other = dynamic_cast<const Multinomial*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Multinomial::__repr__() const
{
  OSS oss;
  oss << "class=" << Multinomial::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " p=" << p_
      << " n=" << n_;
  return oss;
}

String Multinomial::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(n = " << n_ << ", p = " << p_.__str__() << ")";
  return oss;
}

/* Virtual constructor */
Multinomial * Multinomial::clone() const
{
  return new Multinomial(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Multinomial::computeRange()
{
  const UnsignedInteger dimension = getDimension();
  const Point lowerBound(dimension, 0.0);
  const Point upperBound(dimension, n_);
  const Interval::BoolCollection finiteLowerBound(dimension, true);
  const Interval::BoolCollection finiteUpperBound(dimension, true);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get one realization of the distribution */
Point Multinomial::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  Point realization(dimension);
  /* We use an elementary algorithm based on the definition of the Multinomial distribution:
   * the i-th component is generated using a Binomial distribution */
  UnsignedInteger n = n_;
  Scalar sum = 1.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    /* The current component follow a binomial distribution with parameters p_[i] and N */
    const UnsignedInteger xI = DistFunc::rBinomial(n, p_[i] / sum);
    realization[i] = xI;
    n -= xI;
    sum -= p_[i];
  }
  return realization;
}

/* Get the PDF of the distribution */
Scalar Multinomial::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // First, check the validity of the input
  Scalar sumX = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar k = point[i];
    // Early exit if the given point is not in the support of the distribution
    if ((std::abs(k - round(k)) > supportEpsilon_) || (k < -supportEpsilon_) || (k > n_ + supportEpsilon_)) return 0.0;
    sumX += k;
  }
  if (sumX > n_ + supportEpsilon_) return 0.0;
  if ((sumP_ >= 1.0) && (sumX < n_ - supportEpsilon_)) return 0.0;
  Scalar logPDF = -1.0;
  if (sumP_ < 1.0) logPDF = lgamma(n_ + 1.0) - lgamma(n_ - sumX + 1.0) + (n_ - sumX) * log1p(-sumP_);
  // In the case sumP_ >= 1.0, the PDF is positive only if sumX == n_
  else logPDF = lgamma(n_ + 1.0);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar k = point[i];
    // For p_[i] > 0, it is possible to obtain any value of k between 0 and n
    if (p_[i] > 0.0) logPDF += k * std::log(p_[i]) - lgamma(k + 1.0);
    // Else only k == 0 is allowed, with a zero contribution to the log PDF
    else if (k > 0) return 0.0;
  }
  return std::exp(logPDF);
}

/* Compute the generating function of a sum of truncated Poisson distributions as needed in the computeCDF() method */
Complex Multinomial::computeGlobalPhi(const Complex & z,
                                      const Point & x) const
{
  // Initialize with the non truncated term
  Complex value(std::exp(-(1.0 - sumP_) * n_ * (1.0 - z)));
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    value *= computeLocalPhi(z, n_ * p_[i], x[i]);
    if (std::abs(value) == 0.0)
    {
      LOGWARN("Underflow in Multinomial::computePhi");
      return 0.0;
    }
  }
  return value;
}

/* Compute the generating function of a truncated Poisson distributions as needed in the computeCDF() method */
Complex Multinomial::computeLocalPhi(const Complex & z,
                                     const Scalar lambda,
                                     const Scalar a) const
{
  if (z == 0.0) return 1.0;
  const Complex u(lambda * z);
  const UnsignedInteger iMax = static_cast< UnsignedInteger > (floor(a));
  // Small value of a, evaluate the generating function as a polynomial
  if (a <= smallA_)
  {
    Complex value(std::exp(-lambda));
    Complex term(value);
    for (UnsignedInteger i = 1; i <= iMax; ++i)
    {
      term *= u / Complex(i);
      value += term;
    }
    return value;
  } // smallA_
  // Large a
  Complex value(std::exp(-lambda + u));
  UnsignedInteger i = iMax + 1;
  Complex term(std::exp(-lambda + Complex(i) * std::log(u) - lgamma(i + 1.0)));
  while (std::abs(term) > SpecFunc::Precision * std::abs(value))
  {
    value -= term;
    ++i;
    term *= u / Complex(i);
  }
  return value - term;
}

/* Get the CDF of the distribution
   Algorithm described in:
   R. Lebrun, "Efficient time/space algorithm to compute rectangular probabilities of multinomial, multivariate hypergeometric and multivariate Polya distributions", Statistics and Computing, submitted (2011).
*/
Scalar Multinomial::computeCDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();

  // Early exit for 1D case
  if (dimension == 1)
  {
    const Scalar k = point[0];
    if (k < -supportEpsilon_) return 0.0;
    if (k > n_ + supportEpsilon_) return 1.0;
    return DistFunc::pBeta(n_ - floor(k), floor(k) + 1, 1.0 - p_[0]);
  }
  // First, check the bording cases
  Indices indices(0);
  Bool allZero = true;
  Scalar sumX = 0.0;
  // Trivial cases
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // If the given point does not cover any point of the support, return 0.0
    if (point[i] < -supportEpsilon_) return 0.0;
    if (point[i] < n_ - supportEpsilon_) indices.add(i);
    allZero = allZero && (std::abs(point[i]) < supportEpsilon_);
    sumX += point[i];
  }
  // If we are at the origin, CDF=PDF(0,...,0)
  if (allZero) return std::pow(1.0 - sumP_, static_cast<int>(n_));
  // If the atoms with non zero probability sum to N
  if ((std::abs(sumP_ - 1.0) < supportEpsilon_) && (sumX == n_))
  {
    Scalar value = lgamma(n_ + 1.0);
    for (UnsignedInteger j = 0; j < dimension; ++j)
      value += point[j] * std::log(p_[j]) - lgamma(point[j] + 1.0);
    return std::exp(value);
  }
  // If the point covers the whole support of the distribution, return 1.0
  const UnsignedInteger size = indices.getSize();
  if (size == 0) return 1.0;
  // Now, check if there is a possible reduction of the dimension
  if (size < dimension)
  {
    // reduce the dimension to the active indices
    Point pReduced(size);
    Point xReduced(size);
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      pReduced[i] = p_[indices[i]];
      xReduced[i] = point[indices[i]];
    }
    return Multinomial(n_, pReduced).computeCDF(xReduced);
  }
  // Evaluation of P(W=n) using Poisson's formula
  Complex phiK(computeGlobalPhi(r_, point));
  const Complex zetaN(std::exp(Complex(0.0, M_PI / n_)));
  Complex phiKp1(computeGlobalPhi(r_ * zetaN, point));
  Complex delta(phiK - phiKp1);
  Scalar value = delta.real();
  const Scalar dv0 = std::abs(delta);
  if (dv0 == 0.0)
  {
    LOGWARN("Underflow in Multinomial::computeCDF");
    return 0.0;
  }
  Scalar sign = -1.0;
  Complex t(zetaN);
  for (UnsignedInteger k = 1; k < n_; ++k)
  {
    phiK = phiKp1;
    t *= zetaN;
    phiKp1 = computeGlobalPhi(r_ * t, point);
    delta = phiK - phiKp1;
    value += sign * delta.real();
    const Scalar dv = std::abs(delta);
    if (dv < SpecFunc::Precision * dv0) break;
    sign = -sign;
  }
  // Due to round-off errors, the computed CDF can be slightly below 0 or over 1.
  return std::max(0.0, std::min(1.0, value * normalizationCDF_));
}

/* Compute the scalar quantile of the 1D multinomial distribution */
Scalar Multinomial::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  return Binomial(n_, p_[0]).computeQuantile(prob, tail)[0];
} // computeScalarQuantile

/* Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1)
   For Multinomial distribution, the conditional distribution is Binomial
*/
Scalar Multinomial::computeConditionalPDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional PDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if (conditioningDimension == 0) return Binomial(n_, p_[0]).computePDF(x);
  // General case
  // Check that y is a valid conditioning vector
  UnsignedIntegerCollection intY(conditioningDimension);
  Scalar sumY = 0.0;
  Scalar sumP = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    const Scalar yI = y[i];
    const UnsignedInteger intYI = static_cast<UnsignedInteger>(round(yI));
    if (std::abs(yI - intYI) > supportEpsilon_) throw InvalidArgumentException(HERE) << "Error: the conditioning vector has non-integer values";
    sumY += yI;
    intY[i] = intYI;
    sumP += p_[i];
  }
  if (sumY > n_) throw InvalidArgumentException(HERE) << "Error: the conditioning vector has a sum of components greater than the allowed range.";
  return Binomial(static_cast<UnsignedInteger>(n_ - sumY), p_[conditioningDimension] / sumP).computePDF(x);
}

/* Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
Scalar Multinomial::computeConditionalCDF(const Scalar x,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional CDF with a conditioning point of dimension greater or equal to the distribution dimension.";
  // Special case for no conditioning or independent copula
  if (conditioningDimension == 0) return Binomial(n_, p_[0]).computeCDF(x);
  // General case
  // Check that y is a valid conditioning vector
  UnsignedIntegerCollection intY(conditioningDimension);
  Scalar sumY = 0.0;
  Scalar sumP = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    const Scalar yI = y[i];
    const UnsignedInteger intYI = static_cast<UnsignedInteger>(round(yI));
    if (std::abs(yI - intYI) > supportEpsilon_) throw InvalidArgumentException(HERE) << "Error: the conditioning vector has non-integer values";
    sumY += yI;
    intY[i] = intYI;
    sumP += p_[i];
  }
  if (sumY > n_) throw InvalidArgumentException(HERE) << "Error: the conditioning vector has a sum of components greater than the allowed range.";
  return Binomial(static_cast<UnsignedInteger>(n_ - sumY), p_[conditioningDimension] / sumP).computeCDF(x);
}

/* Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
Scalar Multinomial::computeConditionalQuantile(const Scalar q,
    const Point & y) const
{
  const UnsignedInteger conditioningDimension = y.getDimension();
  if (conditioningDimension >= getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile with a conditioning point of dimension greater or equal to the distribution dimension.";
  if ((q < 0.0) || (q > 1.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute a conditional quantile for a probability level outside of [0, 1]";
  // Special case when no contitioning or independent copula
  if (conditioningDimension == 0) return Binomial(n_, p_[0]).computeQuantile(q)[0];
  // General case
  // Check that y is a valid conditioning vector
  UnsignedIntegerCollection intY(conditioningDimension);
  Scalar sumY = 0.0;
  Scalar sumP = 0.0;
  for (UnsignedInteger i = 0; i < conditioningDimension; ++i)
  {
    const Scalar yI = y[i];
    const UnsignedInteger intYI = static_cast<UnsignedInteger>(round(yI));
    if (std::abs(yI - intYI) > supportEpsilon_) throw InvalidArgumentException(HERE) << "Error: the conditioning vector has non-integer values";
    sumY += yI;
    intY[i] = intYI;
    sumP += p_[i];
  }
  if (sumY > n_) throw InvalidArgumentException(HERE) << "Error: the conditioning vector has a sum of components greater than the allowed range.";
  return Binomial(static_cast<UnsignedInteger>(n_ - sumY), p_[conditioningDimension] / sumP).computeQuantile(q)[0];
}

/* Get the i-th marginal distribution */
Multinomial::Implementation Multinomial::getMarginal(const UnsignedInteger i) const
{
  const UnsignedInteger dimension = getDimension();
  if (i >= dimension) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  Multinomial::Implementation marginal(new Binomial(n_, p_[i]));
  marginal->setDescription(Description(1, getDescription()[i]));
  return marginal;
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Multinomial::Implementation Multinomial::getMarginal(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  if (!indices.check(dimension)) throw InvalidArgumentException(HERE) << "The indices of a marginal distribution must be in the range [0, dim-1] and must be different";
  // Special case for dimension 1
  if (dimension == 1) return clone();
  // General case
  const UnsignedInteger outputDimension = indices.getSize();
  Description description(getDescription());
  Description marginalDescription(outputDimension);
  Point marginalP(outputDimension);
  // Extract the correlation matrix, the marginal standard deviations and means
  for (UnsignedInteger i = 0; i < outputDimension; ++i)
  {
    const UnsignedInteger index_i = indices[i];
    marginalP[i] = p_[index_i];
    marginalDescription[i] = description[index_i];
  }
  Multinomial::Implementation marginal(new Multinomial(n_, marginalP));
  marginal->setDescription(marginalDescription);
  return marginal;
} // getMarginal(Indices)

/* Get the support of a discrete distribution that intersect a given interval */
Sample Multinomial::getSupport(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  Sample support(0, dimension);
  // Quick return if the lower bound of the interval is already outside of the support
  const Point lowerBound(interval.getLowerBound());
  const Point upperBound(interval.getUpperBound());
  Scalar sumLower = 0.0;
  Scalar sumUpper = 0.0;
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    // One of the components of the upper bound is negative, so the intersection with the positive quadrant is empty
    if (upperBound[i] < 0.0) return support;
    sumLower += lowerBound[i];
    sumUpper += upperBound[i];
  }
  if ((sumLower > n_) || (sumUpper < 0.0)) return support;
  // Here we know that all the components of the upper bound are positive or null
  const Sample fullSupport(getSupport());
  // Quick return if the interval contains all the support
  // It cannot be possible if the sum of the components of the upper bound is less than n_
  if ((sumUpper >= n_) && (sumLower <= 0.0))
  {
    Bool allInside = true;
    for (UnsignedInteger i = 0; i < dimension; ++i)
    {
      allInside = allInside && (lowerBound[i] <= 0.0);
      if (!allInside) break;
    }
    if (allInside) return fullSupport;
  }
  // We have to remove some points
  const UnsignedInteger size = fullSupport.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point point(fullSupport[i]);
    const Bool isInside = interval.contains(point);
    if (isInside) support.add(point);
  }
  return support;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Multinomial::getSupport() const
{
  const UnsignedInteger dimension = getDimension();
  LinearEnumerateFunction enumerate(dimension);
  const UnsignedInteger size = enumerate.getStrataCumulatedCardinal(n_);
  Sample support(0, dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    Indices multi(enumerate(i));
    support.add(Collection<Scalar>(multi.begin(), multi.end()));
  }
  return support;
}

/* Compute the mean of the distribution */
void Multinomial::computeMean() const
{
  mean_ = n_ * p_;
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void Multinomial::computeCovariance() const
{
  const UnsignedInteger dimension = getDimension();
  // To insure a zero initialization
  covariance_ = CovarianceMatrix(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    const Scalar pI = p_[i];
    covariance_(i, i) = pI * (1.0 - pI) * n_;
    // Be careful! in these computations, n_ cannot be at the begining of the formula else -n_ will underflow the UnsignedInteger range!
    for (UnsignedInteger j = 0; j < i; ++j) covariance_(i, j) = -pI * p_[j] * n_;
  }
  isAlreadyComputedCovariance_ = true;
}

/* Parameters value and description accessor */
Multinomial::PointWithDescriptionCollection Multinomial::getParametersCollection() const
{
  const UnsignedInteger dimension = getDimension();
  PointWithDescriptionCollection parameters((dimension == 1 ? 1 : dimension + 1));
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    PointWithDescription point(2);
    point[0] = n_;
    point[1] = p_[i];
    Description description(2);
    description[0] = "n";
    OSS oss;
    oss << "p_" << i;
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
      point[i + 1] = p_[i];
      OSS oss;
      oss << "p_" << i;
      description[i + 1] = oss;
    }
    point.setDescription(description);
    point.setName("dependence");
    parameters[dimension] = point;
  }
  return parameters;
}


Point Multinomial::getParameter() const
{
  Point parameter(1, n_);
  parameter.add(p_);
  return parameter;
}

Description Multinomial::getParameterDescription() const
{
  Description description(1, "n");
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger i = 0; i < dimension; ++i)
    description.add(OSS() << "p_" << i);
  return description;
}

void Multinomial::setParameter(const Point & parameter)
{
  const UnsignedInteger dimension = getDimension();
  if (parameter.getDimension() != (dimension + 1))
    throw InvalidArgumentException(HERE) << "Expected " << (dimension + 1) << " parameters";
  setN(parameter[0]);
  Point p(dimension);
  std::copy(parameter.begin() + 1, parameter.end(), p.begin());
  setP(p);
}

/* Check if the distribution is elliptical */
Bool Multinomial::isElliptical() const
{
  return (getDimension() > 1) && (p_[0] == 0.5);
}

/* P accessor */
void Multinomial::setP(const Point & p)
{
  // We check that the elements are all positive
  const UnsignedInteger dimension = p.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "P must have a positive dimension.";
  Scalar sum = 0.0;
  for(UnsignedInteger i = 0; i < dimension; ++i)
  {
    Scalar pI = p[i];
    if (!(pI >= 0.0)) throw InvalidArgumentException(HERE) << "P elements MUST be nonnegative";
    sum += pI;
  }
  if (sum > 1.0)
  {
    LOGWARN(OSS() << "P elements have a sum=" << sum << " greater than 1. It has been renormalized to 1.0");
    p_ = p / sum;
    sumP_ = 1.0;
  }
  else
  {
    p_ = p;
    sumP_ = sum;
  }
  setDimension(dimension);
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  isAlreadyCreatedGeneratingFunction_ = false;
}

/* P accessor */
Point Multinomial::getP() const
{
  return p_;
}

/* N accessor */
void Multinomial::setN(const UnsignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0.";
  if (n != n_)
  {
    n_ = n;
    // Best overall performance for Poisson's formula, see reference
    r_ = std::pow(eta_, 1.0 / (2.0 * n));
    normalizationCDF_ = std::exp(lgamma(n + 1.0) - n * std::log(1.0 * n) + n - std::log(2.0 * n) - 0.5 * std::log(eta_));
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    isAlreadyCreatedGeneratingFunction_ = false;
    computeRange();
  }
}

UnsignedInteger Multinomial::getN() const
{
  return n_;
}

/* SmallA accessor */
void Multinomial::setSmallA(const Scalar smallA)
{
  smallA_ = smallA;
}

Scalar Multinomial::getSmallA() const
{
  return smallA_;
}

/* Eta accessor */
void Multinomial::setEta(const Scalar eta)
{
  eta_ = eta;
}

Scalar Multinomial::getEta() const
{
  return eta_;
}

/* Method save() stores the object through the StorageManager */
void Multinomial::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "p_", p_ );
  adv.saveAttribute( "sumP_", sumP_ );
  adv.saveAttribute( "normalizationCDF_", normalizationCDF_ );
  adv.saveAttribute( "r_", r_ );
  adv.saveAttribute( "smallA_", smallA_ );
  adv.saveAttribute( "eta_", eta_ );
}

/* Method load() reloads the object from the StorageManager */
void Multinomial::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "p_", p_ );
  adv.loadAttribute( "sumP_", sumP_ );
  adv.loadAttribute( "normalizationCDF_", normalizationCDF_ );
  adv.loadAttribute( "r_", r_ );
  adv.loadAttribute( "smallA_", smallA_ );
  adv.loadAttribute( "eta_", eta_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
