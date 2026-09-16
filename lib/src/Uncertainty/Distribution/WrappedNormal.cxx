//                                               -*- C++ -*-
/**
 *  @brief The Wrapped Normal distribution on Torus T^n and Circle S^1
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
#include "openturns/WrappedNormal.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/CovarianceMatrix.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WrappedNormal)

static const Factory<WrappedNormal> Factory_WrappedNormal;

WrappedNormal::WrappedNormal()
  : DistributionImplementation()
  , dimension_(2)
  , mu_(2)
  , sigma_(2)
  , period_(2.0 * M_PI)
  , maxLatticeTerms_(ResourceMap::GetAsUnsignedInteger("WrappedNormal-MaxLatticeTerms"))
  , logNormalization_(0.0)
  , sigmaInv_(2)
  , sigmaDet_(1.0)
  , sigmaEigVec_(2)
  , sigmaEig_(2)
  , maxEig_(0.0)
{
  mu_[0] = 0.0;
  mu_[1] = 0.0;
  sigma_(0, 0) = 1.0;
  sigma_(1, 1) = 1.0;
  sigma_(0, 1) = 0.0;
  setName("WrappedNormal");
  setDimension(2);
  computeNormalization();
  computeRange();
}

WrappedNormal::WrappedNormal(const Point & mu,
                             const CovarianceMatrix & sigma,
                             const Scalar period)
  : DistributionImplementation()
  , dimension_(mu.getDimension())
  , mu_(mu)
  , sigma_(sigma.getDimension())
  , period_(period)
  , maxLatticeTerms_(ResourceMap::GetAsUnsignedInteger("WrappedNormal-MaxLatticeTerms"))
  , logNormalization_(0.0)
  , sigmaInv_(sigma.getDimension())
  , sigmaDet_(1.0)
  , sigmaEigVec_(sigma.getDimension())
  , sigmaEig_(sigma.getDimension())
  , maxEig_(0.0)
{
  const UnsignedInteger d = dimension_;
  if (d < 1)
    throw InvalidArgumentException(HERE) << "in WrappedNormal: dimension must be >= 1, got d=" << d;
  if (sigma.getDimension() != d)
    throw InvalidArgumentException(HERE) << "in WrappedNormal: sigma must be of dimension " << d << ", got dimension=" << sigma.getDimension();
  if (period_ <= 0.0)
    throw InvalidArgumentException(HERE) << "in WrappedNormal: period must be > 0, got period=" << period_;

  // Check sigma is positive definite
  SymmetricMatrix sigma_sym(sigma);
  SquareMatrix sigmaEigVec(d);
  const Point sigmaEig = sigma_sym.computeEVInPlace(sigmaEigVec);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (sigmaEig[i] <= SpecFunc::ScalarEpsilon)
      throw InvalidArgumentException(HERE) << "in WrappedNormal: sigma must be positive definite";
  }

  sigma_ = sigma;
  // The wrapped mean belongs to the fundamental domain [-period/2, period/2]
  // where the distribution lives: wrap it here so that getMu and getMean are
  // consistent with the support of the distribution.
  mu_ = wrap(mu);
  setName("WrappedNormal");
  setDimension(d);
  computeNormalization();
  computeRange();
}

Bool WrappedNormal::operator ==(const WrappedNormal & other) const
{
  if (this == &other) return true;
  return (dimension_ == other.dimension_)
      && (mu_ == other.mu_)
      && (sigma_ == other.sigma_)
      && (period_ == other.period_);
}

Bool WrappedNormal::equals(const DistributionImplementation & other) const
{
  const WrappedNormal* p_other = dynamic_cast<const WrappedNormal*>(&other);
  return p_other && (*this == *p_other);
}

String WrappedNormal::__repr__() const
{
  OSS oss(true);
  oss << "class=" << WrappedNormal::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " mu=" << mu_
      << " sigma=" << sigma_
      << " period=" << period_;
  return oss;
}

String WrappedNormal::__str__(const String & ) const
{
  OSS oss(false);
  oss << getClassName() << "(mu = " << mu_ << ", sigma = " << sigma_ << ", period = " << period_ << ")";
  return oss;
}

WrappedNormal * WrappedNormal::clone() const
{
  return new WrappedNormal(*this);
}

void WrappedNormal::computeRange()
{
  const Scalar halfPeriod = period_ / 2.0;
  Point lower(dimension_, -halfPeriod);
  Point upper(dimension_, halfPeriod);
  setRange(Interval(lower, upper));
}

Point WrappedNormal::wrap(const Point & x) const
{
  Point result(dimension_);
  const Scalar halfP = period_ / 2.0;
  for (UnsignedInteger i = 0; i < dimension_; ++i)
  {
    Scalar val = x[i];
    // Wrap to [-period/2, period/2)
    val = std::fmod(val + halfP, period_);
    if (val < 0.0) val += period_;
    val -= halfP;
    result[i] = val;
  }
  return result;
}

void WrappedNormal::computeNormalization()
{
  const UnsignedInteger d = dimension_;

  // Normalization for wrapped normal:
  // The density is a sum over Z^d of Gaussians
  // p(x) = sum_{k in Z^d} (2*pi)^{-d/2} |sigma|^{-1/2}
  //        exp(-1/2 (x - mu + k*period)^T sigma^{-1} (x - mu + k*period))
  // The normalization is 1 because it's a probability density on the torus
  // (the sum integrates to 1 over the fundamental domain)

  // Compute log determinant and inverse of sigma
  SymmetricMatrix sigmaSym(sigma_);
  sigmaEigVec_ = SquareMatrix(d);
  sigmaEig_ = sigmaSym.computeEVInPlace(sigmaEigVec_);
  Scalar logDetSigma = 0.0;
  maxEig_ = 0.0;
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    logDetSigma += std::log(sigmaEig_[i]);
    maxEig_ = std::max(maxEig_, sigmaEig_[i]);
  }
  sigmaDet_ = std::exp(logDetSigma);
  sigmaInv_ = sigma_.inverse();

  // Log normalization constant for the base Gaussian
  logNormalization_ = 0.5 * d * std::log(2.0 * M_PI) + 0.5 * logDetSigma;

  // The wrapped normal sums over lattice points
  // For practical computation, we truncate the sum
  // The normalization is handled by the fact that we sum over k
}

Point WrappedNormal::getRealization() const
{
  const UnsignedInteger d = dimension_;

  // Sample from N(mu, sigma) in R^d, then wrap to torus
  Point z(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    z[i] = DistFunc::rNormal();

  // Apply sigma^{1/2} and add mu using cached eigendecomposition
  Point sample(d);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    sample[i] = mu_[i];
    for (UnsignedInteger j = 0; j < d; ++j)
      sample[i] += sigmaEigVec_(i, j) * std::sqrt(sigmaEig_[j]) * z[j];
  }

  // Wrap to torus
  return wrap(sample);
}

Scalar WrappedNormal::computePDF(const Point & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();

  const Scalar logPDF = computeLogPDF(point);
  return std::exp(logPDF);
}

Scalar WrappedNormal::computeLogPDF(const Point & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();

  const UnsignedInteger d = dimension_;
  const Scalar halfP = period_ / 2.0;

  // Sum over lattice points (truncated for numerical stability)
  // p(x) = sum_{k in Z^d} N(x + k*period; mu, sigma)
  // We sum over k in [-K, K]^d where K is chosen based on sigma

  // Choose K such that exp(-period^2 K^2 / (2 maxEig)) < SpecFunc::Precision
  // Using SpecFunc::Precision (2e-16) as the truncation tolerance
  const Scalar targetLog = std::log(SpecFunc::Precision);
  const UnsignedInteger K = std::max(static_cast<UnsignedInteger>(1),
                                     static_cast<UnsignedInteger>(std::ceil(std::sqrt(-2.0 * maxEig_ * targetLog) / period_)));

  // Compute sum of Gaussians
  Scalar logSum = -SpecFunc::Infinity;

  // For efficiency, use the full sum when the total number of terms is reasonable.
  // The total number of lattice points is (2*K+1)^d. We use the full sum when
  // this is <= maxLatticeTerms_, otherwise fall back to the k=0 approximation.
  // This handles both small d with large K and large d with small K correctly.
  const UnsignedInteger termsPerDim = 2 * K + 1;
  double totalTerms = 1.0;
  for (UnsignedInteger i = 0; i < d; ++i)
    totalTerms *= termsPerDim;

  if (totalTerms <= static_cast<double>(maxLatticeTerms_))
  {
    // Full sum over [-K, K]^d
    std::vector<Point> latticePoints;
    std::function<void(UnsignedInteger, Point&)> generateLattice = [&](UnsignedInteger dim, Point& k)
    {
      if (dim == d)
      {
        latticePoints.push_back(k);
        return;
      }
      for (int ki = -static_cast<int>(K); ki <= static_cast<int>(K); ++ki)
      {
        k[dim] = ki * period_;
        generateLattice(dim + 1, k);
      }
    };
    Point k(d);
    generateLattice(0, k);

    for (const Point& latticePoint : latticePoints)
    {
      Point diff(d);
      for (UnsignedInteger i = 0; i < d; ++i)
        diff[i] = point[i] - mu_[i] + latticePoint[i];

      // Quadratic form
      Scalar quad = 0.0;
      for (UnsignedInteger i = 0; i < d; ++i)
        for (UnsignedInteger j = 0; j < d; ++j)
          quad += diff[i] * sigmaInv_(i, j) * diff[j];

      const Scalar logTerm = -0.5 * quad - logNormalization_;
      if (logSum == -SpecFunc::Infinity)
        logSum = logTerm;
      else
        logSum = logSum + std::log1p(std::exp(logTerm - logSum));
    }
  }
  else
  {
    // Approximation: use only k=0 term wrapped to fundamental domain
    // This is only accurate when sigma is small compared to period
    OSS oss;
    oss << "WrappedNormal: number of lattice terms (" << totalTerms << ") exceeds WrappedNormal-MaxLatticeTerms (" << maxLatticeTerms_ << "), using k=0 approximation";
    LOGWARN(oss.str());
    Point diff(d);
    for (UnsignedInteger i = 0; i < d; ++i)
      diff[i] = point[i] - mu_[i];

    // Wrap diff to fundamental domain
    for (UnsignedInteger i = 0; i < d; ++i)
    {
      Scalar val = diff[i];
      val = std::fmod(val + halfP, period_);
      if (val < 0.0) val += period_;
      val -= halfP;
      diff[i] = val;
    }

    Scalar quad = 0.0;
    for (UnsignedInteger i = 0; i < d; ++i)
      for (UnsignedInteger j = 0; j < d; ++j)
        quad += diff[i] * sigmaInv_(i, j) * diff[j];

    logSum = -0.5 * quad - logNormalization_;
  }

  return logSum;
}

void WrappedNormal::computeMean() const
{
  // The wrapped normal is symmetric around the wrapped mean on the torus
  mean_ = mu_;
  isAlreadyComputedMean_ = true;
}

void WrappedNormal::computeCovariance() const
{
  // By convention the covariance of the wrapped variable is measured in the
  // tangent space at the mean, i.e. the covariance of the unwrapped Gaussian
  covariance_ = sigma_;
  isAlreadyComputedCovariance_ = true;
}

Scalar WrappedNormal::computeCDF(const Point & point) const
{
  if (point.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension_ << ", here dimension=" << point.getDimension();
  if (dimension_ != 1)
    return DistributionImplementation::computeCDF(point);

  // On the circle, the CDF over the fundamental domain is
  // F(x) = sum_{k in Z} [Phi((x - mu + k p)/sigma) - Phi((-p/2 - mu + k p)/sigma)]
  const Scalar halfP = period_ / 2.0;
  const Scalar x = point[0];
  const Scalar mu = mu_[0];
  const Scalar sigma = std::sqrt(sigma_(0, 0));
  // Choose K such that the tail of the normal is below SpecFunc::Precision
  const Scalar targetLog = std::log(SpecFunc::Precision);
  const UnsignedInteger K = std::max(static_cast<UnsignedInteger>(1),
                                     static_cast<UnsignedInteger>(std::ceil(std::sqrt(-2.0 * sigma * sigma * targetLog) / period_)));
  Scalar cdf = 0.0;
  for (int k = -static_cast<int>(K); k <= static_cast<int>(K); ++k)
  {
    const Scalar shift = static_cast<Scalar>(k) * period_;
    cdf += DistFunc::pNormal((x - mu + shift) / sigma)
           - DistFunc::pNormal((-halfP - mu + shift) / sigma);
  }
  return std::min(std::max(cdf, 0.0), 1.0);
}

Point WrappedNormal::getParameter() const
{
  const UnsignedInteger d = dimension_;
  Point point(d + d * d + 1);
  for (UnsignedInteger i = 0; i < d; ++i)
    point[i] = mu_[i];
  UnsignedInteger idx = d;
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
      point[idx++] = sigma_(i, j);
  point[idx] = period_;
  return point;
}

void WrappedNormal::setParameter(const Point & parameter)
{
  // Infer the dimension from the size d^2 + d + 1 of the parameter vector
  const UnsignedInteger size = parameter.getSize();
  const Scalar root = (std::sqrt(static_cast<Scalar>(1 + 4 * (size - 1))) - 1.0) / 2.0;
  const UnsignedInteger d = static_cast<UnsignedInteger>(std::round(root));
  if (d * (d + 1) + 1 != size)
    throw InvalidArgumentException(HERE) << "Error: expected a parameter vector of size d^2 + d + 1 for some d >= 1, got size=" << size;

  Point mu(d);
  CovarianceMatrix sigma(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    mu[i] = parameter[i];
  UnsignedInteger idx = d;
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
      sigma(i, j) = parameter[idx++];
  const Scalar period = parameter[idx];

  const Scalar w = getWeight();
  *this = WrappedNormal(mu, sigma, period);
  setWeight(w);
}

Description WrappedNormal::getParameterDescription() const
{
  const UnsignedInteger d = dimension_;
  Description description(d + d * d + 1);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    OSS oss;
    oss << "mu_" << i;
    description[i] = oss.str();
  }
  UnsignedInteger idx = d;
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
    {
      OSS oss;
      oss << "sigma_" << i << "_" << j;
      description[idx++] = oss.str();
    }
  description[idx] = "period";
  return description;
}

void WrappedNormal::setMu(const Point & mu)
{
  const UnsignedInteger d = dimension_;
  if (mu.getDimension() != d)
    throw InvalidArgumentException(HERE) << "in WrappedNormal::setMu: mu must have dimension " << d << ", got dimension=" << mu.getDimension();

  if (mu != mu_)
  {
    mu_ = wrap(mu);
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
  }
}

Point WrappedNormal::getMu() const
{
  return mu_;
}

void WrappedNormal::setSigma(const CovarianceMatrix & sigma)
{
  const UnsignedInteger d = dimension_;
  if (sigma.getDimension() != d)
    throw InvalidArgumentException(HERE) << "in WrappedNormal::setSigma: sigma must be of dimension " << d << ", got dimension=" << sigma.getDimension();

  SymmetricMatrix sigma_sym(sigma);
  SquareMatrix sigmaEigVec(d);
  const Point sigmaEig = sigma_sym.computeEVInPlace(sigmaEigVec);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (sigmaEig[i] <= SpecFunc::ScalarEpsilon)
      throw InvalidArgumentException(HERE) << "in WrappedNormal::setSigma: sigma must be positive definite";
  }

  if (sigma != sigma_)
  {
    sigma_ = sigma;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeNormalization();
  }
}

CovarianceMatrix WrappedNormal::getSigma() const
{
  return sigma_;
}

void WrappedNormal::setPeriod(const Scalar period)
{
  if (period <= 0.0)
    throw InvalidArgumentException(HERE) << "in WrappedNormal::setPeriod: period must be > 0, got period=" << period;

  if (period != period_)
  {
    period_ = period;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
    computeRange();
  }
}

Scalar WrappedNormal::getPeriod() const
{
  return period_;
}

void WrappedNormal::setMaxLatticeTerms(const UnsignedInteger maxTerms)
{
  if (maxTerms != maxLatticeTerms_)
  {
    maxLatticeTerms_ = maxTerms;
    isAlreadyComputedMean_ = false;
    isAlreadyComputedCovariance_ = false;
  }
}

UnsignedInteger WrappedNormal::getMaxLatticeTerms() const
{
  return maxLatticeTerms_;
}

Scalar WrappedNormal::computeEntropy() const
{
  // Entropy approximation for wrapped normal
  // H = 0.5 * d * (1 + log(2*pi)) + 0.5 * log|sigma| - log(sum_k exp(-1/2 k^T period^2 sigma^{-1} k))
  const UnsignedInteger d = dimension_;
  return 0.5 * d * (1.0 + std::log(2.0 * M_PI)) + 0.5 * std::log(sigmaDet_);
}

Bool WrappedNormal::isContinuous() const
{
  return true;
}

void WrappedNormal::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("mu_", mu_);
  adv.saveAttribute("sigma_", sigma_);
  adv.saveAttribute("period_", period_);
  adv.saveAttribute("logNormalization_", logNormalization_);
  adv.saveAttribute("sigmaInv_", sigmaInv_);
  adv.saveAttribute("sigmaDet_", sigmaDet_);
}

void WrappedNormal::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("mu_", mu_);
  adv.loadAttribute("sigma_", sigma_);
  adv.loadAttribute("period_", period_);
  adv.loadAttribute("logNormalization_", logNormalization_);
  adv.loadAttribute("sigmaInv_", sigmaInv_);
  adv.loadAttribute("sigmaDet_", sigmaDet_);
  computeRange();
}

END_NAMESPACE_OPENTURNS