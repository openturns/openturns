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
#include "openturns/IdentityMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Kent)

static const Factory<Kent> Factory_Kent;

Kent::Kent()
  : DistributionImplementation()
  , kappa_(1.0)
  , beta_(0.25)
  , gamma_(3)
  , epsilon_(ResourceMap::GetAsScalar("Kent-OrthogonalityThreshold"))
  , logNormalization_(0.0)
{
  gamma_(0, 0) = 1.0;
  gamma_(1, 1) = 1.0;
  gamma_(2, 2) = 1.0;
  setName("Kent");
  setDimension(3);
  computeNormalization();
  updateSampler();
  computeRange();
}

Kent::Kent(const Scalar kappa,
           const Scalar beta,
           const SquareMatrix & gamma,
           const Scalar epsilon)
  : DistributionImplementation()
  , kappa_(0.0)
  , beta_(0.0)
  , gamma_(3)
  , epsilon_(std::max(SpecFunc::ScalarEpsilon, epsilon))
  , logNormalization_(0.0)
{
  if (!(kappa > 0.0))
    throw InvalidArgumentException(HERE) << "in Kent: kappa must be > 0, got kappa=" << kappa;
  if (!(beta >= 0.0))
    throw InvalidArgumentException(HERE) << "in Kent: beta must be >= 0, got beta=" << beta;
  if (!(2.0 * beta < kappa))
    throw InvalidArgumentException(HERE) << "in Kent: 2*beta must be < kappa, got kappa=" << kappa << ", beta=" << beta;
  if (gamma.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "in Kent: gamma must be of dimension 3, got dimension=" << gamma.getDimension();
  if ((gamma.computeGram() - IdentityMatrix(3)).frobeniusNorm() > epsilon)
    throw InvalidArgumentException(HERE) << "in Kent: gamma must be an orthogonal matrix.";
  gamma_ = gamma;
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
      && (gamma_ == other.gamma_);
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
      << " gamma=" << gamma_;
  return oss;
}

String Kent::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(kappa = " << kappa_ << ", beta = " << beta_
      << ", gamma = " << gamma_ << ")";
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

Point Kent::ComputeMoments(const Scalar kappa,
                           const Scalar beta)
{
  Scalar r1 = 0.0;
  Scalar r2 = 0.0;
  if (beta == 0.0)
  {
    // von Mises-Fisher case: r1 = I_{3/2}(kappa) / I_{1/2}(kappa) = coth(kappa) - 1/kappa, r2 = 0
    if (kappa < 0.07)
    {
      // Use Taylor expansion for small kappa: coth(kappa) - 1/kappa = kappa/3 - kappa^3/45 + 2*kappa^5/945 - kappa^7/4725 + ...
      // Error bound after 4 terms: next term = 2*kappa^9/93555, which is < 1e-15 for kappa < 0.07
      const Scalar kappa2 = kappa * kappa;
      const Scalar kappa4 = kappa2 * kappa2;
      r1 = kappa / 3.0 - kappa * kappa2 / 45.0 + (2.0 / 945.0) * kappa * kappa4 - (1.0 / 4725.0) * kappa * kappa4 * kappa2;
    }
    else
    {
      const Scalar expM2K = std::exp(-2.0 * kappa);
      r1 = (1.0 + expM2K) / (1.0 - expM2K) - 1.0 / kappa;
    }
    r2 = 0.0;
  }
  else
  {
    const Scalar epsilon = SpecFunc::ScalarEpsilon;
    Scalar factor = 0.0;
    const Scalar logBeta = std::log(beta);
    const Scalar logHalfKappa = std::log(0.5 * kappa);
    UnsignedInteger j = 0;
    Bool go = true;
    const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("Kent-MaximumIteration");
    while (go)
    {
      Scalar delta = 0.0;
      const Scalar logTerm = SpecFunc::LogGamma(j + 0.5) - SpecFunc::LogGamma(j + 1.0)
                             + 2.0 * j * logBeta
                             - (2.0 * j + 0.5) * logHalfKappa - kappa
                             + SpecFunc::LogBesselInu(kappa, 2.0 * j + 0.5);
      if (logTerm > SpecFunc::LogMaxScalar)
      {
        delta = SpecFunc::MaxScalar;
      }
      else
      {
        delta = std::exp(logTerm);
      }
      factor += delta;
      // R_j = I_{2j+1.5}(kappa) / I_{2j+0.5}(kappa)
      const Scalar logBesselNu1 = SpecFunc::LogBesselInu(kappa, 2.0 * j + 1.5);
      const Scalar logBesselNu0 = SpecFunc::LogBesselInu(kappa, 2.0 * j + 0.5);
      const Scalar Rj = std::exp(logBesselNu1 - logBesselNu0);
      r1 += delta * Rj;
      r2 += static_cast<Scalar>(j) * delta;
      ++j;
      if (delta < epsilon * factor) go = false;
      if (j >= maximumIteration) go = false;
    }
    r1 /= factor;
    r2 *= (2.0 / beta) / factor;
  }
  Point result(2);
  result[0] = r1;
  result[1] = r2;
  return result;
}

void Kent::computeMean() const
{
  const Point moments(ComputeMoments(kappa_, beta_));
  const Scalar r1 = moments[0];
  mean_ = Point(3);
  mean_[0] = r1 * gamma_(0, 0);
  mean_[1] = r1 * gamma_(1, 0);
  mean_[2] = r1 * gamma_(2, 0);
  isAlreadyComputedMean_ = true;
}

void Kent::computeNormalization()
{
  if (beta_ == 0.0)
  {
    logNormalization_ = std::log(2.0 * M_PI) + kappa_ + std::log1p(-std::exp(-2.0 * kappa_)) - std::log(kappa_);
    return;
  }
  Scalar factor = 0.0;
  const Scalar logBeta = std::log(beta_);
  const Scalar logHalfKappa = std::log(0.5 * kappa_);
  UnsignedInteger j = 0;
  Bool go = true;
  const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("Kent-MaximumIteration");
  while (go)
  {
    const Scalar logTerm = SpecFunc::LogGamma(j + 0.5) - SpecFunc::LogGamma(j + 1.0)
                           + 2.0 * j * logBeta
                           - (2.0 * j + 0.5) * logHalfKappa - kappa_
                           + SpecFunc::LogBesselInu(kappa_, 2.0 * j + 0.5);
    Scalar delta = 0.0;
    if (logTerm > SpecFunc::LogMaxScalar)
    {
      delta = SpecFunc::MaxScalar;
    }
    else
    {
      delta = std::exp(logTerm);
    }
    factor += delta;
    ++j;
    if (delta < epsilon_ * factor) go = false;
    if (j >= maximumIteration) go = false;
  }
  logNormalization_ = kappa_ + std::log(factor);
}

void Kent::updateSampler()
{
  // Compute the optimal b parameter for the BACG acceptance-rejection algorithm
  // (Kent, Ganeiber & Mardia, 2013, eq. 3.6)
  // Solves sum_{i=1}^{3} 1/(b + 2*lambda_i) = 1 where lambda = (0, kappa/2-beta, kappa/2+beta)
  const Scalar a2 = kappa_ - 2.0 * beta_;
  const Scalar a3 = kappa_ + 2.0 * beta_;
  Scalar lower = SpecFunc::ScalarEpsilon;
  Scalar upper = 3.0;
  for (UnsignedInteger i = 0; i < 100; ++i)
  {
    const Scalar mid = 0.5 * (lower + upper);
    const Scalar fMid = 1.0 / mid + 1.0 / (mid + a2) + 1.0 / (mid + a3) - 1.0;
    if (fMid > 0.0) lower = mid;
    else upper = mid;
  }
  optimalB_ = 0.5 * (lower + upper);
}

Point Kent::getRealization() const
{
  // BACG acceptance-rejection algorithm
  // Kent, Ganeiber & Mardia (2013), Sections 3 & 6
  const Scalar a2 = kappa_ - 2.0 * beta_;
  const Scalar a3 = kappa_ + 2.0 * beta_;
  const Scalar half3MinusB = 0.5 * (3.0 - optimalB_);
  const Scalar logB3 = std::log(optimalB_ / 3.0);
  const Scalar invScale2 = std::sqrt(1.0 + a2 / optimalB_);
  const Scalar invScale3 = std::sqrt(1.0 + a3 / optimalB_);
  for (;;)
  {
    Scalar u1 = RandomGenerator::Generate();
    Scalar u2 = RandomGenerator::Generate();
    const Scalar z1 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    u1 = RandomGenerator::Generate();
    u2 = RandomGenerator::Generate();
    const Scalar z2 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    u1 = RandomGenerator::Generate();
    u2 = RandomGenerator::Generate();
    const Scalar z3 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    const Scalar y1 = z1;
    const Scalar y2 = z2 / invScale2;
    const Scalar y3 = z3 / invScale3;
    const Scalar norm = std::sqrt(y1 * y1 + y2 * y2 + y3 * y3);
    const Scalar u = y1 / norm;
    const Scalar v = y2 / norm;
    const Scalar w = y3 / norm;
    const Scalar xOmegaX = u * u + (invScale2 * invScale2) * v * v + (invScale3 * invScale3) * w * w;
    const Scalar logAccept = kappa_ * u + beta_ * (v * v - w * w) - kappa_
                             + half3MinusB
                             + 1.5 * (logB3 + std::log(xOmegaX));
    if (std::log(RandomGenerator::Generate()) < logAccept)
    {
      Point result(3);
      result[0] = u * gamma_(0, 0) + v * gamma_(0, 1) + w * gamma_(0, 2);
      result[1] = u * gamma_(1, 0) + v * gamma_(1, 1) + w * gamma_(1, 2);
      result[2] = u * gamma_(2, 0) + v * gamma_(2, 1) + w * gamma_(2, 2);
      return result;
    }
  }
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
  const Scalar dot1 = point[0] * gamma_(0, 0) + point[1] * gamma_(1, 0) + point[2] * gamma_(2, 0);
  const Scalar dot2 = point[0] * gamma_(0, 1) + point[1] * gamma_(1, 1) + point[2] * gamma_(2, 1);
  const Scalar dot3 = point[0] * gamma_(0, 2) + point[1] * gamma_(1, 2) + point[2] * gamma_(2, 2);
  return kappa_ * dot1 + beta_ * (dot2 * dot2 - dot3 * dot3) - logNormalization_;
}

Point Kent::getParameter() const
{
  Point point(11);
  point[0] = kappa_;
  point[1] = beta_;
  point[2] = gamma_(0, 0);
  point[3] = gamma_(1, 0);
  point[4] = gamma_(2, 0);
  point[5] = gamma_(0, 1);
  point[6] = gamma_(1, 1);
  point[7] = gamma_(2, 1);
  point[8] = gamma_(0, 2);
  point[9] = gamma_(1, 2);
  point[10] = gamma_(2, 2);
  return point;
}

void Kent::setParameter(const Point & parameter)
{
  if (parameter.getSize() != 11)
    throw InvalidArgumentException(HERE) << "Error: expected 11 values, got " << parameter.getSize();
  const Scalar w = getWeight();
  SquareMatrix gamma(3);
  gamma(0, 0) = parameter[2];
  gamma(1, 0) = parameter[3];
  gamma(2, 0) = parameter[4];
  gamma(0, 1) = parameter[5];
  gamma(1, 1) = parameter[6];
  gamma(2, 1) = parameter[7];
  gamma(0, 2) = parameter[8];
  gamma(1, 2) = parameter[9];
  gamma(2, 2) = parameter[10];
  *this = Kent(parameter[0], parameter[1], gamma, epsilon_);
  setWeight(w);
}

Description Kent::getParameterDescription() const
{
  Description description(11);
  description[0] = "kappa";
  description[1] = "beta";
  description[2] = "gamma_0_0";
  description[3] = "gamma_1_0";
  description[4] = "gamma_2_0";
  description[5] = "gamma_0_1";
  description[6] = "gamma_1_1";
  description[7] = "gamma_2_1";
  description[8] = "gamma_0_2";
  description[9] = "gamma_1_2";
  description[10] = "gamma_2_2";
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
  if (!(2.0 * beta_ < kappa))
    throw InvalidArgumentException(HERE) << "in Kent::setKappa: kappa must be > 2*beta, got kappa=" << kappa << ", beta=" << beta_;
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

SquareMatrix Kent::getGamma() const
{
  return gamma_;
}

void Kent::setGamma(const SquareMatrix & gamma)
{
  if (gamma.getDimension() != 3)
    throw InvalidArgumentException(HERE) << "in Kent::setGamma: gamma must be of dimension 3, got dimension=" << gamma.getDimension();
  // Check orthonormality of columns
  if ((gamma.computeGram() - IdentityMatrix(3)).frobeniusNorm() > epsilon_)
    throw InvalidArgumentException(HERE) << "in Kent::setGamma: the matrix is not orthogonal.";
  if (gamma != gamma_)
  {
    gamma_ = gamma;
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
  adv.saveAttribute("gamma_", gamma_);
  adv.saveAttribute("epsilon_", epsilon_);
}

void Kent::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("kappa_", kappa_);
  adv.loadAttribute("beta_", beta_);
  adv.loadAttribute("gamma_", gamma_);
  adv.loadAttribute("epsilon_", epsilon_);
  computeNormalization();
  updateSampler();
  computeRange();
}

END_NAMESPACE_OPENTURNS
