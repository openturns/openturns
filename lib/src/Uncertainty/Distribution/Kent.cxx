//                                               -*- C++ -*-
/**
 *  @brief The Kent (Fisher-Bingham) distribution
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Kent.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Function.hxx"
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Kent)

static const Factory<Kent> Factory_Kent;

class KentLogUnscaledPDFEvaluation : public EvaluationImplementation
{
public:
  KentLogUnscaledPDFEvaluation(const Scalar kappa,
                               const Scalar beta,
                               const Point & gamma1,
                               const Point & gamma2,
                               const Point & gamma3,
                               const Scalar normalization)
    : EvaluationImplementation()
    , kappa_(kappa)
    , beta_(beta)
    , gamma1_(gamma1)
    , gamma2_(gamma2)
    , gamma3_(gamma3)
    , normalization_(normalization)
  {
    // Nothing to do
  }

  KentLogUnscaledPDFEvaluation * clone() const override
  {
    return new KentLogUnscaledPDFEvaluation(*this);
  }

  Point operator()(const Point & inP) const override
  {
    const Scalar theta = inP[0];
    const Scalar phi = inP[1];
    const Scalar sinTheta = std::sin(theta);
    const Scalar cosTheta = std::cos(theta);
    const Scalar sinPhi = std::sin(phi);
    const Scalar cosPhi = std::cos(phi);
    const Scalar x0 = sinTheta * cosPhi;
    const Scalar x1 = sinTheta * sinPhi;
    const Scalar x2 = cosTheta;
    const Scalar dot1 = x0 * gamma1_[0] + x1 * gamma1_[1] + x2 * gamma1_[2];
    const Scalar dot2 = x0 * gamma2_[0] + x1 * gamma2_[1] + x2 * gamma2_[2];
    const Scalar dot3 = x0 * gamma3_[0] + x1 * gamma3_[1] + x2 * gamma3_[2];
    const Scalar logSin = std::log(sinTheta);
    return Point(1, logSin + kappa_ * dot1 + beta_ * (dot2 * dot2 - dot3 * dot3) - normalization_);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 2;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

private:
  Scalar kappa_;
  Scalar beta_;
  Point gamma1_;
  Point gamma2_;
  Point gamma3_;
  Scalar normalization_;
};

Kent::Kent()
  : DistributionImplementation()
  , kappa_(1.0)
  , beta_(0.25)
  , gamma1_({1.0, 0.0, 0.0})
  , gamma2_({0.0, 1.0, 0.0})
  , gamma3_({0.0, 0.0, 1.0})
  , epsilon_(1e-15)
  , normalization_(0.0)
{
  setName("Kent");
  setDimension(3);
  computeNormalization();
  updateSampler();
  computeRange();
}

Kent::Kent(const Scalar kappa,
           const Scalar beta,
           const Point & gamma1,
           const Point & gamma2,
           const Point & gamma3,
           const Scalar epsilon)
  : DistributionImplementation()
  , kappa_(0.0)
  , beta_(0.0)
  , gamma1_(gamma1)
  , gamma2_(gamma2)
  , gamma3_(gamma3)
  , epsilon_(epsilon)
  , normalization_(0.0)
{
  if (!(kappa > 0.0))
    throw InvalidArgumentException(HERE) << "in Kent: kappa must be > 0, got kappa=" << kappa;
  if (!(beta >= 0.0))
    throw InvalidArgumentException(HERE) << "in Kent: beta must be >= 0, got beta=" << beta;
  if (!(2.0 * beta < kappa))
    throw InvalidArgumentException(HERE) << "in Kent: 2*beta must be < kappa, got kappa=" << kappa << ", beta=" << beta;
  if (gamma1.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "in Kent: gamma1 must be of dimension 3, got dimension=" << gamma1.getDimension();
  if (gamma2.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "in Kent: gamma2 must be of dimension 3, got dimension=" << gamma2.getDimension();
  if (gamma3.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "in Kent: gamma3 must be of dimension 3, got dimension=" << gamma3.getDimension();
  const Scalar norm1 = gamma1.norm();
  const Scalar norm2 = gamma2.norm();
  const Scalar norm3 = gamma3.norm();
  if (std::abs(norm1 - 1.0) > epsilon)
    throw InvalidArgumentException(HERE) << "in Kent: gamma1 must be a unit vector, norm=" << norm1;
  if (std::abs(norm2 - 1.0) > epsilon)
    throw InvalidArgumentException(HERE) << "in Kent: gamma2 must be a unit vector, norm=" << norm2;
  if (std::abs(norm3 - 1.0) > epsilon)
    throw InvalidArgumentException(HERE) << "in Kent: gamma3 must be a unit vector, norm=" << norm3;
  const Scalar dot12 = gamma1.dot(gamma2);
  const Scalar dot13 = gamma1.dot(gamma3);
  const Scalar dot23 = gamma2.dot(gamma3);
  if (std::abs(dot12) > epsilon)
    throw InvalidArgumentException(HERE) << "in Kent: gamma1 and gamma2 must be orthogonal, dot=" << dot12;
  if (std::abs(dot13) > epsilon)
    throw InvalidArgumentException(HERE) << "in Kent: gamma1 and gamma3 must be orthogonal, dot=" << dot13;
  if (std::abs(dot23) > epsilon)
    throw InvalidArgumentException(HERE) << "in Kent: gamma2 and gamma3 must be orthogonal, dot=" << dot23;
  setName("Kent");
  setDimension(3);
  kappa_ = kappa;
  beta_ = beta;
  computeNormalization();
  updateSampler();
  computeRange();
}

Bool Kent::operator ==(const Kent & other) const
{
  if (this == &other) return true;
  return (kappa_ == other.kappa_)
      && (beta_ == other.beta_)
      && (gamma1_ == other.gamma1_)
      && (gamma2_ == other.gamma2_)
      && (gamma3_ == other.gamma3_);
}

Bool Kent::equals(const DistributionImplementation & other) const
{
  const Kent* p_other = dynamic_cast<const Kent*>(&other);
  return p_other && (*this == *p_other);
}

String Kent::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Kent::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " kappa=" << kappa_
      << " beta=" << beta_
      << " gamma1=" << gamma1_
      << " gamma2=" << gamma2_
      << " gamma3=" << gamma3_;
  return oss;
}

String Kent::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(kappa = " << kappa_ << ", beta = " << beta_
      << ", gamma1 = " << gamma1_ << ", gamma2 = " << gamma2_ << ", gamma3 = " << gamma3_ << ")";
  return oss;
}

Kent * Kent::clone() const
{
  return new Kent(*this);
}

void Kent::computeRange()
{
  setRange(Interval({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0}));
}

void Kent::computeNormalization()
{
  if (beta_ == 0.0)
  {
    normalization_ = std::exp(std::log(4.0 * M_PI) + std::log(std::sinh(kappa_)) - std::log(kappa_));
    return;
  }
  Scalar factor = 0.0;
  const Scalar logBeta = std::log(beta_);
  const Scalar logHalfKappa = std::log(0.5 * kappa_);
  UnsignedInteger j = 0;
  Bool go = true;
  while (go)
  {
    const Scalar delta = std::exp(SpecFunc::LogGamma(j + 0.5) - SpecFunc::LogGamma(j + 1.0)
                                  + 2.0 * j * logBeta
                                  - (2.0 * j + 0.5) * logHalfKappa
                                  + SpecFunc::LogBesselInu(kappa_, 2.0 * j + 0.5));
    factor += delta;
    ++j;
    if (delta < epsilon_ * factor) go = false;
  }
  normalization_ = factor;
}

void Kent::updateSampler()
{
  EvaluationImplementation * p_eval = new KentLogUnscaledPDFEvaluation(kappa_, beta_, gamma1_, gamma2_, gamma3_, std::log(normalization_));
  Function logUnscaledPDF(p_eval);
  sampler_ = RatioOfUniforms(logUnscaledPDF, Interval({0.0, 0.0}, {M_PI, 2.0 * M_PI}), true);
}

Point Kent::getRealization() const
{
  const Point thetaPhi = sampler_.getRealization();
  const Scalar theta = thetaPhi[0];
  const Scalar phi = thetaPhi[1];
  const Scalar sinTheta = std::sin(theta);
  const Scalar cosTheta = std::cos(theta);
  const Scalar sinPhi = std::sin(phi);
  const Scalar cosPhi = std::cos(phi);
  Point result(3);
  result[0] = sinTheta * cosPhi;
  result[1] = sinTheta * sinPhi;
  result[2] = cosTheta;
  return result;
}

Scalar Kent::computePDF(const Point & point) const
{
  if (point.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=3, here dimension=" << point.getDimension();
  const Scalar normX = point.norm();
  if (std::abs(normX - 1.0) > epsilon_) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar Kent::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=3, here dimension=" << point.getDimension();
  const Scalar normX = point.norm();
  if (std::abs(normX - 1.0) > epsilon_) return -SpecFunc::Infinity;
  const Scalar dot1 = point.dot(gamma1_);
  const Scalar dot2 = point.dot(gamma2_);
  const Scalar dot3 = point.dot(gamma3_);
  return kappa_ * dot1 + beta_ * (dot2 * dot2 - dot3 * dot3) - std::log(normalization_);
}

Point Kent::getParameter() const
{
  Point point(11);
  point[0] = kappa_;
  point[1] = beta_;
  point[2] = gamma1_[0];
  point[3] = gamma1_[1];
  point[4] = gamma1_[2];
  point[5] = gamma2_[0];
  point[6] = gamma2_[1];
  point[7] = gamma2_[2];
  point[8] = gamma3_[0];
  point[9] = gamma3_[1];
  point[10] = gamma3_[2];
  return point;
}

void Kent::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 11)
    throw InvalidArgumentException(HERE) << "Error: expected 11 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  *this = Kent(parameter[0], parameter[1],
               {parameter[2], parameter[3], parameter[4]},
               {parameter[5], parameter[6], parameter[7]},
               {parameter[8], parameter[9], parameter[10]},
               epsilon_);
  setWeight(w);
}

Description Kent::getParameterDescription() const
{
  Description description(11);
  description[0] = "kappa";
  description[1] = "beta";
  description[2] = "gamma1_0";
  description[3] = "gamma1_1";
  description[4] = "gamma1_2";
  description[5] = "gamma2_0";
  description[6] = "gamma2_1";
  description[7] = "gamma2_2";
  description[8] = "gamma3_0";
  description[9] = "gamma3_1";
  description[10] = "gamma3_2";
  return description;
}

Scalar Kent::getKappa() const
{
  return kappa_;
}

void Kent::setKappa(const Scalar kappa)
{
  if (!(kappa > 0.0))
    throw InvalidArgumentException(HERE) << "in Kent::setKappa: kappa must be > 0, got kappa=" << kappa;
  if (kappa != kappa_)
  {
    kappa_ = kappa;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeNormalization();
    updateSampler();
    computeRange();
  }
}

Scalar Kent::getBeta() const
{
  return beta_;
}

void Kent::setBeta(const Scalar beta)
{
  if (!(beta >= 0.0))
    throw InvalidArgumentException(HERE) << "in Kent::setBeta: beta must be >= 0, got beta=" << beta;
  if (!(2.0 * beta < kappa_))
    throw InvalidArgumentException(HERE) << "in Kent::setBeta: 2*beta must be < kappa, got kappa=" << kappa_ << ", beta=" << beta;
  if (beta != beta_)
  {
    beta_ = beta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeNormalization();
    updateSampler();
  }
}

Point Kent::getGamma1() const
{
  return gamma1_;
}

void Kent::setGamma1(const Point & gamma1)
{
  if (gamma1.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "in Kent::setGamma1: gamma1 must be of dimension 3, got dimension=" << gamma1.getDimension();
  if (std::abs(gamma1.norm() - 1.0) > epsilon_)
    throw InvalidArgumentException(HERE) << "in Kent::setGamma1: gamma1 must be a unit vector, norm=" << gamma1.norm();
  if (gamma1 != gamma1_)
  {
    gamma1_ = gamma1;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    updateSampler();
  }
}

Point Kent::getGamma2() const
{
  return gamma2_;
}

void Kent::setGamma2(const Point & gamma2)
{
  if (gamma2.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "in Kent::setGamma2: gamma2 must be of dimension 3, got dimension=" << gamma2.getDimension();
  if (std::abs(gamma2.norm() - 1.0) > epsilon_)
    throw InvalidArgumentException(HERE) << "in Kent::setGamma2: gamma2 must be a unit vector, norm=" << gamma2.norm();
  if (gamma2 != gamma2_)
  {
    gamma2_ = gamma2;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    updateSampler();
  }
}

Point Kent::getGamma3() const
{
  return gamma3_;
}

void Kent::setGamma3(const Point & gamma3)
{
  if (gamma3.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "in Kent::setGamma3: gamma3 must be of dimension 3, got dimension=" << gamma3.getDimension();
  if (std::abs(gamma3.norm() - 1.0) > epsilon_)
    throw InvalidArgumentException(HERE) << "in Kent::setGamma3: gamma3 must be a unit vector, norm=" << gamma3.norm();
  if (gamma3 != gamma3_)
  {
    gamma3_ = gamma3;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    updateSampler();
  }
}

Scalar Kent::getEpsilon() const
{
  return epsilon_;
}

Bool Kent::isContinuous() const
{
  return true;
}

void Kent::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("kappa_", kappa_);
  adv.saveAttribute("beta_", beta_);
  adv.saveAttribute("gamma1_", gamma1_);
  adv.saveAttribute("gamma2_", gamma2_);
  adv.saveAttribute("gamma3_", gamma3_);
  adv.saveAttribute("epsilon_", epsilon_);
}

void Kent::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("kappa_", kappa_);
  adv.loadAttribute("beta_", beta_);
  adv.loadAttribute("gamma1_", gamma1_);
  adv.loadAttribute("gamma2_", gamma2_);
  adv.loadAttribute("gamma3_", gamma3_);
  adv.loadAttribute("epsilon_", epsilon_);
  computeNormalization();
  updateSampler();
  computeRange();
}

END_NAMESPACE_OPENTURNS
