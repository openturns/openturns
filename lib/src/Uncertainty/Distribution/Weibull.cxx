//                                               -*- C++ -*-
/**
 *  @brief The Weibull distribution
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
Weibull::Weibull(const NumericalScalar arg1,
                 const NumericalScalar arg2,
                 const NumericalScalar gamma,
                 const ParameterSet set)
  : ContinuousDistribution()
  , alpha_(0.0)
  , beta_(0.0)
  , gamma_(gamma)
{
  setName("Weibull");
  switch (set)
  {
    case ALPHABETA:
      // This call set also the range
      setAlphaBeta(arg1, arg2);
      break;

    case MUSIGMA:
      // This call set also the range
      setMuSigma(arg1, arg2);
      break;

    default:
      throw InvalidArgumentException(HERE) << "Invalid parameter set argument";

  } /* end switch */

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
  const NumericalPoint lowerBound(1, gamma_);
  const NumericalPoint upperBound(computeUpperBound());
  const Interval::BoolCollection finiteLowerBound(1, true);
  const Interval::BoolCollection finiteUpperBound(1, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}


/* Get one realization of the distribution */
NumericalPoint Weibull::getRealization() const
{
  return NumericalPoint(1, gamma_ + alpha_ * std::pow(-std::log(1.0 - RandomGenerator::Generate()), 1.0 / beta_));
}


/* Get the DDF of the distribution */
NumericalPoint Weibull::computeDDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - gamma_);
  if (x <= 0.0) return NumericalPoint(1, 0.0);
  const NumericalScalar powX(std::pow(x / alpha_, beta_));
  return NumericalPoint(1, (beta_ * (1.0 - powX) - 1.0) / (x * x) * beta_ * powX * std::exp(-powX));
}


/* Get the PDF of the distribution */
NumericalScalar Weibull::computePDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - gamma_);
  if (x <= 0.0) return 0.0;
  return std::exp(computeLogPDF(point));
}

NumericalScalar Weibull::computeLogPDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - gamma_);
  if (x <= 0.0) return -SpecFunc::MaxNumericalScalar;
  const NumericalScalar y(x / alpha_);
  return std::log(beta_) + (beta_ - 1.0) * std::log(y) - std::log(alpha_) - std::pow(y, beta_);
}


/* Get the CDF of the distribution */
NumericalScalar Weibull::computeCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - gamma_);
  if (x <= 0.0) return 0.0;
  return -expm1(-std::pow(x / alpha_, beta_));
}

NumericalScalar Weibull::computeComplementaryCDF(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - gamma_);
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
  const NumericalScalar u(x * alpha_);
  const NumericalScalar sign(x < 0.0 ? -1.0 : 1.0);
  const NumericalScalar logAbsU(std::log(std::abs(u)));
  NumericalScalar oldNorm(0.0);
  NumericalScalar norm(oldNorm);
  UnsignedInteger r(1);
  Bool increasing(true);
  while (increasing || (norm > std::abs(value) * SpecFunc::NumericalScalarEpsilon))
  {
    const NumericalScalar term1(std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / beta_)));
    ++r;
    const NumericalScalar term2(std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / beta_)));
    ++r;
    const NumericalScalar term3(std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / beta_)));
    ++r;
    const NumericalScalar term4(std::exp(r * logAbsU - SpecFunc::LogGamma(r) + SpecFunc::LogGamma(r / beta_)));
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
NumericalPoint Weibull::computePDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - gamma_);
  NumericalPoint pdfGradient(3, 0.0);
  if (x <= 0.0) return pdfGradient;
  const NumericalScalar powX(std::pow(x / alpha_, beta_));
  const NumericalScalar factor(powX / x * std::exp(-powX));
  pdfGradient[0] = factor * (powX - 1.0) * beta_ * beta_ / alpha_;
  pdfGradient[1] = factor * (1.0 + (1.0 - powX) * std::log(powX));
  pdfGradient[2] = factor * (1.0 - beta_ + beta_ * powX) / x * beta_;
  return pdfGradient;
}

/* Get the CDFGradient of the distribution */
NumericalPoint Weibull::computeCDFGradient(const NumericalPoint & point) const
{
  if (point.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=1, here dimension=" << point.getDimension();

  const NumericalScalar x(point[0] - gamma_);
  NumericalPoint cdfGradient(3, 0.0);
  if (x <= 0.0) return cdfGradient;
  const NumericalScalar powX(std::pow(x / alpha_, beta_));
  const NumericalScalar factor(powX * std::exp(-powX));
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
  mean_ =  NumericalPoint(1, getMu());
  isAlreadyComputedMean_ = true;
}

/* Get the standard deviation of the distribution */
NumericalPoint Weibull::getStandardDeviation() const
{
  return NumericalPoint(1, getSigma());
}

/* Get the skewness of the distribution */
NumericalPoint Weibull::getSkewness() const
{
  const NumericalScalar gamma1(SpecFunc::Gamma(1.0 + 1.0 / beta_));
  const NumericalScalar gamma1_2(gamma1 * gamma1);
  const NumericalScalar gamma2(SpecFunc::Gamma(1.0 + 2.0 / beta_));
  const NumericalScalar gamma3(SpecFunc::Gamma(1.0 + 3.0 / beta_));
  return NumericalPoint(1, (2.0 * gamma1_2 * gamma1 - 3.0 * gamma1 * gamma2 + gamma3) / std::pow((gamma2 - gamma1_2), 1.5));
}

/* Get the kurtosis of the distribution */
NumericalPoint Weibull::getKurtosis() const
{
  const NumericalScalar gamma1(SpecFunc::Gamma(1.0 + 1.0 / beta_));
  const NumericalScalar gamma1_2(gamma1 * gamma1);
  const NumericalScalar gamma2(SpecFunc::Gamma(1.0 + 2.0 / beta_));
  const NumericalScalar gamma3(SpecFunc::Gamma(1.0 + 3.0 / beta_));
  const NumericalScalar gamma4(SpecFunc::Gamma(1.0 + 4.0 / beta_));
  return NumericalPoint(1, (6.0 * gamma1_2 * gamma2 + gamma4 - 4.0 * gamma1 * gamma3 - 3.0 * gamma1_2 * gamma1_2) / std::pow(gamma2 - gamma1_2, 2.0));
}

/* Compute the covariance of the distribution */
void Weibull::computeCovariance() const
{
  covariance_ = CovarianceMatrix(1);
  covariance_(0, 0) = std::pow(getSigma(), 2.0);
  isAlreadyComputedCovariance_ = true;
}

/* Get the moments of the standardized distribution */
NumericalPoint Weibull::getStandardMoment(const UnsignedInteger n) const
{
  return NumericalPoint(1, SpecFunc::Gamma(1.0 + n / beta_));
}

/* Get the standard representative in the parametric family, associated with the standard moments */
Weibull::Implementation Weibull::getStandardRepresentative() const
{
  return Weibull(1.0, beta_, 0.0).clone();
}

/* Parameters value accessor */
NumericalPoint Weibull::getParameter() const
{
  NumericalPoint point(3);
  point[0] = alpha_;
  point[1] = beta_;
  point[2] = gamma_;
  return point;
}

void Weibull::setParameter(const NumericalPoint & parameter)
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
  if (beta <= 0.0) throw InvalidArgumentException(HERE) << "Beta MUST be positive";
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
  if (alpha <= 0.0) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
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
  if (alpha <= 0.0) throw InvalidArgumentException(HERE) << "Alpha MUST be positive";
  if (beta <= 0.0) throw InvalidArgumentException(HERE) << "Beta MUST be positive";
  if ((alpha != alpha_) || (beta != beta_))
  {
    alpha_ = alpha;
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}


/* MuSigma accessor */
/* To compute (alpha, beta) from (mu, sigma), we proceed as follows:
   1 + var_ / (mu_ - gamma_)^2 = GAMMA(1 + 2 / beta_) / GAMMA(1 + 1 / beta_)^2
   = t(beta_)
   This function is strictly decreasing and takes arbitrary large values near 0.
   We solve the equation t(beta) = 1 + var_ / (mu_ - gamma_)^2 using Bisection's method, as it is very robust and we don't really have a speed constraint here. Then, we find alpha_ thanks to:
   alpha_ = (mu_ - gamma_) / GAMMA(1 + 1 / beta_)
*/
void Weibull::setMuSigma(const NumericalScalar mu,
                         const NumericalScalar sigma)
{
  if (mu <= gamma_) throw InvalidArgumentException(HERE) << "Mu MUST be > gamma, here mu=" << mu << " and gamma=" << gamma_;
  if (sigma <= 0.0) throw InvalidArgumentException(HERE) << "Sigma MUST be > 0.0, here sigma=" << sigma;
  const NumericalScalar ratio(1.0 + std::pow(sigma / (mu - gamma_), 2.0));
  NumericalScalar t;
  NumericalScalar betaMin(1.0);
  NumericalScalar betaMax(1.0);
  NumericalScalar step(0.5);
  // Bracketing interval
  // Case beta < 1, i.e. ratio > 2
  if (ratio > 2)
  {
    do
    {
      betaMin -= step;
      step *= 0.5;
      t = std::exp(SpecFunc::LnGamma(1.0 + 2.0 / betaMin) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / betaMin));
    }
    while (t < ratio);
    // Here, we know that betaMin <= beta < betaMin + 2.0 * step
    betaMax = betaMin + 2.0 * step;
  }
  // Case beta >= 1, i.e. ratio <= 2
  else
  {
    do
    {
      betaMax += step;
      step *= 2.0;
      t = std::exp(SpecFunc::LnGamma(1.0 + 2.0 / betaMax) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / betaMax));
    }
    while (t >= ratio);
    // Here, we know that betaMax - 0.5 * step <= beta < betaMax
    betaMin = betaMax - 0.5 * step;
  }
  // Bisection loop
  const NumericalScalar epsilon(ResourceMap::GetAsNumericalScalar("Distribution-DefaultQuantileEpsilon"));
  for (;;)
  {
    beta_ = 0.5 * (betaMin + betaMax);
    // Convergence
    if (betaMax - betaMin <= epsilon * (1.0 + std::abs(betaMax + betaMin)))
    {
      alpha_ = (mu - gamma_) / SpecFunc::Gamma(1.0 + 1.0 / beta_);
      break;
    }
    // Non convergence, one step further
    t = std::exp(SpecFunc::LnGamma(1.0 + 2.0 / beta_) - 2.0 * SpecFunc::LnGamma(1.0 + 1.0 / beta_));
    if (t < ratio) betaMax = beta_;
    else betaMin = beta_;
  }
  isAlreadyComputedMean_ = false;
  isAlreadyComputedCovariance_ = false;
  computeRange();
}

NumericalScalar Weibull::getMu() const
{
  return gamma_ + alpha_ * SpecFunc::Gamma(1.0 + 1.0 / beta_);
}


NumericalScalar Weibull::getSigma() const
{
  return alpha_ * std::sqrt(SpecFunc::Gamma(1.0 + 2.0 / beta_) - std::pow(SpecFunc::Gamma(1.0 + 1.0 / beta_), 2.0));
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
