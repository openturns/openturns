//                                               -*- C++ -*-
/**
 *  @brief The Beta distribution
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Beta.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Beta)

static const Factory<Beta> Factory_Beta;


/* Default constructor */
Beta::Beta()
  : ContinuousDistribution()
  , alpha_(2.0)
  , beta_(2.0)
  , a_(-1.0)
  , b_(1.0)
  , normalizationFactor_(std::log(0.75))
{
  setName("Beta");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
Beta::Beta(const Scalar alpha,
           const Scalar beta,
           const Scalar a,
           const Scalar b)
  : ContinuousDistribution()
  , alpha_(0.0)
  , beta_(0.0)
  , a_(a)
  , b_(b)
  , normalizationFactor_(0.0)
{
  setName("Beta");
  setA(a);
  setB(b);
  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  alpha_ = alpha;
  setBeta(beta);
  setDimension(1);
  computeRange();
}

/* Comparison operator */
Bool Beta::operator ==(const Beta & other) const
{
  if (this == &other) return true;
  return (alpha_ == other.alpha_) && (beta_ == other.beta_) &&
         (a_ == other.a_) && (b_ == other.b_);
}

Bool Beta::equals(const DistributionImplementation & other) const
{
  const Beta* p_other = dynamic_cast<const Beta*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Beta::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Beta::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " a=" << a_
      << " b=" << b_;
  return oss;
}

String Beta::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(alpha = " << alpha_ << ", beta = " << beta_ << ", a = " << a_ << ", b = " << b_ << ")";
  return oss;
}

/* Virtual constructor */
Beta * Beta::clone() const
{
  return new Beta(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Beta::computeRange()
{
  setRange(Interval(a_, b_));
}

/* Update the derivative attributes */
void Beta::update()
{
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  normalizationFactor_ = (1.0 - (alpha_ + beta_)) * std::log(b_ - a_) - SpecFunc::LnBeta(alpha_, beta_);
}


/* Get one realization of the distribution */
Point Beta::getRealization() const
{
  return Point(1, a_ + (b_ - a_) * DistFunc::rBeta(alpha_, beta_));
}

/* Get the DDF of the distribution */
Point Beta::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0];
  if ((x <= a_) || (x > b_)) return Point(1, 0.0);
  return Point(1, ((alpha_ - 1.0) / (x - a_) - (beta_ - 1.0) / (b_ - x)) * computePDF(point));
}


/* Get the PDF of the distribution */
/* Get the PDF of the distribution */
Scalar Beta::computePDF(const Scalar x) const
{
  if ((x == b_) && (beta_ == 1.0)) return 1.0;
  if ((x <= a_) || (x >= b_)) return 0.0;
  return std::exp(computeLogPDF(x));
}

Scalar Beta::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computePDF(point[0]);
}

Scalar Beta::computeLogPDF(const Scalar x) const
{
  if ((x == b_) && (beta_ == 1.0)) return 0.0;
  if ((x <= a_) || (x >= b_)) return SpecFunc::LogMinScalar;
  return normalizationFactor_ + (alpha_ - 1.0) * std::log(x - a_) + (beta_ - 1.0) * std::log(b_ - x);
}

Scalar Beta::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();
  return computeLogPDF(point[0]);
}


/* Get the CDF of the distribution */
Scalar Beta::computeCDF(const Scalar x) const
{
  if (x <= a_) return 0.0;
  if (x >= b_) return 1.0;
  return DistFunc::pBeta(alpha_, beta_, (x - a_) / (b_ - a_));
}

Scalar Beta::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  return computeCDF(point[0]);
}

/* Get the PDFGradient of the distribution */
Point Beta::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point pdfGradient(4, 0.0);
  const Scalar x = point[0];
  if ((x <= a_) || (x > b_)) return pdfGradient;
  const Scalar pdf = computePDF(point);
  const Scalar psiTR = SpecFunc::Psi(beta_);
  const Scalar iBA = 1.0 / (b_ - a_);
  const Scalar BX = b_ - x;
  const Scalar iBX = 1.0 / BX;
  const Scalar XA = x - a_;
  const Scalar iXA = 1.0 / XA;
  pdfGradient[0] = pdf * (std::log(iBA) + std::log(XA) - SpecFunc::Psi(alpha_) + SpecFunc::Psi(alpha_ + beta_));
  pdfGradient[1] = pdf * (std::log(BX * iBA) - psiTR + SpecFunc::Psi(alpha_ + beta_));
  pdfGradient[2] = pdf * (((alpha_ + beta_) - 1.0) * iBA - (alpha_ - 1.0) * iXA);
  pdfGradient[3] = pdf * (((alpha_ + beta_) - 1.0) * XA * iBA * iBX - alpha_ * iBX);
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Beta::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  Point cdfGradient(4, 0.0);
  const Scalar x = point[0];
  if ((x <= a_) || (x > b_)) return cdfGradient;
  const Scalar cdf = computeCDF(point);
  const Scalar iBA = 1.0 / (b_ - a_);
  const Scalar cdfShift = DistFunc::pBeta(alpha_ + 1.0, beta_ - 1.0, (x - a_) * iBA);
  const Scalar cdfDiff = cdfShift - cdf;
  const Scalar factor = alpha_ * iBA;
  static const Scalar eps(std::pow(cdfEpsilon_, 1.0 / 3.0));
  static const Scalar i2Eps(0.5 / eps);
  cdfGradient[0] = i2Eps * (DistFunc::pBeta(alpha_ + eps, beta_, (x - a_) / (b_ - a_)) - DistFunc::pBeta(alpha_ - eps, beta_, (x - a_) / (b_ - a_)));
  cdfGradient[1] = i2Eps * (DistFunc::pBeta(alpha_, beta_ + eps, (x - a_) / (b_ - a_)) - DistFunc::pBeta(alpha_, beta_ - eps, (x - a_) / (b_ - a_)));
  cdfGradient[3] = factor * cdfDiff;
  cdfGradient[2] = cdfGradient[3] * (b_ - x) / (x - a_);
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar Beta::computeScalarQuantile(const Scalar prob,
                                   const Bool tail) const
{
  return a_ + (b_ - a_) * DistFunc::qBeta(alpha_, beta_, prob, tail);
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex Beta::computeCharacteristicFunction(const Scalar x) const
{
  if (x == 0.0) return 1.0;
  const Scalar r1 = std::abs(x * alpha_ / (alpha_ + beta_));
  // We have numerical stability issues for large values of r1
  if (r1 <= 1.0) return std::exp(Complex(0.0, a_)) * SpecFunc::HyperGeom_1_1(alpha_, (alpha_ + beta_), Complex(0.0, (b_ - a_) * x));
  return DistributionImplementation::computeCharacteristicFunction(x);
}

/* Get the roughness, i.e. the L2-norm of the PDF */
Scalar Beta::getRoughness() const
{
  const Scalar den = SpecFunc::Beta(alpha_, beta_);
  return SpecFunc::Beta(2.0 * alpha_ - 1.0, 2.0 * (beta_) - 1.0) / (den * den * (b_ - a_));
}

/* Compute the entropy of the distribution */
Scalar Beta::computeEntropy() const
{
  return SpecFunc::LogBeta(alpha_, beta_) - (alpha_ - 1.0) * SpecFunc::Psi(alpha_) - (beta_ - 1.0) * SpecFunc::Psi(beta_) + ((alpha_ + beta_) - 2.0) * SpecFunc::Psi((alpha_ + beta_)) + std::log(b_ - a_);
}

/* Compute the mean of the distribution */
void Beta::computeMean() const
{
  mean_ = Point(1, a_ + (b_ - a_) * alpha_ / (alpha_ + beta_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Beta::getStandardDeviation() const
{
  return Point(1, (b_ - a_) / (alpha_ + beta_) * std::sqrt(alpha_ * (beta_) / ((alpha_ + beta_) + 1.0)));
}

/* Get the skewness of the distribution */
Point Beta::getSkewness() const
{
  return Point(1, 2.0 * ((alpha_ + beta_) - 2.0 * alpha_) / ((alpha_ + beta_) + 2.0) * std::sqrt(((alpha_ + beta_) + 1.0) / (alpha_ * (beta_))));
}

/* Get the kurtosis of the distribution */
Point Beta::getKurtosis() const
{
  return Point(1, 3.0 * (1.0 + (alpha_ + beta_)) * (2.0 * (alpha_ + beta_) * (alpha_ + beta_) + alpha_ * ((alpha_ + beta_) - 6.0) * (beta_)) / (alpha_ * (beta_) * (3.0 + (alpha_ + beta_)) * (2.0 + (alpha_ + beta_))));
}

/* Compute the covariance of the distribution */
void Beta::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  const Scalar eta = (b_ - a_) / (alpha_ + beta_);
  covariance_(0, 0) = eta * eta * alpha_ * (beta_) / ((alpha_ + beta_) + 1.0);
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
Point Beta::getStandardMoment(const UnsignedInteger n) const
{
  if (n == 0) return Point(1, 1.0);
  // Here we have to convert n to a signed type else -n will produce an overflow
  const Scalar value = (n % 2 == 0 ? 1.0 : -1.0) * SpecFunc::HyperGeom_2_1(alpha_, -static_cast<Scalar>(n), (alpha_ + beta_), 2.0);
  return Point(1, value);
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution Beta::getStandardRepresentative() const
{
  return new Beta(alpha_, beta_, -1.0, 1.0);
}

/* Parameters value accessor */
Point Beta::getParameter() const
{
  Point point(4);
  point[0] = alpha_;
  point[1] = beta_;
  point[2] = a_;
  point[3] = b_;
  return point;
}

void Beta::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 4) throw InvalidArgumentException(HERE) << "Error: expected 4 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Beta(parameter[0], parameter[1], parameter[2], parameter[3]);
  setWeight(w);
}

/* Parameters value and description accessor */
Description Beta::getParameterDescription() const
{
  Description description(4);
  description[0] = "alpha";
  description[1] = "beta";
  description[2] = "a";
  description[3] = "b";
  return description;
}

/* Check if the distribution is elliptical */
Bool Beta::isElliptical() const
{
  return (alpha_ + beta_) == 2.0 * alpha_;
}



/* RT accessor */
void Beta::setAlpha(const Scalar alpha)
{
  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (alpha_ != alpha)
  {
    alpha_ = alpha;
    update();
  }
}

Scalar Beta::getAlpha() const
{
  return alpha_;
}

void Beta::setBeta(const Scalar beta)
{
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";
  if (beta_ != beta)
  {
    beta_ = beta;
    update();
  }
}

Scalar Beta::getBeta() const
{
  return beta_;
}

/* R accessor */
void Beta::setR(const Scalar r)
{
  LOGWARN("Beta::setR is deprecated");
  setAlpha(r);
}

Scalar Beta::getR() const
{
  LOGWARN("Beta::getR is deprecated");
  return alpha_;
}


/* T accessor */
void Beta::setT(const Scalar t)
{
  LOGWARN("Beta::setT is deprecated");
  setBeta(t - alpha_);
}

Scalar Beta::getT() const
{
  LOGWARN("Beta::getT is deprecated");
  return alpha_ + beta_;
}

/* A accessor */
void Beta::setA(const Scalar a)
{
  if (!SpecFunc::IsNormal(a)) throw InvalidArgumentException(HERE) << "The lower bound must be a real value, here a=" << a;
  if (b_ <= a) throw InvalidArgumentException(HERE) << "The lower bound must be less than the upper bound, here a=" << a << " and b=" << b_;
  if (a != a_)
  {
    a_ = a;
    computeRange();
    update();
  }
}

Scalar Beta::getA() const
{
  return a_;
}


/* B accessor */
void Beta::setB(const Scalar b)
{
  if (!SpecFunc::IsNormal(b)) throw InvalidArgumentException(HERE) << "The upper bound must be a real value, here b=" << b;
  if (b <= a_) throw InvalidArgumentException(HERE) << "The upper bound must be greater than the lower bound, here a=" << a_ << " and b=" << b;
  if (b != b_)
  {
    b_ = b;
    computeRange();
    update();
  }
}

Scalar Beta::getB() const
{
  return b_;
}

/* Method save() stores the object through the StorageManager */
void Beta::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "a_", a_ );
  adv.saveAttribute( "b_", b_ );
  adv.saveAttribute( "normalizationFactor_", normalizationFactor_ );
}

/* Method load() reloads the object from the StorageManager */
void Beta::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  if (adv.hasAttribute("r_"))
  {
    LOGINFO("in Beta::load, using old parametrization");
    Scalar r = 0.0;
    Scalar t = 0.0;
    adv.loadAttribute( "r_", r );
    adv.loadAttribute( "t_", t );
    alpha_ = r;
    beta_ = t - r;
  }
  else
  {
    adv.loadAttribute( "alpha_", alpha_ );
    adv.loadAttribute( "beta_", beta_ );
  }
  adv.loadAttribute( "a_", a_ );
  adv.loadAttribute( "b_", b_ );
  adv.loadAttribute( "normalizationFactor_", normalizationFactor_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
