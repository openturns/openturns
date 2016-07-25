//                                               -*- C++ -*-
/**
 *  @brief The Frechet distribution
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
#include "openturns/Frechet.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Frechet);

static const Factory<Frechet> Factory_Frechet;

/* Parameters constructor */
Frechet::Frechet(const NumericalScalar alpha,
                 const NumericalScalar beta,
                 const NumericalScalar gamma)
  : ContinuousDistribution()
  , alpha_(alpha)
  , beta_(beta)
  , gamma_(gamma)
{
  setAlpha(alpha);
  setBeta(beta);
  setGamma(gamma);
  setName("Frechet");
  setDimension(1);
  computeRange();
}

/* Comparison operator */
Bool Frechet::operator ==(const Frechet & other) const
{
  if (this == &other) return true;
  return (alpha_ == other.alpha_) && (beta_ == other.beta_) && (gamma_ == other.gamma_);
}

Bool Frechet::equals(const DistributionImplementation & other) const
{
  const Frechet* p_other = dynamic_cast<const Frechet*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Frechet::__repr__() const
{
  OSS oss;
  oss << "class=" << Frechet::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " gamma=" << gamma_;
  return oss;
}

String Frechet::__str__(const String & offset) const
{
  OSS oss;
  oss << offset << getClassName() << "(alpha = " << alpha_ << ", beta = " << beta_ << ", gamma = " << gamma_ << ")";
  return oss;
}


/* Virtual constructor */
Frechet * Frechet::clone() const
{
  return new Frechet(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Frechet::computeRange()
{
  const NumericalPoint lowerBound(1, gamma_);
  const NumericalPoint upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Get the quantile of the distribution */
NumericalScalar Frechet::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  return gamma_ + beta_ * std::pow(-std::log(tail ? 1.0 - prob : prob), -1.0 / alpha_);
}

/* Get one realization of the distribution */
NumericalPoint Frechet::getRealization() const
{
  return NumericalPoint(1, computeScalarQuantile(RandomGenerator::Generate()));
}

NumericalPoint Frechet::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - gamma_);
  if (x <= 0.0) return NumericalPoint(1, 0.0);
  const NumericalScalar y(x / beta_);
  const NumericalScalar minusAlphalogY(-alpha_ * std::log(y));
  return NumericalPoint(1, -alpha_ * (-alpha_ * expm1(minusAlphalogY) + 1.0) * std::exp(-std::exp(minusAlphalogY) + minusAlphalogY) / (x * x));
}

/* Get the PDF of the distribution */
NumericalScalar Frechet::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return 0.0;
  return std::exp(-std::pow(x / beta_, -alpha_));
}

/* Get the PDF of the distribution */
NumericalScalar Frechet::computePDF(const NumericalPoint & point) const
{
  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar Frechet::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return -SpecFunc::MaxNumericalScalar;
  return std::log(alpha_ / beta_) + (-1.0 - alpha_) * std::log(x / beta_) - std::pow(x / beta_, -alpha_);
}

/* Parameters value and description accessor */
NumericalPoint Frechet::getParameter() const
{
  NumericalPoint point(3);
  point[0] = alpha_;
  point[1] = beta_;
  point[2] = gamma_;
  return point;
}

void Frechet::setParameter(const NumericalPoint & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize(); 
  const NumericalScalar w = getWeight();
  *this = Frechet(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description Frechet::getParameterDescription() const
{
  Description description(3);
  description[0] = "alpha";
  description[1] = "beta";
  description[2] = "gamma";
  return description;
}

/* Get the PDFGradient of the distribution */
NumericalPoint Frechet::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  NumericalPoint pdfGradient(3);
  if (x <= gamma_) return pdfGradient;
  const NumericalScalar logCdf = -std::pow((x - gamma_) / beta_, -alpha_);
  const NumericalScalar logCdfm1 = std::pow((x - gamma_) / beta_, -alpha_ - 1.0);
  const NumericalScalar cdf = std::exp(logCdf);
  pdfGradient[0] = -alpha_ * logCdfm1 * cdf * std::log((x - gamma_) / beta_) / beta_ - alpha_ * logCdf * logCdfm1 * cdf * std::log((x - gamma_) / beta_) / beta_ + logCdfm1 * cdf / beta_;
  pdfGradient[1] = alpha_ * alpha_ * logCdf * logCdfm1 * cdf / (beta_ * beta_) - alpha_ * logCdfm1 * (-alpha_ - 1.0) * cdf / (beta_ * beta_) - alpha_ * logCdfm1 * cdf / (beta_ * beta_);
  pdfGradient[2] = alpha_ * alpha_ * logCdf * logCdfm1 * cdf / (beta_ * (x - gamma_)) - alpha_ * logCdfm1 * (-alpha_ - 1.0) * cdf / (beta_ * (x - gamma_));
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
NumericalPoint Frechet::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0];
  NumericalPoint cdfGradient(3);
  if (x <= gamma_) return cdfGradient;
  const NumericalScalar logCdf = -std::pow((x - gamma_) / beta_, -alpha_);
  const NumericalScalar cdf = std::exp(logCdf);
  cdfGradient[0] = -logCdf * cdf * std::log((x - gamma_) / beta_);
  cdfGradient[1] = alpha_ * logCdf * cdf / beta_;
  cdfGradient[2] = alpha_ * logCdf * cdf / (x - gamma_);
  return cdfGradient;
}

/* Compute the mean of the distribution */
void Frechet::computeMean() const
{
  if (alpha_ <= 1.0) throw InvalidArgumentException(HERE) << "mean is not defined for alpha <= 1";

  mean_ = NumericalPoint(1, gamma_ + beta_ * SpecFunc::Gamma(1.0 - 1.0 / alpha_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Frechet::getStandardDeviation() const
{
  return NumericalPoint(1, std::sqrt(getCovariance()(0, 0)));
}

/* Compute the covariance of the distribution */
void Frechet::computeCovariance() const
{
  if (alpha_ <= 2.0) throw InvalidArgumentException(HERE) << "covariance is not defined for alpha <= 2";

  covariance_ = CovarianceMatrix(1);
  const NumericalScalar gammaInvAlpha(SpecFunc::Gamma(1.0 - 1.0 / alpha_));
  covariance_(0, 0) = beta_ * beta_ * (SpecFunc::Gamma(1.0 - 2.0 / alpha_) - gammaInvAlpha * gammaInvAlpha);
  isAlreadyComputedCovariance_ = true;
}


/* Get the skewness of the distribution */
NumericalPoint Frechet::getSkewness() const
{
  if (alpha_ <= 3.0) throw InvalidArgumentException(HERE) << "skewness is not defined for alpha <= 3";

  const NumericalScalar gammaInvAlpha(SpecFunc::Gamma(1.0 - 1.0 / alpha_));
  const NumericalScalar gamma2InvAlpha(SpecFunc::Gamma(1.0 - 2.0 / alpha_));
  const NumericalScalar gamma3InvAlpha(SpecFunc::Gamma(1.0 - 3.0 / alpha_));
  const NumericalScalar gammaInvAlpha2(gammaInvAlpha * gammaInvAlpha);
  const NumericalScalar skewness((gamma3InvAlpha + gammaInvAlpha * (2.0 * gammaInvAlpha2 - 3.0 * gamma2InvAlpha)) / std::pow(gamma2InvAlpha - gammaInvAlpha2, 1.5));
  return NumericalPoint(1, skewness);
}

/* Get the kurtosis of the distribution */
NumericalPoint Frechet::getKurtosis() const
{
  if (alpha_ <= 4.0) throw InvalidArgumentException(HERE) << "kurtosis is not defined for alpha <= 4";

  const NumericalScalar gammaInvAlpha(SpecFunc::Gamma(1.0 - 1.0 / alpha_));
  const NumericalScalar gamma2InvAlpha(SpecFunc::Gamma(1.0 - 2.0 / alpha_));
  const NumericalScalar gamma3InvAlpha(SpecFunc::Gamma(1.0 - 3.0 / alpha_));
  const NumericalScalar gamma4InvAlpha(SpecFunc::Gamma(1.0 - 4.0 / alpha_));
  const NumericalScalar gammaInvAlpha2(gammaInvAlpha * gammaInvAlpha);
  const NumericalScalar kurtosis((gamma4InvAlpha + gammaInvAlpha * (-4.0 * gamma3InvAlpha + 3.0 * gammaInvAlpha * (2.0 * gamma2InvAlpha - gammaInvAlpha2))) / std::pow(gamma2InvAlpha - gammaInvAlpha2, 2.0)); 
  return NumericalPoint(1, kurtosis);
}

/* Get the moments of the standardized distribution */
NumericalPoint Frechet::getStandardMoment(const UnsignedInteger n) const
{
  if (n >= alpha_) throw NotDefinedException(HERE) << "Error: cannot compute a standard moment of order greater or equal to alpha=" << alpha_;
  return NumericalPoint(1, SpecFunc::Gamma(1.0 - n / alpha_));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Frechet::Implementation Frechet::getStandardRepresentative() const
{
  return Frechet(alpha_, 1.0, 0.0).clone();
}

/* Alpha accessor */
void Frechet::setAlpha(const NumericalScalar alpha)
{
  if (alpha <= 0.) throw InvalidArgumentException(HERE) << "Error: Frechet alpha parameter must be positive";
  if (alpha != alpha_)
  {
    alpha_ = alpha;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar Frechet::getAlpha() const
{
  return alpha_;
}

/* Beta accessor */
void Frechet::setBeta(const NumericalScalar beta)
{
  if (beta <= 0.) throw InvalidArgumentException(HERE) << "Error: Frechet beta parameter must be positive";
  if (beta != beta_)
  {
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar Frechet::getBeta() const
{
  return beta_;
}

/* Gamma accessor */
void Frechet::setGamma(const NumericalScalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}


NumericalScalar Frechet::getGamma() const
{
  return gamma_;
}


/* Method save() stores the object through the StorageManager */
void Frechet::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

/* Method load() reloads the object from the StorageManager */
void Frechet::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "gamma_", gamma_ );
  setDimension(1);
  computeRange();
}

END_NAMESPACE_OPENTURNS
