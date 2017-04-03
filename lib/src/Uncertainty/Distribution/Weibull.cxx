//                                               -*- C++ -*-
/**
 *  @brief The Weibull distribution
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/Weibull.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Weibull);

static const Factory<Weibull> Factory_Weibull;

/* Default constructor */
Weibull::Weibull()
  : ContinuousDistribution()
  , alpha_(1.0)
  , beta_(1.0)
  , gamma_(0.0)
{
  setName("Weibull");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
Weibull::Weibull(const NumericalScalar alpha,
                 const NumericalScalar beta,
                 const NumericalScalar gamma)
  : ContinuousDistribution()
  , alpha_(0.0)
  , beta_(0.0)
  , gamma_(gamma)
{
  setName("Weibull");
  setAlphaBeta(alpha, beta);
  setDimension(1);
}

/* Comparison operator */
Bool Weibull::operator ==(const Weibull & other) const
{
  if (this == &other) return true;
  return (alpha_ == other.alpha_) && (beta_ == other.beta_) && (gamma_ == other.gamma_);
}

Bool Weibull::equals(const DistributionImplementation & other) const
{
  const Weibull* p_other = dynamic_cast<const Weibull*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String Weibull::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Weibull::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " gamma=" << gamma_;
  return oss;
}

String Weibull::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(alpha = " << alpha_ << ", beta = " << beta_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* Virtual constructor */
Weibull * Weibull::clone() const
{
  return new Weibull(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void Weibull::computeRange()
{
  const Point lowerBound(1, gamma_);
  const Point upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}


/* Get one realization of the distribution */
Point Weibull::getRealization() const
{
  return Point(1, gamma_ + alpha_ * std::pow(-std::log(1.0 - RandomGenerator::Generate()), 1.0 / beta_));
}


/* Get the DDF of the distribution */
Point Weibull::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return Point(1, 0.0);
  const NumericalScalar powX = std::pow(x / alpha_, beta_);
  return Point(1, (beta_ * (1.0 - powX) - 1.0) / (x * x) * beta_ * powX * std::exp(-powX));
}


/* Get the PDF of the distribution */
NumericalScalar Weibull::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar Weibull::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return SpecFunc::LogMinScalar;
  const NumericalScalar y = x / alpha_;
  return std::log(beta_) + (beta_ - 1.0) * std::log(y) - std::log(alpha_) - std::pow(y, beta_);
}


/* Get the CDF of the distribution */
NumericalScalar Weibull::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return 0.0;
  return -expm1(-std::pow(x / alpha_, beta_));
}

NumericalScalar Weibull::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  if (x <= 0.0) return 1.0;
  return std::exp(-std::pow(x / alpha_, beta_));
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
NumericalComplex Weibull::computeCharacteristicFunction(const NumericalScalar x) const
{
  /*
  X=mu+sigma*Y;
  phi_X(u)=E(exp(I*u*(mu+sigma*Y)))=exp(I*u*mu)*E(I*u*sigma*Y)=exp(I*u*mu)*phi_Y(sigma*u)
  phi_Y(u)=1+(\sum_{r=1}^{\infty}(iu)^r\frac{\Gamma(r/\lambda)}{\Gamma(r)}
  */
  if (x == 0.0) return 1.0;
  // Special case: beta == 1 -> exponential distribution
  if (beta_ == 1.0) return 1.0 / NumericalComplex(1.0, -x / alpha_);
  // If beta < 1.0, the series based on the Gamma function is divergente so use the generic implementation
  if (beta_ < 1.0) return DistributionImplementation::computeCharacteristicFunction(x);
  NumericalComplex value(1.0);
  const NumericalScalar u = x * alpha_;
  const NumericalScalar sign = x < 0.0 ? -1.0 : 1.0;
  const NumericalScalar logAbsU = std::log(std::abs(u));
  NumericalScalar oldNorm = 0.0;
  NumericalScalar norm = oldNorm;
  UnsignedInteger r = 1;
  Bool increasing = true;
  while (increasing || (norm > std::abs(value) * SpecFunc::ScalarEpsilon))
  {
    const NumericalScalar term1 = std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / beta_));
    ++r;
    const NumericalScalar term2 = std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / beta_));
    ++r;
    const NumericalScalar term3 = std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / beta_));
    ++r;
    const NumericalScalar term4 = std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / beta_));
    ++r;
    const NumericalComplex term((term4 - term2) / beta_, sign * (term1 - term3) / beta_);
    oldNorm = norm;
    norm = std::abs(term);
    // If the term grows too much, the cancelation will be too large
    if (norm > 1e3) return DistributionImplementation::computeCharacteristicFunction(x);
    value += term;
    increasing = norm > oldNorm;
  }
  value *= std::exp(NumericalComplex(0.0, x * gamma_));
  return value;
}

/* Get the PDFGradient of the distribution */
Point Weibull::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  Point pdfGradient(3, 0.0);
  if (x <= 0.0) return pdfGradient;
  const NumericalScalar powX = std::pow(x / alpha_, beta_);
  const NumericalScalar factor = powX / x * std::exp(-powX);
  pdfGradient[0] = factor * (powX - 1.0) * beta_ * beta_ / alpha_;
  pdfGradient[1] = factor * (1.0 + (1.0 - powX) * std::log(powX));
  pdfGradient[2] = factor * (1.0 - beta_ + beta_ * powX) / x * beta_;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point Weibull::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x = point[0] - gamma_;
  Point cdfGradient(3, 0.0);
  if (x <= 0.0) return cdfGradient;
  const NumericalScalar powX = std::pow(x / alpha_, beta_);
  const NumericalScalar factor = powX * std::exp(-powX);
  cdfGradient[0] = -factor * beta_ / alpha_;
  cdfGradient[1] = factor * std::log(x / alpha_);
  cdfGradient[2] = -factor * beta_ / x;
  return cdfGradient;
}

/* Get the quantile of the distribution */
NumericalScalar Weibull::computeScalarQuantile(const NumericalScalar prob,
    const Bool tail) const
{
  if (tail) return gamma_ + alpha_ * std::pow(-std::log(prob), 1.0 / beta_);
  return gamma_ + alpha_ * std::pow(-std::log(1.0 - prob), 1.0 / beta_);
}

/* compute the mean of the distribution */
void Weibull::computeMean() const
{
  mean_ =  Point(1, gamma_ + alpha_ * SpecFunc::Gamma(1.0 + 1.0 / beta_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point Weibull::getStandardDeviation() const
{
  return Point(1, alpha_ * std::sqrt(SpecFunc::Gamma(1.0 + 2.0 / beta_) - std::pow(SpecFunc::Gamma(1.0 + 1.0 / beta_), 2.0)));
}

/* Get the skewness of the distribution */
Point Weibull::getSkewness() const
{
  const NumericalScalar gamma1 = SpecFunc::Gamma(1.0 + 1.0 / beta_);
  const NumericalScalar gamma1_2 = gamma1 * gamma1;
  const NumericalScalar gamma2 = SpecFunc::Gamma(1.0 + 2.0 / beta_);
  const NumericalScalar gamma3 = SpecFunc::Gamma(1.0 + 3.0 / beta_);
  return Point(1, (2.0 * gamma1_2 * gamma1 - 3.0 * gamma1 * gamma2 + gamma3) / std::pow((gamma2 - gamma1_2), 1.5));
}

/* Get the kurtosis of the distribution */
Point Weibull::getKurtosis() const
{
  const NumericalScalar gamma1 = SpecFunc::Gamma(1.0 + 1.0 / beta_);
  const NumericalScalar gamma1_2 = gamma1 * gamma1;
  const NumericalScalar gamma2 = SpecFunc::Gamma(1.0 + 2.0 / beta_);
  const NumericalScalar gamma3 = SpecFunc::Gamma(1.0 + 3.0 / beta_);
  const NumericalScalar gamma4 = SpecFunc::Gamma(1.0 + 4.0 / beta_);
  return Point(1, (6.0 * gamma1_2 * gamma2 + gamma4 - 4.0 * gamma1 * gamma3 - 3.0 * gamma1_2 * gamma1_2) / std::pow(gamma2 - gamma1_2, 2.0));
}

/* Compute the covariance of the distribution */
void Weibull::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = std::pow(getStandardDeviation()[0], 2.0);
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
Point Weibull::getStandardMoment(const UnsignedInteger n) const
{
  return Point(1, SpecFunc::Gamma(1.0 + n / beta_));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Weibull::Implementation Weibull::getStandardRepresentative() const
{
  return Weibull(1.0, beta_, 0.0).clone();
}

/* Parameters value accessor */
Point Weibull::getParameter() const
{
  Point point(3);
  point[0] = alpha_;
  point[1] = beta_;
  point[2] = gamma_;
  return point;
}

void Weibull::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const NumericalScalar w = getWeight();
  *this = Weibull(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description Weibull::getParameterDescription() const
{
  Description description(3);
  description[0] = "alpha";
  description[1] = "beta";
  description[2] = "gamma";
  return description;
}

/* Beta accessor */
void Weibull::setBeta(const NumericalScalar beta)
{
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";
  if (beta != beta_)
  {
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar Weibull::getBeta() const
{
  return beta_;
}


/* Alpha accessor */
void Weibull::setAlpha(const NumericalScalar alpha)
{
  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (alpha != alpha_)
  {
    alpha_ = alpha;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

NumericalScalar Weibull::getAlpha() const
{
  return alpha_;
}

/* Alpha Beta accessor */
void Weibull::setAlphaBeta(const NumericalScalar alpha,
                           const NumericalScalar beta)
{
  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";
  if ((alpha != alpha_) || (beta != beta_))
  {
    alpha_ = alpha;
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}


/* Gamma accessor */
void Weibull::setGamma(const NumericalScalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on gamma
    computeRange();
  }
}

NumericalScalar Weibull::getGamma() const
{
  return gamma_;
}


/* Method save() stores the object through the StorageManager */
void Weibull::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

/* Method load() reloads the object from the StorageManager */
void Weibull::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "gamma_", gamma_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
