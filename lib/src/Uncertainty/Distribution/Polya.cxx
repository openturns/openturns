//                                               -*- C++ -*-
/**
 *  @brief The Polya distribution
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Polya.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Polya)

static const Factory<Polya> Factory_Polya;

/* Default constructor */
Polya::Polya()
  : DistributionImplementation()
  , r_(1.0)
  , p_(0.5)
{
  setName( "Polya" );
  // We set the dimension of the Polya distribution
  setDimension( 1 );
  computeRange();
}

/* Parameters constructor */
Polya::Polya(const Scalar r,
             const Scalar p)
  : DistributionImplementation()
  , r_(r)
  , p_(p)
{
  setName( "Polya" );
  // We set the dimension of the Polya distribution
  setR(r);
  setP(p);
  setDimension( 1 );
  computeRange();
}

/* Tell if the distribution is continuous */
Bool Polya::isContinuous() const
{
  return false;
}

/* Tell if the distribution is discrete */
Bool Polya::isDiscrete() const
{
  return true;
}

/* Tell if the distribution is integer valued */
Bool Polya::isIntegral() const
{
  return true;
}

/* Comparison operator */
Bool Polya::operator ==(const Polya & other) const
{
  if (this == &other) return true;
  return (r_ == other.r_) && (p_ == other.p_);
}

Bool Polya::equals(const DistributionImplementation & other) const
{
  const Polya* p_other = dynamic_cast<const Polya*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Polya::__repr__() const
{
  OSS oss;
  oss << "class=" << Polya::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " r=" << r_
      << " p=" << p_;
  return oss;
}

String Polya::__str__(const String & ) const
{
  OSS oss;
  oss << getClassName() << "(r = " << r_ << ", p = " << p_ << ")";
  return oss;
}

/* Virtual constructor */
Polya * Polya::clone() const
{
  return new Polya(*this);
}

/* Get one realization of the distribution */
Point Polya::getRealization() const
{
  return Point(1, DistFunc::rPoisson(DistFunc::rGamma(r_) * p_ / (1.0 - p_)));
}


/* Get the PDF of the distribution */
Scalar Polya::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return 0.0;
  return std::exp(SpecFunc::LogGamma(k + r_) - SpecFunc::LogGamma(r_) - SpecFunc::LogGamma(k + 1.0) + k * std::log(p_) + r_ * log1p(-p_));
}


/* Get the CDF of the distribution */
Scalar Polya::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return 0.0;
  if (p_ <= 0.5) return DistFunc::pBeta(r_, floor(k) + 1, 1.0 - p_, false);
  return DistFunc::pBeta(floor(k) + 1, r_, p_, true);
}

Scalar Polya::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return 1.0;
  // Complementary relation for incomplete regularized Beta function: I(a, b, x) = 1 - I(b, a, 1-x)
  if (p_ <= 0.5) return DistFunc::pBeta(r_, floor(k) + 1, 1.0 - p_, true);
  return DistFunc::pBeta(floor(k) + 1, r_, p_, false);
}

/* Get the PDF gradient of the distribution */
Point Polya::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  Point pdfGradient(1, 0.0);
  if ((k < -supportEpsilon_) || (std::abs(k - round(k)) > supportEpsilon_)) return pdfGradient;
  throw NotYetImplementedException(HERE) << "In Polya::computePDFGradient(const Point & point) const";
}


/* Get the CDF gradient of the distribution */
Point Polya::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar k = point[0];
  if (k < -supportEpsilon_) return Point(1, 0.0);
  throw NotYetImplementedException(HERE) << "In Polya::computeCDFGradient(const Point & point) const";
}

/* Compute the mean of the distribution */
void Polya::computeMean() const
{
  mean_ = Point(1, r_ * p_ / (1.0 - p_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Polya::getStandardDeviation() const
{
  return Point(1, std::sqrt(r_ * p_) / (1.0 - p_));
}

/* Get the skewness of the distribution */
Point Polya::getSkewness() const
{
  return Point(1, (1.0 + p_) / std::sqrt(p_ * r_));
}

/* Get the kurtosis of the distribution */
Point Polya::getKurtosis() const
{
  return Point(1, 3.0 + 6.0 / r_ + std::pow(1.0 - p_, 2.0) / (p_ * r_));
}

/* Compute the covariance of the distribution */
void Polya::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = r_ * p_ / std::pow(1.0 - p_, 2.0);
  isAlreadyComputedCovariance_ = true;
}

/* Get the support of a discrete distribution that intersect a given interval */
Sample Polya::getSupport(const Interval & interval) const
{
  if (interval.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: the given interval has a dimension that does not match the distribution dimension.";
  const SignedInteger kMin = static_cast< SignedInteger > (std::max(0.0, ceil(interval.getLowerBound()[0])));
  const SignedInteger kMax = static_cast< SignedInteger > (std::min(getRange().getUpperBound()[0], floor(interval.getUpperBound()[0])));
  Sample result(0, 1);
  for (SignedInteger k = kMin; k <= kMax; ++ k)
    result.add(Point(1, k));
  return result;
}

/* Parameters value accessor */
Point Polya::getParameter() const
{
  Point point(2);
  point[0] = r_;
  point[1] = p_;
  return point;
}

void Polya::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 2) throw InvalidArgumentException(HERE) << "Error: expected 2 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Polya(parameter[0], parameter[1]);
  setWeight(w);
}

/* Parameters description accessor */
Description Polya::getParameterDescription() const
{
  Description description(2);
  description[0] = "r";
  description[1] = "p";
  return description;
}

/* P accessor */
void Polya::setP(const Scalar p)
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
Scalar Polya::getP() const
{
  return p_;
}

/* N accessor */
void Polya::setR(const Scalar r)
{
  if (!(r > 0.0)) throw InvalidArgumentException(HERE) << "Error: r must be > 0, here r=" << r;
  if (r != r_)
  {
    r_ = r;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

/* N accessor */
Scalar Polya::getR() const
{
  return r_;
}

/* Compute the numerical range of the distribution given the parameters values */
void Polya::computeRange()
{
  const Point lowerBound(1, 0.0);
  Scalar upper = 1.0;
  Scalar step = 1.0;
  while (computeComplementaryCDF(upper) > cdfEpsilon_)
    {
      upper += step;
      step *=2.0;
    }
  const Point upperBound(1, upper);
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get the quantile of the distribution */
Scalar Polya::computeScalarQuantile(const Scalar prob,
                                    const Bool tail) const
{
  LOGDEBUG(OSS() << "in Polya::computeScalarQuantile, prob=" << prob << ", tail=" << tail);
  if (!((prob >= 0.0) && (prob <= 1.0)))
    throw InvalidArgumentException(HERE) << "computeScalarQuantile expected prob to belong to [0,1], but is " << prob;
  if (tail ? (prob == 1.0) : (prob == 0.0)) return 0.0;
  if (tail ? (prob == 0.0) : (prob == 1.0)) return SpecFunc::Infinity;
  // Initialization by the Cornish-Fisher expansion
  Scalar qNorm = DistFunc::qNormal(prob, tail);
  Scalar gamma1 = getSkewness()[0];
  Scalar gamma2 = getKurtosis()[0] - 3.0;
  Scalar quantile = round(getMean()[0] + getStandardDeviation()[0] * (qNorm + (qNorm * qNorm - 1.0) * gamma1 / 6.0 + qNorm * (qNorm * qNorm - 3.0) * gamma2 / 24.0 - qNorm * (2.0 * qNorm * qNorm - 5.0) * gamma1 * gamma1 / 36.0));
  if (quantile < 0.0) quantile = 0.0;
  Scalar cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
  LOGDEBUG(OSS() << "in Polya::computeScalarQuantile, Cornish-Fisher estimate=" << quantile << ", cdf=" << cdf);
  Scalar oldCDF = cdf;
  Scalar step = tail ? -1.0 : 1.0;
  while (cdf >= prob)
  {
    quantile -= step;
    oldCDF = cdf;
    cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
    LOGDEBUG(OSS() << "in Polya::computeScalarQuantile, backward search, quantile=" << quantile << ", cdf=" << cdf);
    step *= 2;
  }
  step /= 2;
  if (cdf < oldCDF)
  {
    quantile += step;
    LOGDEBUG(OSS() << "in Polya::computeScalarQuantile, final quantile=" << quantile);
    return quantile;
  }
  while (cdf < prob)
  {
    quantile += step;
    oldCDF = cdf;
    cdf = tail ? computeComplementaryCDF(quantile) : computeCDF(quantile);
    LOGDEBUG(OSS() << "in Polya::computeScalarQuantile, forward search, quantile=" << quantile << ", cdf=" << cdf);
  }
  LOGDEBUG(OSS() << "in Polya::computeScalarQuantile, final quantile=" << quantile);
  return quantile;
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Polya::computeCharacteristicFunction(const Scalar x) const
{
  const Complex value((1.0 - p_) / (1.0 - p_ * std::exp(Complex(0.0, x))));
  return std::pow(value, r_);
}

Complex Polya::computeLogCharacteristicFunction(const Scalar x) const
{
  const Complex value((1.0 - p_) / (1.0 - p_ * std::exp(Complex(0.0, x))));
  return Complex(r_) * std::log(value);
}

/* Get the generating function of the distribution, i.e. psi(z) = E(z^X) */
Complex Polya::computeGeneratingFunction(const Complex & z) const
{
  const Complex value((1.0 - p_) / (1.0 - p_ * z));
  return std::pow(value, r_);
}

Complex Polya::computeLogGeneratingFunction(const Complex & z) const
{
  const Complex value((1.0 - p_) / (1.0 - p_ * z));
  return Complex(r_) * std::log(value);
}

/* Method save() stores the object through the StorageManager */
void Polya::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute( "r_", r_ );
  adv.saveAttribute( "p_", p_ );
}

/* Method load() reloads the object from the StorageManager */
void Polya::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute( "r_", r_ );
  adv.loadAttribute( "p_", p_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
