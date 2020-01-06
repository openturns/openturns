//                                               -*- C++ -*-
/**
 *  @brief The NormalGamma distribution
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
#include "openturns/NormalGamma.hxx"
#include "openturns/Gamma.hxx"
#include "openturns/Normal.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/PlatformInfo.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NormalGamma)

static const Factory<NormalGamma> Factory_NormalGamma;


/* Default constructor */
NormalGamma::NormalGamma()
  : BayesDistribution()
  , mu_(0.0)
  , kappa_(1.0)
  , alpha_(1.0)
  , beta_(1.0)
  , logNormalization_(0.0)
{
  Point parameter(4);
  parameter[0] = mu_;
  parameter[1] = kappa_;
  parameter[2] = alpha_;
  parameter[3] = beta_;
  setParameter(parameter);
}

/* Parameters constructor */
NormalGamma::NormalGamma(const Scalar mu,
                         const Scalar kappa,
                         const Scalar alpha,
                         const Scalar beta)
  : BayesDistribution()
  , mu_(mu)
  , kappa_(kappa)
  , alpha_(alpha)
  , beta_(beta)
  , logNormalization_(0.0)
{
  if (!(kappa_ > 0.0)) throw InvalidArgumentException(HERE) << "Error: kappa must be positive, here kappa=" << kappa;
  if (!(alpha_ > 0.0)) throw InvalidArgumentException(HERE) << "Error: alpha must be positive, here alpha=" << alpha;
  if (!(beta_ > 0.0)) throw InvalidArgumentException(HERE) << "Error: beta must be positive, here beta=" << beta;
  computeLogNormalization();
  // If we want to have the same behavior of the setParameter() method in NormalGamma and BayesDistribution
  // the values (mu, kappa) must be part of the parameter. As they are not parameter of the conditioning
  // distribution they have to be parameter of the link function
  setName("NormalGamma");
  Description inVars(3);
  inVars[0] = "y";
  inVars[1] = "mu";
  inVars[2] = "kappa";
  Description formulas(2);
  formulas[0] = "mu";
  formulas[1] = "1.0 / sqrt(kappa * y)";
  Indices indices(2);
  indices[0] = 1;
  indices[1] = 2;
  Point values(2);
  values[0] = mu_;
  values[1] = kappa_;
  const ParametricFunction link(SymbolicFunction(inVars, formulas), indices, values);
  setConditionedAndConditioningDistributionsAndLinkFunction(Normal(), Gamma(alpha_, beta_), link);
  computeRange();
}

/* Compute the numerical range of the distribution given the parameters values */
void NormalGamma::computeRange()
{
  const Interval range0(getMarginal(0).getRange());
  const Interval range1(getMarginal(1).getRange());
  Point lowerBound(2, 0.0);
  lowerBound[0] = range0.getLowerBound()[0];
  lowerBound[1] = range1.getLowerBound()[0];
  Interval::BoolCollection finiteLowerBound(2, false);
  finiteLowerBound[1] = true;
  Point upperBound(2, 0.0);
  upperBound[0] = range0.getUpperBound()[0];
  upperBound[1] = range1.getUpperBound()[0];
  Interval::BoolCollection finiteUpperBound(2, false);
  setRange(Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound));
}

/* Comparison operator */
Bool NormalGamma::operator ==(const NormalGamma & other) const
{
  if (this == &other) return true;
  return (mu_ == other.mu_) && (kappa_ == other.kappa_) && (alpha_ == other.alpha_) && (beta_ == other.beta_);
}

Bool NormalGamma::equals(const DistributionImplementation & other) const
{
  const NormalGamma* p_other = dynamic_cast<const NormalGamma*>(&other);
  return p_other && (*this == *p_other);
}

/* Compute the log-normalization factor of the PDF */
void NormalGamma::computeLogNormalization()
{
  logNormalization_ = alpha_ * std::log(beta_) - SpecFunc::LogGamma(alpha_);
}

/* Compute the mean of the distribution */
void NormalGamma::computeMean() const
{
  mean_ = Point(2);
  mean_[0] = mu_;
  mean_[1] = alpha_ / beta_;
  isAlreadyComputedMean_ = true;
}

/* Compute the covariance of the distribution */
void NormalGamma::computeCovariance() const
{
  if (!(alpha_ > 1.0)) throw NotDefinedException(HERE) << "Error: the covariance is not defined for a NormalGamma distribution with alpha<=1, here alpha=" << alpha_;
  covariance_ = CovarianceMatrix(2);
  covariance_(0, 0) = beta_ / (kappa_ * (alpha_ - 1.0));
  covariance_(1, 1) = alpha_ / (beta_ * beta_);
  isAlreadyComputedCovariance_ = true;
}

/* Get the PDF of the distribution */
Scalar NormalGamma::computePDF(const Point & point) const
{
  if (point.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=2, here dimension=" << point.getDimension();

  const Scalar logPDF = computeLogPDF(point);
  if (logPDF == -SpecFunc::LogMaxScalar) return 0.0;
  return std::exp(logPDF);
}

/* Get the log PDF of the distribution */
Scalar NormalGamma::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=2, here dimension=" << point.getDimension();

  const Scalar y = point[1];
  const Scalar a = getRange().getLowerBound()[1];
  const Scalar b = getRange().getUpperBound()[1];
  if ((y <= a) || (y >= b)) return -SpecFunc::LogMaxScalar;
  const Scalar x = point[0] - mu_;
  return logNormalization_ + (alpha_ - 0.5) * std::log(y) - 0.5 * y * (kappa_ * x * x + 2.0 * beta_) + 0.5 * std::log(kappa_ / (2.0 * M_PI));
}

namespace NormalGammaFunctions
{

class KernelProbability: public EvaluationImplementation
{
public:
  KernelProbability(const Scalar uMin,
                    const Scalar uMax,
                    const Scalar kappa,
                    const Scalar alpha,
                    const Scalar beta,
                    const Scalar logNormalization,
                    const UnsignedInteger flag)
    : EvaluationImplementation()
    , uMin_(uMin)
    , uMax_(uMax)
    , kappa_(kappa)
    , alpha_(alpha)
    , beta_(beta)
    , logNormalization_(logNormalization)
      // flag == 0->int_{-\infty}^{uMax}p(x,y)dx = F(uMax) = k(B - 0)
      // flag == 1->int_{uMin}^{\infty}p(x,y)dx = 1 - F(uMin) = k(1 - A)
      // flag == 2->int_{uMin}^{uMax}p(x,y)dx = F(uMax) - F(uMin) = k(B - A)
    , flag_(flag)
  {
    // Nothing to do
  }

  KernelProbability * clone() const
  {
    return new KernelProbability(*this);
  }

  Point operator() (const Point & point) const
  {
    // logNormalization_ = alpha_ * std::log(beta_) - SpecFunc::LogGamma(alpha_);    const Scalar z = point[0];
    const Scalar & y = point[0];
    const Scalar scale = std::sqrt(0.5 * kappa_ * y);
    const Scalar A = (flag_ == 0 ? 0.0 : SpecFunc::Erf(scale * uMin_));
    const Scalar B = (flag_ == 1 ? 1.0 : SpecFunc::Erf(scale * uMax_));
    const Scalar kernelValue = 0.5 * std::exp(logNormalization_ + (alpha_ - 1.0) * std::log(y) - beta_ * y) * (B - A);
    return Point(1, kernelValue);
  }

  UnsignedInteger getInputDimension() const
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const
  {
    return 1;
  }

  String __repr__() const
  {
    OSS oss(true);
    oss << "class=KernelProbability"
        << " uMin=" << uMin_
        << " uMax=" << uMax_
        << " kappa=" << kappa_
        << " alpha=" << alpha_
        << " logNormalization=" << logNormalization_
        << " flag=" << flag_;
    return oss;
  }

  String __str__(const String & ) const
  {
    OSS oss(false);
    oss << "KernelProbability("
        << "uMin=" << uMin_
        << ", uMax=" << uMax_
        << ", kappa=" << kappa_
        << ", alpha=" << alpha_
        << ", beta=" << beta_
        << ", logNormalization=" << logNormalization_
        << ", flag=" << flag_
        << ")";
    return oss;
  }

private:
  Scalar uMin_;
  Scalar uMax_;
  Scalar kappa_;
  Scalar alpha_;
  Scalar beta_;
  Scalar logNormalization_;
  UnsignedInteger flag_;
}; // KernelProbability

} // namespace NormalGammaFunctions

/* Get the CDF of the distribution */
Scalar NormalGamma::computeCDF(const Point & point) const
{
  if (point.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=2, here dimension=" << point.getDimension();
  const Scalar y = point[1];
  const Scalar a = getRange().getLowerBound()[1];
  const Scalar b = getRange().getUpperBound()[1];
  if (y <= a) return 0.0;
  const Scalar x = point[0] - mu_;
  // DistFunc.qNormal(SpecFunc.ScalarEpsilon)=8.12589...
  const Scalar xBound(8.126 / std::sqrt(kappa_ * y));
  if (x < -xBound) return 0.0;
  if (x >  xBound)
  {
    if (y > b) return 1.0;
    return DistFunc::pGamma(alpha_, beta_ * y);
  }
  // Here the integration wrt x is given in closed form
  const Function integrand(NormalGammaFunctions::KernelProbability(-SpecFunc::MaxScalar, x, kappa_, alpha_, beta_, logNormalization_, 0));
  // Integrate over the interval (-inf, y] of the conditioning Gamma distribution
  const Scalar cdf = GaussKronrod().integrate(integrand, Interval(a, std::min(y, b)))[0];
  return cdf;
}

/* Get the survival function of the distribution */
Scalar NormalGamma::computeSurvivalFunction(const Point & point) const
{
  if (point.getDimension() != 2) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=2, here dimension=" << point.getDimension();
  const Scalar y = point[1];
  const Scalar a = getRange().getLowerBound()[1];
  const Scalar b = getRange().getUpperBound()[1];
  if (y > b) return 0.0;
  const Scalar x = point[0] - mu_;
  // DistFunc.qNormal(SpecFunc.ScalarEpsilon)=8.12589...
  const Scalar xBound(8.126 / std::sqrt(kappa_ * y));
  if (x >  xBound) return 0.0;
  if (x < -xBound)
  {
    if (y <= a) return 1.0;
    return DistFunc::pGamma(alpha_, beta_ * y, true);
  }
  // Here the integration wrt x is given in closed form
  const Function integrand(NormalGammaFunctions::KernelProbability(x, SpecFunc::MaxScalar, kappa_, alpha_, beta_, logNormalization_, 1));
  // Integrate over the interval [y, +inf) of the conditioning Gamma distribution
  const Scalar survival = GaussKronrod().integrate(integrand, Interval(std::max(y, a), b))[0];
  return survival;
}

/* Compute the probability content of an interval */
Scalar NormalGamma::computeProbability(const Interval & interval) const
{
  const UnsignedInteger dimension = getDimension();
  if (interval.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given interval must have dimension=" << dimension << ", here dimension=" << interval.getDimension();

  const Interval reducedInterval(interval.intersect(getRange()));
  // If the interval is empty
  if (reducedInterval.isEmpty()) return 0.0;

  // If the interval is the range
  if (reducedInterval == getRange()) return 1.0;

  const Scalar uMin = reducedInterval.getLowerBound()[0];
  const Scalar uMax = reducedInterval.getUpperBound()[0];
  const Scalar a = reducedInterval.getLowerBound()[1];
  const Scalar b = reducedInterval.getUpperBound()[1];
  // Here the integration wrt x is given in closed form
  const Function integrand(NormalGammaFunctions::KernelProbability(uMin, uMax, kappa_, alpha_, beta_, logNormalization_, 2));
  // Integrate over the interval [a, b] of the conditioning Gamma distribution
  const Scalar probability = GaussKronrod().integrate(integrand, Interval(a, b))[0];
  return probability;
}

/* Compute the entropy of the distribution */
Scalar NormalGamma::computeEntropy() const
{
  return alpha_ + (0.5 - alpha_) * SpecFunc::Psi(alpha_) + SpecFunc::LogGamma(alpha_) - 0.5 * (std::log(beta_) - M_LN2 + std::log(kappa_) - 1.0 - std::log(M_PI));
}

/* Parameters value and description accessor */
Point NormalGamma::getParameter() const
{
  Point point(4);
  point[0] = mu_;
  point[1] = kappa_;
  point[2] = alpha_;
  point[3] = beta_;
  return point;
} // getParameter

void NormalGamma::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 4) throw InvalidArgumentException(HERE) << "Error: expected 4 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = NormalGamma(parameter[0], parameter[1], parameter[2], parameter[3]);
  setWeight(w);
} // setParameter

/* Parameters value and description accessor */
Description NormalGamma::getParameterDescription() const
{
  Description description(4);
  description[0] = "mu";
  description[1] = "kappa";
  description[2] = "alpha";
  description[3] = "beta";
  return description;
} // getParameterDescription

/* String converter */
String NormalGamma::__repr__() const
{
  OSS oss(true);
  oss << "class=" << NormalGamma::GetClassName()
      << " name=" << getName()
      << " mu=" << mu_
      << " kappa=" << kappa_
      << " alpha=" << alpha_
      << " beta=" << beta_;
  return oss;
}

String NormalGamma::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu=" << mu_ << ", kappa=" << kappa_ << ", alpha=" << alpha_ << ", beta=" << beta_ << ")";
  return oss;
}

/* Virtual constructor */
NormalGamma * NormalGamma::clone() const
{
  return new NormalGamma(*this);
}

/* Mu accessor */
void NormalGamma::setMu(const Scalar mu)
{
  if (mu != mu_)
  {
    mu_ = mu;
    computeLogNormalization();
    isAlreadyComputedMean_ = false;
  }
}

Scalar NormalGamma::getMu() const
{
  return mu_;
}


/* Kappa accessor */
void NormalGamma::setKappa(const Scalar kappa)
{
  if (kappa != kappa_)
  {
    if (!(kappa_ > 0.0)) throw InvalidArgumentException(HERE) << "Error: kappa must be positive, here kappa=" << kappa;
    kappa_ = kappa;
    computeLogNormalization();
    isAlreadyComputedCovariance_ = false;
  }
}

Scalar NormalGamma::getKappa() const
{
  return kappa_;
}

/* Alpha accessor */
void NormalGamma::setAlpha(const Scalar alpha)
{
  if (!(alpha == alpha_))
  {
    if (!(alpha_ > 0.0)) throw InvalidArgumentException(HERE) << "Error: alpha must be positive, here alpha=" << alpha;
    alpha_ = alpha;
    computeLogNormalization();
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
  }
}

Scalar NormalGamma::getAlpha() const
{
  return alpha_;
}

/* Beta accessor */
void NormalGamma::setBeta(const Scalar beta)
{
  if (!(beta == beta_))
  {
    if (!(beta_ > 0.0)) throw InvalidArgumentException(HERE) << "Error: beta must be positive, here beta=" << beta;
    beta_ = beta;
    computeLogNormalization();
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
  }
}

Scalar NormalGamma::getBeta() const
{
  return beta_;
}

/* Get the skewness of the distribution */
Point NormalGamma::getSkewness() const
{
  Point skewness(2);
  skewness[1] = 2.0 / std::sqrt(alpha_);
  return skewness;
}

/* Get the kurtosis of the distribution */
Point NormalGamma::getKurtosis() const
{
  Point kurtosis(2);
  kurtosis[0] = 3.0 * (alpha_ - 1.0) / (alpha_ - 2.0);
  kurtosis[1] = 3.0 * (alpha_ + 2.0) / alpha_;
  return kurtosis;
}

/* Method save() stores the object through the StorageManager */
void NormalGamma::save(Advocate & adv) const
{
  BayesDistribution::save(adv);
  adv.saveAttribute( "mu_", mu_ );
  adv.saveAttribute( "kappa_", kappa_ );
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
}

/* Method load() reloads the object from the StorageManager */
void NormalGamma::load(Advocate & adv)
{
  BayesDistribution::load(adv);
  adv.loadAttribute( "mu_", mu_ );
  adv.loadAttribute( "kappa_", kappa_ );
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
  computeLogNormalization();
  // The range is computed using the upper class
}


END_NAMESPACE_OPENTURNS
