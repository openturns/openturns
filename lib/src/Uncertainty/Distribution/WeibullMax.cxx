//                                               -*- C++ -*-
/**
 *  @brief The WeibullMax distribution
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/WeibullMax.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WeibullMax)

static const Factory<WeibullMax> Factory_WeibullMax;

/* Default constructor */
WeibullMax::WeibullMax()
  : ContinuousDistribution()
  , beta_(1.0)
  , alpha_(1.0)
  , gamma_(0.0)
{
  setName("WeibullMax");
  setDimension(1);
  computeRange();
}

/* Parameters constructor */
WeibullMax::WeibullMax(const Scalar beta,
                       const Scalar alpha,
                       const Scalar gamma)
  : ContinuousDistribution()
  , beta_(0.0)
  , alpha_(0.0)
  , gamma_(gamma)
{
  setName("WeibullMax");
  setAlphaBeta(alpha, beta);
  setDimension(1);
}

/* Comparison operator */
Bool WeibullMax::operator ==(const WeibullMax & other) const
{
  if (this == &other) return true;
  return (beta_ == other.beta_) && (alpha_ == other.alpha_) && (gamma_ == other.gamma_);
}

Bool WeibullMax::equals(const DistributionImplementation & other) const
{
  const WeibullMax* p_other = dynamic_cast<const WeibullMax*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String WeibullMax::__repr__() const
{
  OSS oss(true);
  oss << "class=" << WeibullMax::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " beta=" << beta_
      << " alpha=" << alpha_
      << " gamma=" << gamma_;
  return oss;
}

String WeibullMax::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(beta = " << beta_ << ", alpha = " << alpha_ << ", gamma = " << gamma_ << ")";
  return oss;
}

/* Virtual constructor */
WeibullMax * WeibullMax::clone() const
{
  return new WeibullMax(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void WeibullMax::computeRange()
{
  const Point lowerBound(computeLowerBound());
  const Point upperBound(1, gamma_);
  const Interval::BoolCollection finiteLowerBound(1, false);
  const Interval::BoolCollection finiteUpperBound(1, true);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}


/* Get one realization of the distribution */
Point WeibullMax::getRealization() const
{
  return Point(1, gamma_ - beta_ * std::pow(-std::log(1.0 - RandomGenerator::Generate()), 1.0 / alpha_));
}


/* Get the DDF of the distribution */
Point WeibullMax::computeDDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x >= 0.0) return Point(1, 0.0);
  const Scalar powX = std::pow(-x / beta_, alpha_);
  return Point(1, -(alpha_ * (1.0 - powX) - 1.0) / (x * x) * alpha_ * powX * std::exp(-powX));
}


/* Get the PDF of the distribution */
Scalar WeibullMax::computePDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x >= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar WeibullMax::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x >= 0.0) return SpecFunc::LowestScalar;
  const Scalar y = -x / beta_;
  return std::log(alpha_) + (alpha_ - 1.0) * std::log(y) - std::log(beta_) - std::pow(y, alpha_);
}


/* Get the CDF of the distribution */
Scalar WeibullMax::computeCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x >= 0.0) return 1.0;
  return std::exp(-std::pow(-x / beta_, alpha_));
}

Scalar WeibullMax::computeComplementaryCDF(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  if (x >= 0.0) return 0.0;
  return -expm1(-std::pow(-x / beta_, alpha_));
}

/* Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
Complex WeibullMax::computeCharacteristicFunction(const Scalar x) const
{
  /*
  X=mu+sigma*Y;
  phi_X(u)=E(exp(I*u*(mu+sigma*Y)))=exp(I*u*mu)*E(I*u*sigma*Y)=exp(I*u*mu)*phi_Y(sigma*u)
  phi_Y(u)=1+(\sum_{r=1}^{\infty}(iu)^r\frac{\Gamma(r/\lambda)}{\Gamma(r)}
  */
  if (x == 0.0) return 1.0;
  // Special case: beta == 1 -> exponential distribution
  if (alpha_ == 1.0) return 1.0 / Complex(1.0, -x / beta_);
  // If beta < 1.0, the series based on the Gamma function is divergente so use the generic implementation
  if (alpha_ < 1.0) return DistributionImplementation::computeCharacteristicFunction(x);
  Complex value(1.0);
  const Scalar u = x * beta_;
  const Scalar sign = x < 0.0 ? -1.0 : 1.0;
  const Scalar logAbsU = std::log(std::abs(u));
  Scalar oldNorm = 0.0;
  Scalar norm = oldNorm;
  UnsignedInteger r = 1;
  Bool increasing = true;
  while (increasing || (norm > std::abs(value) * SpecFunc::ScalarEpsilon))
  {
    const Scalar term1 = std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / alpha_));
    ++r;
    const Scalar term2 = std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / alpha_));
    ++r;
    const Scalar term3 = std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / alpha_));
    ++r;
    const Scalar term4 = std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / alpha_));
    ++r;
    const Complex term((term4 - term2) / alpha_, sign * (term1 - term3) / alpha_);
    oldNorm = norm;
    norm = std::abs(term);
    // If the term grows too much, the cancelation will be too large
    if (norm > 1e3) return DistributionImplementation::computeCharacteristicFunction(x);
    value += term;
    increasing = norm > oldNorm;
  }
  value *= std::exp(Complex(0.0, x * gamma_));
  return value;
}

/* Compute the entropy of the distribution */
Scalar WeibullMax::computeEntropy() const
{
  return 1.0 + SpecFunc::EulerConstant * (1.0 - 1.0 / alpha_) + std::log(beta_ / alpha_);
}

/* Get the PDFGradient of the distribution */
Point WeibullMax::computePDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  Point pdfGradient(3, 0.0);
  if (x >= 0.0) return pdfGradient;
  const Scalar powX = std::pow(-x / beta_, alpha_);
  const Scalar factor = -powX / x * std::exp(-powX);
  pdfGradient[0] = factor * (powX - 1.0) * alpha_ * alpha_ / beta_;
  pdfGradient[1] = factor * (1.0 + (1.0 - powX) * std::log(powX));
  pdfGradient[2] = factor * (1.0 - alpha_ + alpha_ * powX) / x * alpha_;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
Point WeibullMax::computeCDFGradient(const Point & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const Scalar x = point[0] - gamma_;
  Point cdfGradient(3, 0.0);
  if (x >= 0.0) return cdfGradient;
  const Scalar powX = std::pow(-x / beta_, alpha_);
  const Scalar factor = powX * std::exp(-powX);
  cdfGradient[0] = factor * alpha_ / beta_;
  cdfGradient[1] = -factor * std::log(-x / beta_);
  cdfGradient[2] = factor * alpha_ / x;
  return cdfGradient;
}

/* Get the quantile of the distribution */
Scalar WeibullMax::computeScalarQuantile(const Scalar prob,
    const Bool tail) const
{
  if (tail) return gamma_ - beta_ * std::pow(-std::log(1.0 - prob), 1.0 / alpha_);
  return gamma_ - beta_ * std::pow(-std::log(prob), 1.0 / alpha_);
}

/* compute the mean of the distribution */
void WeibullMax::computeMean() const
{
  mean_ =  Point(1, gamma_ - beta_ * SpecFunc::Gamma(1.0 + 1.0 / alpha_));
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
Point WeibullMax::getStandardDeviation() const
{
  return Point(1, beta_ * std::sqrt(SpecFunc::Gamma(1.0 + 2.0 / alpha_) - std::pow(SpecFunc::Gamma(1.0 + 1.0 / alpha_), 2.0)));
}

/* Get the skewness of the distribution */
Point WeibullMax::getSkewness() const
{
  const Scalar gamma1 = SpecFunc::Gamma(1.0 + 1.0 / alpha_);
  const Scalar gamma1_2 = gamma1 * gamma1;
  const Scalar gamma2 = SpecFunc::Gamma(1.0 + 2.0 / alpha_);
  const Scalar gamma3 = SpecFunc::Gamma(1.0 + 3.0 / alpha_);
  return Point(1, -(2.0 * gamma1_2 * gamma1 - 3.0 * gamma1 * gamma2 + gamma3) / std::pow((gamma2 - gamma1_2), 1.5));
}

/* Get the kurtosis of the distribution */
Point WeibullMax::getKurtosis() const
{
  const Scalar gamma1 = SpecFunc::Gamma(1.0 + 1.0 / alpha_);
  const Scalar gamma1_2 = gamma1 * gamma1;
  const Scalar gamma2 = SpecFunc::Gamma(1.0 + 2.0 / alpha_);
  const Scalar gamma3 = SpecFunc::Gamma(1.0 + 3.0 / alpha_);
  const Scalar gamma4 = SpecFunc::Gamma(1.0 + 4.0 / alpha_);
  return Point(1, (6.0 * gamma1_2 * gamma2 + gamma4 - 4.0 * gamma1 * gamma3 - 3.0 * gamma1_2 * gamma1_2) / std::pow(gamma2 - gamma1_2, 2.0));
}

/* Compute the covariance of the distribution */
void WeibullMax::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = std::pow(getStandardDeviation()[0], 2.0);
  isAlreadyComputedCovariance_ = true;
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Distribution WeibullMax::getStandardRepresentative() const
{
  return new WeibullMax(1.0, alpha_, 0.0);
}

/* Parameters value accessor */
Point WeibullMax::getParameter() const
{
  Point point(3);
  point[0] = beta_;
  point[1] = alpha_;
  point[2] = gamma_;
  return point;
}

void WeibullMax::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 3) throw InvalidArgumentException(HERE) << "Error: expected 3 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = WeibullMax(parameter[0], parameter[1], parameter[2]);
  setWeight(w);
}

/* Parameters description accessor */
Description WeibullMax::getParameterDescription() const
{
  Description description(3);
  description[0] = "beta";
  description[1] = "alpha";
  description[2] = "gamma";
  return description;
}

/* Beta accessor */
void WeibullMax::setBeta(const Scalar beta)
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

Scalar WeibullMax::getBeta() const
{
  return beta_;
}


/* Alpha accessor */
void WeibullMax::setAlpha(const Scalar alpha)
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

Scalar WeibullMax::getAlpha() const
{
  return alpha_;
}

/* Alpha Beta accessor */
void WeibullMax::setAlphaBeta(const Scalar alpha,
                              const Scalar beta)
{
  if (!(alpha > 0.0)) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (!(beta > 0.0)) throw InvalidArgumentException(HERE) << "Beta MUST be positive";
  if ((alpha != alpha_) || (beta != beta_))
  {
    beta_ = beta;
    alpha_ = alpha;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}


/* Gamma accessor */
void WeibullMax::setGamma(const Scalar gamma)
{
  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    // The covariance does not depend on gamma
    computeRange();
  }
}

Scalar WeibullMax::getGamma() const
{
  return gamma_;
}


/* Method save() stores the object through the StorageManager */
void WeibullMax::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "beta_", beta_ );
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "gamma_", gamma_ );
}

/* Method load() reloads the object from the StorageManager */
void WeibullMax::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "beta_", beta_ );
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "gamma_", gamma_ );
  computeRange();
}

END_NAMESPACE_OPENTURNS
