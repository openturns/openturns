//                                               -*- C++ -*-
/**
 *  @brief The Binomial distribution
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
#include "openturns/Binomial.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Binomial)

static const Factory<Binomial> Factory_Binomial;

/* Default constructor */
Binomial::Binomial()
  : DiscreteDistribution(),
    n_(1),
    p_(0.5)
{
  setName( "Binomial" );
  // We set the dimension of the Binomial distribution
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Binomial::Binomial(const UnsignedInteger n,
                   const Scalar p)
  : DiscreteDistribution(),
    n_(n),
    p_(-1.0)
{
  setName( "Binomial" );
  // This call sets also the range
  setP(p);
  // We set the dimension of the Binomial distribution
  setDimension( 1 );
}

/* Comparison operator */
Bool Binomial::operator ==(const Binomial & other) const
{
  if (this == &other) return true;
  return (n_ == other.n_) && (p_ == other.p_);
}

Bool Binomial::equals(const DistributionImplementation & other) const
{
  const Binomial* p_other = dynamic_cast<const Binomial*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Binomial::__repr__() const
{
  OSS oss;
  oss << "class=" << Binomial::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " n=" << n_
      << " p=" << p_;
  return oss;
}

String Binomial::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(n = " << n_ << ", p = " << p_ << ")";
  return oss;
}

/* Virtual constructor */
Binomial * Binomial::clone() const
{
  return new Binomial(*this);
}

/* Get one realization of the distribution */
Point Binomial::getRealization() const
{
  return Point(1, DistFunc::rBinomial(n_, p_));
}


/* Get the PDF of the distribution */
Scalar Binomial::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_) || (k > n_ + supportEpsilon_)) return 0.0;
  return DistFunc::dBinomial(n_, p_, static_cast<UnsignedInteger>(round(k)));
}


Scalar Binomial::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_) || (k > n_ + supportEpsilon_)) return SpecFunc::LowestScalar;
  return DistFunc::logdBinomial(n_, p_, static_cast<UnsignedInteger>(round(k)));
}


/* Get the CDF of the distribution */
Scalar Binomial::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return 0.0;
  if (k > n_ - supportEpsilon_) return 1.0;
  // Complementary relation for incomplete regularized Beta function: I(a, b, x) = 1 - I(b, a, 1-x)
  return DistFunc::pBeta(n_ - floor(k), floor(k) + 1, 1.0 - p_);
}

Scalar Binomial::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return 1.0;
  if (k > n_ - supportEpsilon_) return 0.0;
  // Complementary relation for incomplete regularized Beta function: I(a, b, x) = 1 - I(b, a, 1-x)
  Scalar value = DistFunc::pBeta(floor(k) + 1, n_ - floor(k), p_);
  return value;
}

/* Get the PDF gradient of the distribution */
Point Binomial::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  Point pdfGradient(1, 0.0);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return pdfGradient;
  throw NotYetImplementedException(HERE) << "In Binomial::computePDFGradient(const Point & point) const";
}


/* Get the CDF gradient of the distribution */
Point Binomial::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return Point(1, 0.0);
  throw NotYetImplementedException(HERE) << "In Binomial::computeCDFGradient(const Point & point) const";
}

/* Compute the mean of the distribution */
void Binomial::computeMean() const
{
  mean_ = Point(1, n_ * p_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Binomial::getStandardDeviation() const
{
  return Point(1, std::sqrt(n_ * p_ * (1.0 - p_)));
}

/* Get the skewness of the distribution */
Point Binomial::getSkewness() const
{
  if ((p_ == 0.0) || (p_ == 1.0)) throw NotDefinedException(HERE) << "Error: the skewness is not defined for the Binomial distribution when p is zero or one.";
  return Point(1, (1.0 - 2.0 * p_) / std::sqrt(n_ * p_ * (1.0 - p_)));
}

/* Get the kurtosis of the distribution */
Point Binomial::getKurtosis() const
{
  if ((p_ == 0.0) || (p_ == 1.0)) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for the Binomial distribution when p is zero or one.";
  return Point(1, 3.0 - 6.0 / n_ + 1.0 / (n_ * p_ * (1.0 - p_)));
}

/* Compute the covariance of the distribution */
void Binomial::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = n_ * p_ * (1.0 - p_);
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Binomial::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const SignedInteger kMin = static_cast< SignedInteger > (std::max(0.0, ceil(interval.getLowerBound()[0])));
  const SignedInteger kMax = static_cast< SignedInteger > (std::min(static_cast<Scalar>(n_), floor(interval.getUpperBound()[0])));
  Sample result(0, 1);
  for (SignedInteger k = kMin; k <= kMax; ++ k)
    result.add(Point(1, k));
  return result;
}

/* Parameters value accessor */
Point Binomial::getParameter() const
{
  return {static_cast<Scalar>(n_), p_};
}

void Binomial::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Binomial(static_cast<UnsignedInteger>(parameter[0]), parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Binomial::getParameterDescription() const
{
  return {"n", "p"};
}

/* Check if the distribution is elliptical */
Bool Binomial::isElliptical() const
{
  return p_ == 0.5;
}

/* P accessor */
void Binomial::setP(const Scalar p)
{
  if ((p < 0.0) || (p > 1.0)) throw InvalidArgumentException(HERE) << "P must be in [0, 1], here p=" << p;
  if (p != p_)
  {
    p_ = p;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* P accessor */
Scalar Binomial::getP() const
{
  return p_;
}

/* N accessor */
void Binomial::setN(const UnsignedInteger n)
{
  if (n != n_)
  {
    n_ = n;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* N accessor */
UnsignedInteger Binomial::getN() const
{
  return n_;
}

/* Compute the numerical range of the distribution given the parameters values */
void Binomial::computeRange()
{
  // Degenerate cases
  if (p_ == 0.0) setRange(Interval(0.0, 0.0));
  if (p_ == 1.0) setRange(Interval(n_, n_));
  // Usual case
  setRange(Interval(0.0, n_));
}

/* Get the quantile of the distribution */
Scalar Binomial::computeScalarQuantile(const Scalar prob,
                                       const Bool tail) const
{
  LOGDEBUG(OSS() << "in Binomial::computeScalarQuantile, prob=" << prob << ", tail=" << tail);
  if (prob <= 0.0) return (tail ? n_ : 0.0);
  if (prob >= 1.0) return (tail ? 0.0 : n_);
  // Initialization by the Cornish-Fisher expansion
  Scalar qNorm = DistFunc::qNormal(prob, tail);
  Scalar gamma1 = getSkewness()[0];
  Scalar gamma2 = getKurtosis()[0] - 3.0;
  Scalar quantile = round(getMean()[0] + getStandardDeviation()[0] * (qNorm + (qNorm * qNorm - 1.0) * gamma1 / 6.0 + qNorm * (qNorm * qNorm - 3.0) * gamma2 / 24.0 - qNorm * (2.0 * qNorm * qNorm - 5.0) * gamma1 * gamma1 / 36.0));
  if (quantile < 0.0) quantile = 0.0;
  if (quantile > n_) quantile = n_;
  // CDF of the guess
  Scalar cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
  LOGDEBUG(OSS() << "in Binomial::computeScalarQuantile, Cornish-Fisher estimate=" << quantile << ", cdf=" << cdf);
  Scalar oldCDF = cdf;
  const Scalar step = tail ? -1.0 : 1.0;
  // Do we have to
  while (cdf >= prob)
  {
    quantile -= step;
    oldCDF = cdf;
    cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
    LOGDEBUG(OSS() << "in Binomial::computeScalarQuantile, backward search, quantile=" << quantile << ", cdf=" << cdf);
  }
  // If
  if (cdf < oldCDF)
  {
    quantile += step;
    LOGDEBUG(OSS() << "in Binomial::computeScalarQuantile, final quantile=" << quantile);
    return quantile;
  }
  while (cdf < prob)
  {
    quantile += step;
    cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
    LOGDEBUG(OSS() << "in Binomial::computeScalarQuantile, forward search, quantile=" << quantile << ", cdf=" << cdf);
  }
  LOGDEBUG(OSS() << "in Binomial::computeScalarQuantile, final quantile=" << quantile);
  return quantile;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Binomial::computeCharacteristicFunction(const Scalar x) const
{
  const Complex value(1.0 - p_ + p_ * std::exp(Complex(0.0, x)));
  return std::pow(value, n_);
}

Complex Binomial::computeLogCharacteristicFunction(const Scalar x) const
{
  const Complex value(1.0 - p_ + p_ * std::exp(Complex(0.0, x)));
  return Scalar(n_) * std::log(value);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Complex Binomial::computeGeneratingFunction(const Complex & z) const
{
  const Complex value(1.0 - p_ + p_ * z);
  return std::pow(value, n_);
}

Complex Binomial::computeLogGeneratingFunction(const Complex & z) const
{
  const Complex value(1.0 - p_ + p_ * z);
  return Complex(n_) * std::log(value);
}

/* Method save() stores the object through the StorageManager */
void Binomial::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "n_", n_ );
  adv.saveAttribute( "p_", p_ );
}

/* Method load() reloads the object from the StorageManager */
void Binomial::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "n_", n_ );
  adv.loadAttribute( "p_", p_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
