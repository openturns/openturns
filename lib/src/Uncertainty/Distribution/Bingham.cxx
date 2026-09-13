//                                               -*- C++ -*-
/**
 *  @brief The Bingham distribution
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
#include <algorithm>
#include <functional>
#include "openturns/Bingham.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/DistFunc.hxx"


BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Bingham)

static const Factory<Bingham> Factory_Bingham;

Bingham::Bingham()
  : DistributionImplementation()
  , dimension_(3)
  , zeta_(3)
  , gamma_(3)
  , epsilon_(ResourceMap::GetAsScalar("Bingham-OrthogonalityThreshold"))
  , logNormalization_(0.0)
  , optimalB_(0.0)
{
  zeta_[0] = 0.0;
  zeta_[1] = -0.5;
  zeta_[2] = -1.0;
  gamma_(0, 0) = 1.0;
  gamma_(1, 1) = 1.0;
  gamma_(2, 2) = 1.0;
  setName("Bingham");
  setDimension(3);
  computeNormalization();
  updateSampler();
  computeRange();
}

Bingham::Bingham(const Point & zeta,
                 const SquareMatrix & gamma,
                 const Scalar epsilon)
  : DistributionImplementation()
  , dimension_(zeta.getDimension())
  , zeta_(zeta)
  , gamma_(gamma.getDimension())
  , epsilon_(std::max(SpecFunc::ScalarEpsilon, epsilon))
  , logNormalization_(0.0)
  , optimalB_(0.0)
{
  const UnsignedInteger n = dimension_;
  if (n < 2)
    throw InvalidArgumentException(HERE) << "in Bingham: dimension must be >= 2, got n=" << n;
  if (zeta.getSize() != n)
    throw InvalidArgumentException(HERE) << "in Bingham: zeta must have size=" << n << ", got size=" << zeta.getSize();
  if (gamma.getDimension() != n)
    throw InvalidArgumentException(HERE) << "in Bingham: gamma must be of dimension " << n << ", got dimension=" << gamma.getDimension();

  // Check that zeta is ordered: zeta[0] >= zeta[1] >= ... >= zeta[n-1]
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    if (zeta[i-1] < zeta[i])
      throw InvalidArgumentException(HERE) << "in Bingham: zeta must be in non-increasing order";
  }

  // Check orthonormality of gamma
  if ((gamma.computeGram() - IdentityMatrix(n)).frobeniusNorm() > epsilon_)
    throw InvalidArgumentException(HERE) << "in Bingham: gamma must be an orthogonal matrix.";

  // The density only depends on zeta up to a constant shift: normalize so
  // that the smallest concentration parameter is zero (canonical form)
  const Scalar minZeta = zeta[n - 1];
  for (UnsignedInteger i = 0; i < n; ++i)
    zeta_[i] = zeta[i] - minZeta;

  gamma_ = gamma;
  setName("Bingham");
  setDimension(n);
  computeNormalization();
  updateSampler();
  computeRange();
}

Bool Bingham::operator ==(const Bingham & other) const
{
  if (this == &other) return true;
  return (dimension_ == other.dimension_)
      && (zeta_ == other.zeta_)
      && (gamma_ == other.gamma_);
}

Bool Bingham::equals(const DistributionImplementation & other) const
{
  const Bingham* p_other = dynamic_cast<const Bingham*>(&other);
  return p_other && (*this == *p_other);
}

String Bingham::__repr__() const
{
  OSS oss(true);
  oss << "class=" << Bingham::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " zeta=" << zeta_
      << " gamma=" << gamma_;
  return oss;
}

String Bingham::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(zeta = " << zeta_ << ", gamma = " << gamma_ << ")";
  return oss;
}

Bingham * Bingham::clone() const
{
  return new Bingham(*this);
}

void Bingham::computeRange()
{
  Point lower(dimension_, -1.0);
  Point upper(dimension_, 1.0);
  setRange(Interval(lower, upper));
}

Scalar Bingham::computeLogNormalizationConstant(const Point & zeta) const
{
  const UnsignedInteger n = dimension_;
  Point cZeta(zeta);

  // The density depends on zeta up to an additive constant: shift so that
  // the smallest entry is zero, then F(zeta) = exp(minZeta) * F(zeta - minZeta)
  Scalar minZeta = cZeta[0];
  for (UnsignedInteger i = 1; i < n; ++i)
    minZeta = std::min(minZeta, cZeta[i]);
  for (UnsignedInteger i = 0; i < n; ++i)
    cZeta[i] -= minZeta;

  // log of the surface area of the unit sphere S^{n-1}
  const Scalar logArea = std::log(2.0) + 0.5 * n * std::log(M_PI) - SpecFunc::LogGamma(0.5 * n);

  // Log-space addition, robust to infinities and to the almost-equal case
  auto logAdd = [](const Scalar a, const Scalar b)
  {
    if (a == -SpecFunc::Infinity) return b;
    if (b == -SpecFunc::Infinity) return a;
    if (a >= b) return a + std::log1p(std::exp(b - a));
    return b + std::log1p(std::exp(a - b));
  };

  // F(zeta) = area * sum_{k>=0} S_k with
  // S_k = Gamma(n/2)/Gamma(n/2+k) * sum_{|m|=k} prod_i (1/2)_{m_i} zeta_i^{m_i} / m_i!
  // All the terms are nonnegative (canonical zeta >= 0), sum them in log-space.
  // The k = 0 block is exactly 1, so logSum starts at 0.
  const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("Bingham-MaximumIteration");
  Scalar logSum = 0.0;
  Bool converged = false;
  for (UnsignedInteger k = 1; k <= maximumIteration; ++k)
  {
    // log of the block sum S_k
    Scalar logBlock = -SpecFunc::Infinity;
    // log of (1/2)_{m} zeta^m / m! = logGamma(m + 1/2) - logGamma(1/2) + m log(zeta) - logGamma(m + 1)
    // with logGamma(1/2) = 0.5 * log(pi)
    const Scalar logBlockFactor = SpecFunc::LogGamma(0.5 * n) - SpecFunc::LogGamma(0.5 * n + static_cast<Scalar>(k));
    std::function<void(UnsignedInteger, UnsignedInteger, Scalar)> enumerate =
        [&](UnsignedInteger index, UnsignedInteger remaining, Scalar logProduct)
    {
      if (index == n - 1)
      {
        const UnsignedInteger mLast = remaining;
        Scalar logTermLast = 0.0;
        if (mLast > 0)
          logTermLast = (cZeta[index] == 0.0) ? -SpecFunc::Infinity
                        : SpecFunc::LogGamma(mLast + 0.5) - 0.5 * std::log(M_PI)
                          + static_cast<Scalar>(mLast) * std::log(cZeta[index])
                          - SpecFunc::LogGamma(mLast + 1.0);
        const Scalar logBlockTerm = logProduct + logTermLast + logBlockFactor;
        logBlock = logAdd(logBlock, logBlockTerm);
        return;
      }
      for (UnsignedInteger mIndex = 0; mIndex <= remaining; ++mIndex)
      {
        Scalar logMi = 0.0;
        if (mIndex > 0)
          logMi = (cZeta[index] == 0.0) ? -SpecFunc::Infinity
                  : SpecFunc::LogGamma(mIndex + 0.5) - 0.5 * std::log(M_PI)
                    + static_cast<Scalar>(mIndex) * std::log(cZeta[index])
                    - SpecFunc::LogGamma(mIndex + 1.0);
        enumerate(index + 1, remaining - mIndex, logProduct + logMi);
      }
    };
    enumerate(0, k, 0.0);
    logSum = logAdd(logSum, logBlock);
    // Stop when the block contribution falls below the precision threshold
    if (logBlock < logSum + std::log(SpecFunc::Precision))
    {
      converged = true;
      break;
    }
  }

  if (!converged)
  {
    // The series converged too slowly: use the saddlepoint approximation
    // log F(zeta) ~ 0.5 * sum_i log(1 + 2*zeta_i) + (n/2) log(2*pi) - log Gamma(n/2)
    Scalar saddle = 0.0;
    for (UnsignedInteger i = 0; i < n; ++i)
      saddle += 0.5 * std::log1p(2.0 * cZeta[i]);
    return saddle + minZeta;
  }

  return logArea + minZeta + logSum;
}

void Bingham::computeNormalization()
{
  logNormalization_ = computeLogNormalizationConstant(zeta_);
}

Point Bingham::computeSecondMoments() const
{
  const UnsignedInteger n = dimension_;
  const Scalar delta = std::sqrt(SpecFunc::ScalarEpsilon);
  Point secondMoments(n);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    Point zetaPlus(zeta_);
    Point zetaMinus(zeta_);
    const Scalar step = delta * std::max(1.0, std::abs(zeta_[i]));
    zetaPlus[i] += step;
    zetaMinus[i] -= step;
    const Scalar logFP = computeLogNormalizationConstant(zetaPlus);
    const Scalar logFM = computeLogNormalizationConstant(zetaMinus);
    secondMoments[i] = (logFP - logFM) / (2.0 * step);
  }
  return secondMoments;
}

void Bingham::updateSampler()
{
  // Sampling is done by acceptance-rejection from an Angular Central Gaussian
  // proposal (Kent, Ganeiber & Mardia, 2013). The mean direction of the
  // proposal is e_1 (axis of the largest concentration) and its inverse
  // concentration matrix is diag(b, b + zeta_1 - zeta_2, ..., b + zeta_1 - zeta_n).
  // The parameter b is chosen as the root of sum_i 1/(b + zeta_1 - zeta_i) = 1,
  // which makes the envelope asymptotically tight.
  const UnsignedInteger n = dimension_;
  Point delta(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    delta[i] = zeta_[0] - zeta_[i];

  // Uniform case: zeta_1 = zeta_n, no concentration
  if (delta[n - 1] < SpecFunc::ScalarEpsilon)
  {
    optimalB_ = static_cast<Scalar>(n);
    return;
  }

  // Root of sum_i 1/(b + delta_i) = 1 by bisection over (0, upper)
  Scalar lower = SpecFunc::ScalarEpsilon;
  Scalar upper = std::max(1.0, 2.0 * delta[n - 1]);
  auto f = [&](const Scalar b)
  {
    Scalar sum = 0.0;
    for (UnsignedInteger i = 0; i < n; ++i)
      sum += 1.0 / (b + delta[i]);
    return sum - 1.0;
  };
  while (f(upper) > 0.0)
    upper *= 2.0;
  while (upper - lower > SpecFunc::Precision * (lower + upper))
  {
    const Scalar mid = 0.5 * (lower + upper);
    if (f(mid) > 0.0) lower = mid;
    else upper = mid;
  }
  optimalB_ = 0.5 * (lower + upper);
}

Point Bingham::getRealization() const
{
  const UnsignedInteger n = dimension_;
  const Scalar b = optimalB_;

  // Maximum over u in [0, zeta_1] of h(u) = exp(-u) * (b + u)^{n/2} of the
  // acceptance function, used to normalize the rejection envelope.
  const Scalar uMax = zeta_[0];
  Point delta(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    delta[i] = zeta_[0] - zeta_[i];
  const Scalar invScale1 = std::sqrt(1.0 / b);
  Point invScale(n);
  invScale[0] = invScale1;
  for (UnsignedInteger i = 1; i < n; ++i)
    invScale[i] = std::sqrt(1.0 / (b + delta[i]));

  // log of h(0) = b^{n/2}, log of h(uMax), and of h at the stationarity point
  const Scalar halfN = 0.5 * n;
  Point logCandidate(3);
  logCandidate[0] = halfN * std::log(b);
  logCandidate[1] = -uMax + halfN * std::log(b + uMax);
  const Scalar critical = halfN - b;
  logCandidate[2] = ((critical >= 0.0) && (critical <= uMax))
                    ? -critical + halfN * std::log(halfN)
                    : -SpecFunc::LogMaxScalar;
  const Scalar logMaxAccept = *std::max_element(logCandidate.begin(), logCandidate.end());

  std::vector<Scalar> z(n);
  for (;;)
  {
    Scalar norm = 0.0;
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      const Scalar zi = DistFunc::rNormal() * invScale[i];
      z[i] = zi;
      norm += zi * zi;
    }
    norm = std::sqrt(norm);
    Scalar y1 = z[0] / norm;
    Scalar u = 0.0;
    for (UnsignedInteger i = 1; i < n; ++i)
    {
      const Scalar yi = z[i] / norm;
      u += delta[i] * yi * yi;
    }
    const Scalar logAccept = -u + halfN * std::log(b + u) - logMaxAccept;
    if (std::log(RandomGenerator::Generate()) < logAccept)
    {
      // Rotate the diagonalizing frame back to the original one: x = gamma * y
      Point result(n);
      for (UnsignedInteger i = 0; i < n; ++i)
      {
        result[i] = y1 * gamma_(i, 0);
        for (UnsignedInteger j = 1; j < n; ++j)
          result[i] += (z[j] / norm) * gamma_(i, j);
      }
      return result;
    }
  }
}

Scalar Bingham::computePDF(const Point & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();
  const Scalar normX = point.norm();
  if (std::abs(normX - 1.0) > epsilon_) return 0.0;
  return std::exp(computeLogPDF(point));
}

Scalar Bingham::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();
  const Scalar normX = point.norm();
  if (std::abs(normX - 1.0) > epsilon_) return -SpecFunc::Infinity;

  // The density is exp(x^T gamma diag(zeta) gamma^T x) = exp(sum zeta_i (gamma_i.x)^2)
  // where gamma_i are the COLUMNS of the orientation matrix
  Scalar exponent = 0.0;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar dot = 0.0;
    for (UnsignedInteger j = 0; j < dimension_; ++j)
      dot += gamma_(j, i) * point[j];
    exponent += zeta_[i] * dot * dot;
  }

  return exponent - logNormalization_;
}

Point Bingham::getParameter() const
{
  const UnsignedInteger n = dimension_;
  Point point(n + n * n);
  for (UnsignedInteger i = 0; i < n; ++i)
    point[i] = zeta_[i];
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = 0; j < n; ++j)
      point[n + i * n + j] = gamma_(i, j);
  return point;
}

void Bingham::setParameter(const Point & parameter)
{
  const UnsignedInteger n = dimension_;
  if (parameter.getSize() != n + n * n)
    throw InvalidArgumentException(HERE) << "Error: expected " << (n + n * n) << " values, got " << parameter.getSize();

  Point zeta(n);
  SquareMatrix gamma(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    zeta[i] = parameter[i];
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = 0; j < n; ++j)
      gamma(i, j) = parameter[n + i * n + j];

  const Scalar w = getWeight();
  *this = Bingham(zeta, gamma, epsilon_);
  setWeight(w);
}

Description Bingham::getParameterDescription() const
{
  const UnsignedInteger n = dimension_;
  Description description(n + n * n);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    OSS oss;
    oss << "zeta_" << i;
    description[i] = oss.str();
  }
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = 0; j < n; ++j)
    {
      OSS oss;
      oss << "gamma_" << i << "_" << j;
      description[n + i * n + j] = oss.str();
    }
  return description;
}

void Bingham::setZeta(const Point & zeta)
{
  const UnsignedInteger n = dimension_;
  if (zeta.getSize() != n)
    throw InvalidArgumentException(HERE) << "Error: zeta has a dimension=" << zeta.getDimension() << " must be equal to the distribution dimension=" << n;

  // Check ordering
  for (UnsignedInteger i = 1; i < n; ++i)
  {
    if (zeta[i-1] < zeta[i])
      throw InvalidArgumentException(HERE) << "in Bingham::setZeta: zeta must be in non-increasing order";
  }

  // Canonicalize: shift so that the smallest zeta is zero
  const Scalar minZeta = zeta[n - 1];
  Point canonicalZeta(n);
  for (UnsignedInteger i = 0; i < n; ++i)
    canonicalZeta[i] = zeta[i] - minZeta;

  if (canonicalZeta != zeta_)
  {
    zeta_ = canonicalZeta;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeNormalization();
    updateSampler();
    computeRange();
  }
}

Point Bingham::getZeta() const
{
  return zeta_;
}

void Bingham::setGamma(const SquareMatrix & gamma)
{
  const UnsignedInteger n = dimension_;
  if (gamma.getDimension() != n)
    throw InvalidArgumentException(HERE) << "in Bingham::setGamma: gamma must be of dimension " << n << ", got dimension=" << gamma.getDimension();
  if ((gamma.computeGram() - IdentityMatrix(n)).frobeniusNorm() > epsilon_)
    throw InvalidArgumentException(HERE) << "in Bingham::setGamma: the matrix is not orthogonal.";

  if (gamma != gamma_)
  {
    gamma_ = gamma;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    updateSampler();
  }
}

SquareMatrix Bingham::getGamma() const
{
  return gamma_;
}

Scalar Bingham::getEpsilon() const
{
  return epsilon_;
}

void Bingham::computeMean() const
{
  // The Bingham distribution is antipodally symmetric: the mean is zero
  mean_ = Point(dimension_, 0.0);
  isAlreadyComputedMean_ = true;
}

void Bingham::computeCovariance() const
{
  // E[X X^T] = gamma diag(E[u_i^2]) gamma^T where u = gamma^T X is in the
  // diagonalizing frame: the coordinates are independent in the sense that
  // E[u_i u_j] = 0 for i != j and E[u_i] = 0.
  const Point secondMoments(computeSecondMoments());
  SquareMatrix diag(dimension_);
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    diag(i, i) = secondMoments[i];
  const SquareMatrix tmp = gamma_ * diag * gamma_.transpose();
  covariance_ = CovarianceMatrix(tmp.getImplementation());
  isAlreadyComputedCovariance_ = true;
}

Scalar Bingham::computeEntropy() const
{
  // Entropy: H = log F(zeta) - E[X^T diag(zeta) X] with
  // E[X^T diag(zeta) X] = sum_i zeta_i E[u_i^2]
  const Point secondMoments(computeSecondMoments());
  Scalar expectation = 0.0;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
    expectation += zeta_[i] * secondMoments[i];
  return logNormalization_ - expectation;
}

Bool Bingham::isContinuous() const
{
  return true;
}

void Bingham::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("zeta_", zeta_);
  adv.saveAttribute("gamma_", gamma_);
  adv.saveAttribute("epsilon_", epsilon_);
  adv.saveAttribute("logNormalization_", logNormalization_);
  adv.saveAttribute("optimalB_", optimalB_);
}

void Bingham::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("zeta_", zeta_);
  adv.loadAttribute("gamma_", gamma_);
  adv.loadAttribute("epsilon_", epsilon_);
  adv.loadAttribute("logNormalization_", logNormalization_);
  adv.loadAttribute("optimalB_", optimalB_);
  computeRange();
}

END_NAMESPACE_OPENTURNS