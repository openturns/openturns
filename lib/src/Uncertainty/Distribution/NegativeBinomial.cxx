//                                               -*- C++ -*-
/**
 *  @brief The NegativeBinomial distribution
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
#include "openturns/NegativeBinomial.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NegativeBinomial);

static const Factory<NegativeBinomial> Factory_NegativeBinomial;

/* Default constructor */
NegativeBinomial::NegativeBinomial()
  : DiscreteDistribution()
  , r_(1.0)
  , p_(0.5)
{
  setName( "NegativeBinomial" );
  // We set the dimension of the NegativeBinomial distribution
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
NegativeBinomial::NegativeBinomial(const NumericalScalar r,
                                   const NumericalScalar p)
  : DiscreteDistribution()
  , r_(r)
  , p_(p)
{
  setName( "NegativeBinomial" );
  // We set the dimension of the NegativeBinomial distribution
  setR(r);
  setP(p);
  setDimension( 1 );
  computeRange();
}

/* Comparison operator */
Bool NegativeBinomial::operator ==(const NegativeBinomial & other) const
{
  if (this == &other) return true;
  return (r_ == other.r_) && (p_ == other.p_);
}

Bool NegativeBinomial::equals(const DistributionImplementation & other) const
{
  const NegativeBinomial* p_other = dynamic_cast<const NegativeBinomial*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String NegativeBinomial::__repr__() const
{
  OSS oss;
  oss << "class=" << NegativeBinomial::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " r=" << r_
      << " p=" << p_;
  return oss;
}

String NegativeBinomial::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(r = " << r_ << ", p = " << p_ << ")";
  return oss;
}

/* Virtual constructor */
NegativeBinomial * NegativeBinomial::clone() const
{
  return new NegativeBinomial(*this);
}

/* Get one realization of the distribution */
NumericalPoint NegativeBinomial::getRealization() const
{
  return NumericalPoint(1, DistFunc::rPoisson(DistFunc::rGamma(r_) * p_ / (1.0 - p_)));
}


/* Get the PDF of the distribution */
NumericalScalar NegativeBinomial::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return 0.0;
  return std::exp(SpecFunc::LnGamma(k + r_) - SpecFunc::LnGamma(r_) - SpecFunc::LnGamma(k + 1.0) + k * std::log(p_) + r_ * log1p(-p_));
}


/* Get the CDF of the distribution */
NumericalScalar NegativeBinomial::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return 0.0;
  if (p_ <= 0.5) return DistFunc::pBeta(r_, floor(k) + 1, 1.0 - p_, false);
  return DistFunc::pBeta(floor(k) + 1, r_, p_, true);
}

NumericalScalar NegativeBinomial::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return 1.0;
  // Complementary relation for incomplete regularized Beta function: I(a, b, x) = 1 - I(b, a, 1-x)
  if (p_ <= 0.5) return DistFunc::pBeta(r_, floor(k) + 1, 1.0 - p_, true);
  return DistFunc::pBeta(floor(k) + 1, r_, p_, false);
}

/* Get the PDF gradient of the distribution */
NumericalPoint NegativeBinomial::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  NumericalPoint pdfGradient(1, 0.0);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return pdfGradient;
  throw NotYetImplementedException(HERE) << "In NegativeBinomial::computePDFGradient(const NumericalPoint & point) const";
}


/* Get the CDF gradient of the distribution */
NumericalPoint NegativeBinomial::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar k(point[0]);
  if (k < -supportEpsilon_) return NumericalPoint(1, 0.0);
  throw NotYetImplementedException(HERE) << "In NegativeBinomial::computeCDFGradient(const NumericalPoint & point) const";
}

/* Compute the mean of the distribution */
void NegativeBinomial::computeMean() const
{
  mean_ = NumericalPoint(1, r_ * p_ / (1.0 - p_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint NegativeBinomial::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(r_ * p_) / (1.0 - p_));
}

/* Get the skewness of the distribution */
NumericalPoint NegativeBinomial::getSkewness() const
{
  return NumericalPoint(1, (1.0 + p_) / std::sqrt(p_ * r_));
}

/* Get the kurtosis of the distribution */
NumericalPoint NegativeBinomial::getKurtosis() const
{
  return NumericalPoint(1, 3.0 + 6.0 / r_ + std::pow(1.0 - p_, 2.0) / (p_ * r_));
}

/* Compute the covariance of the distribution */
void NegativeBinomial::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = r_ * p_ / std::pow(1.0 - p_, 2.0);
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
NumericalSample NegativeBinomial::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const UnsignedInteger kMin(static_cast< UnsignedInteger > (std::max(ceil(interval.getLowerBound()[0]), 0.0)));
  const UnsignedInteger kMax(static_cast< UnsignedInteger > (std::min(floor(interval.getUpperBound()[0]), getRange().getUpperBound()[0])));
  NumericalSample result(0, 1);
  for (UnsignedInteger k = kMin; k <= kMax; ++k) result.add(NumericalPoint(1, k));
  return result;
}

/* Parameters value accessor */
NumericalPoint NegativeBinomial::getParameter() const
{
  NumericalPoint point(2);
  point[0] = r_;
  point[1] = p_;
  return point;
}

void NegativeBinomial::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = NegativeBinomial(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description NegativeBinomial::getParameterDescription() const
{
  Description description(2);
  description[0] = "r";
  description[1] = "p";
  return description;
}

/* P accessor */
void NegativeBinomial::setP(const NumericalScalar p)
{
  if ((p <= 0.0) || (p >= 1.0)) throw InvalidArgumentException(HERE) << "Error: p must be in (0, 1), here p=" << p;
  if (p != p_)
  {
    p_ = p;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* P accessor */
NumericalScalar NegativeBinomial::getP() const
{
  return p_;
}

/* N accessor */
void NegativeBinomial::setR(const NumericalScalar r)
{
  if (r <= 0.0) throw InvalidArgumentException(HERE) << "Error: r must be > 0, here r=" << r;
  if (r != r_)
  {
    r_ = r;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* N accessor */
NumericalScalar NegativeBinomial::getR() const
{
  return r_;
}

/* Compute the numerical range of the distribution given the parameters values */
void NegativeBinomial::computeRange()
{
  const NumericalPoint lowerBound(1, 0.0);
  const NumericalPoint upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get the quantile of the distribution */
NumericalScalar NegativeBinomial::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  LOGDEBUG(OSS() << "in NegativeBinomial::computeScalarQuantile, prob=" << prob << ", tail=" << (tail ? "true" : "false"));
  if (prob <= 0.0) return (tail ? getRange().getUpperBound()[0] : 0.0);
  if (prob >= 1.0) return (tail ? 0.0 : getRange().getUpperBound()[0]);
  // Initialization by the Cornish-Fisher expansion
  NumericalScalar qNorm(DistFunc::qNormal(prob, tail));
  NumericalScalar gamma1(getSkewness()[0]);
  NumericalScalar gamma2(getKurtosis()[0] - 3.0);
  NumericalScalar quantile(round(getMean()[0] + getStandardDeviation()[0] * (qNorm + (qNorm * qNorm - 1.0) * gamma1 / 6.0 + qNorm * (qNorm * qNorm - 3.0) * gamma2 / 24.0 - qNorm * (2.0 * qNorm * qNorm - 5.0) * gamma1 * gamma1 / 36.0)));
  if (quantile < 0.0) quantile = 0.0;
  NumericalScalar cdf(tail ? computeComplementaryCDF(quantile) : computeCDF(quantile));
  LOGDEBUG(OSS() << "in NegativeBinomial::computeScalarQuantile, Cornish-Fisher estimate=" << quantile << ", cdf=" << cdf);
  NumericalScalar oldCDF(cdf);
  const NumericalScalar step(tail ? -1.0 : 1.0);
  while (cdf >= prob)
  {
    quantile -= step;
    oldCDF = cdf;
    cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
    LOGDEBUG(OSS() << "in NegativeBinomial::computeScalarQuantile, backward search, quantile=" << quantile << ", cdf=" << cdf);
  }
  if (cdf < oldCDF)
  {
    quantile += step;
    LOGDEBUG(OSS() << "in NegativeBinomial::computeScalarQuantile, final quantile=" << quantile);
    return quantile;
  }
  while (cdf < prob)
  {
    quantile += step;
    oldCDF = cdf;
    cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
    LOGDEBUG(OSS() << "in NegativeBinomial::computeScalarQuantile, forward search, quantile=" << quantile << ", cdf=" << cdf);
  }
  LOGDEBUG(OSS() << "in NegativeBinomial::computeScalarQuantile, final quantile=" << quantile);
  return quantile;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex NegativeBinomial::computeCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalComplex value((1.0 - p_) / (1.0 - p_ * std::exp(NumericalComplex(0.0, x))));
  return std::pow(value, r_);
}

NumericalComplex NegativeBinomial::computeLogCharacteristicFunction(const NumericalScalar x) const
{
  const NumericalComplex value((1.0 - p_) / (1.0 - p_ * std::exp(NumericalComplex(0.0, x))));
  return NumericalComplex(r_) * std::log(value);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
NumericalComplex NegativeBinomial::computeGeneratingFunction(const NumericalComplex & z) const
{
  const NumericalComplex value((1.0 - p_) / (1.0 - p_ * z));
  return std::pow(value, r_);
}

NumericalComplex NegativeBinomial::computeLogGeneratingFunction(const NumericalComplex & z) const
{
  const NumericalComplex value((1.0 - p_) / (1.0 - p_ * z));
  return NumericalComplex(r_) * std::log(value);
}

/* Method save() stores the object through the StorageManager */
void NegativeBinomial::save(Advocate & adv) const
{
  DiscreteDistribution::save(adv);
  adv.saveAttribute( "r_", r_ );
  adv.saveAttribute( "p_", p_ );
}

/* Method load() reloads the object from the StorageManager */
void NegativeBinomial::load(Advocate & adv)
{
  DiscreteDistribution::load(adv);
  adv.loadAttribute( "r_", r_ );
  adv.loadAttribute( "p_", p_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
