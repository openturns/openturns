//                                               -*- C++ -*-
/**
 *  @brief The Binomial distribution
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/Binomial.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Binomial);

static const Factory<Binomial> RegisteredFactory;

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
                   const NumericalScalar p)
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

String Binomial::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(n = " << n_ << ", p = " << p_ << ")";
  return oss;
}

/* Virtual constructor */
Binomial * Binomial::clone() const
{
  return new Binomial(*this);
}

/* Get one realization of the distribution */
NumericalPoint Binomial::getRealization() const
{
  return NumericalPoint(1, DistFunc::rBinomial(n_, p_));
}


/* Get the PDF of the distribution */
NumericalScalar Binomial::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_) || (k > n_ + supportEpsilon_)) return 0.0;
  return std::exp(SpecFunc::LnGamma(n_ + 1.0) - SpecFunc::LnGamma(n_ - k + 1.0) - SpecFunc::LnGamma(k + 1.0) + k * std::log(p_) + (n_ - k) * log1p(-p_));
}


/* Get the CDF of the distribution */
NumericalScalar Binomial::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return 0.0;
  if (k > n_ - supportEpsilon_) return 1.0;
  // Complementary relation for incomplete regularized Beta function: I(a, b, x) = 1 - I(b, a, 1-x)
  return DistFunc::pBeta(n_ - floor(k), floor(k) + 1, 1.0 - p_);
}

NumericalScalar Binomial::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return 1.0;
  if (k > n_ + supportEpsilon_) return 0.0;
  // Complementary relation for incomplete regularized Beta function: I(a, b, x) = 1 - I(b, a, 1-x)
  NumericalScalar value = DistFunc::pBeta(floor(k) + 1, n_ - floor(k), p_);
  return value;
}

/* Get the PDF gradient of the distribution */
NumericalPoint Binomial::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  NumericalPoint pdfGradient(1, 0.0);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return pdfGradient;
  throw NotYetImplementedException(HERE) << "In Binomial::computePDFGradient(const NumericalPoint & point) const";
}


/* Get the CDF gradient of the distribution */
NumericalPoint Binomial::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return NumericalPoint(1, 0.0);
  throw NotYetImplementedException(HERE) << "In Binomial::computeCDFGradient(const NumericalPoint & point) const";
}

/* Compute the mean of the distribution */
void Binomial::computeMean() const
{
  mean_ = NumericalPoint(1, n_ * p_);
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Binomial::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(n_ * p_ * (1.0 - p_)));
}

/* Get the skewness of the distribution */
NumericalPoint Binomial::getSkewness() const
{
  if ((p_ == 0.0) || (p_ == 1.0)) throw NotDefinedException(HERE) << "Error: the skewness is not defined for the Binomial distribution when p is zero or one.";
  return NumericalPoint(1, (1.0 - 2.0 * p_) / std::sqrt(n_ * p_ * (1.0 - p_)));
}

/* Get the kurtosis of the distribution */
NumericalPoint Binomial::getKurtosis() const
{
  if ((p_ == 0.0) || (p_ == 1.0)) throw NotDefinedException(HERE) << "Error: the kurtosis is not defined for the Binomial distribution when p is zero or one.";
  return NumericalPoint(1, 3.0 - 6.0 / n_ + 1.0 / (n_ * p_ * (1.0 - p_)));
}

/* Compute the covariance of the distribution */
void Binomial::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = n_ * p_ * (1.0 - p_);
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
NumericalSample Binomial::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const UnsignedInteger kMin(static_cast< UnsignedInteger > (std::max(ceil(interval.getLowerBound()[0]), 0.0)));
  const UnsignedInteger kMax(static_cast< UnsignedInteger > (std::min(floor(interval.getUpperBound()[0]), NumericalScalar(n_))));
  NumericalSample result(0, 1);
  for (UnsignedInteger k = kMin; k <= kMax; ++k) result.add(NumericalPoint(1, k));
  return result;
}

/* Parameters value accessor */
NumericalPoint Binomial::getParameter() const
{
  NumericalPoint point(2);
  point[0] = n_;
  point[1] = p_;
  return point;
}

void Binomial::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
  *this = Binomial(static_cast<UnsignedInteger>(parameter[0]), parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Binomial::getParameterDescription() const
{
  Description description(2);
  description[0] = "n";
  description[1] = "p";
  return description;
}

/* Check if the distribution is elliptical */
Bool Binomial::isElliptical() const
{
  return p_ == 0.5;
}

/* P accessor */
void Binomial::setP(const NumericalScalar p)
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
NumericalScalar Binomial::getP() const
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
NumericalScalar Binomial::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  LOGDEBUG(OSS() << "in Binomial::computeScalarQuantile, prob=" << prob << ", tail=" << (tail ? "true" : "false"));
  if (prob <= 0.0) return (tail ? n_ : 0.0);
  if (prob >= 1.0) return (tail ? 0.0 : n_);
  // Initialization by the Cornish-Fisher expansion
  NumericalScalar qNorm(DistFunc::qNormal(prob, tail));
  NumericalScalar gamma1(getSkewness()[0]);
  NumericalScalar gamma2(getKurtosis()[0] - 3.0);
  NumericalScalar quantile(round(getMean()[0] + getStandardDeviation()[0] * (qNorm + (qNorm * qNorm - 1.0) * gamma1 / 6.0 + qNorm * (qNorm * qNorm - 3.0) * gamma2 / 24.0 - qNorm * (2.0 * qNorm * qNorm - 5.0) * gamma1 * gamma1 / 36.0)));
  if (quantile < 0.0) quantile = 0.0;
  if (quantile > n_) quantile = n_;
  // CDF of the guess
  NumericalScalar cdf(tail ? computeComplementaryCDF(quantile) : computeCDF(quantile));
  LOGDEBUG(OSS() << "in Binomial::computeScalarQuantile, Cornish-Fisher estimate=" << quantile << ", cdf=" << cdf);
  NumericalScalar oldCDF(cdf);
  const NumericalScalar step(tail ? -1.0 : 1.0);
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
NumericalComplex Binomial::computeCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalComplex value(1.0 - p_ + p_ * std::exp(NumericalComplex(0.0, x)));
  return std::pow(value, n_);
}

NumericalComplex Binomial::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalComplex value(1.0 - p_ + p_ * std::exp(NumericalComplex(0.0, x)));
  return NumericalScalar(n_) * std::log(value);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
NumericalComplex Binomial::computeGeneratingFunction(const NumericalComplex & z) const
{
  const NumericalComplex value(1.0 - p_ + p_ * z);
  return std::pow(value, n_);
}

NumericalComplex Binomial::computeLogGeneratingFunction(const NumericalComplex & z) const
{
  const NumericalComplex value(1.0 - p_ + p_ * z);
  return NumericalComplex(n_) * std::log(value);
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
