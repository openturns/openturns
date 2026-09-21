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

namespace {

// Enumerate the integer vectors m with (m - center)^T A (m - center) <= radiusSquare
// by Cholesky recursion (Fincke-Pohst), where lower holds the lower Cholesky
// factor of the SPD matrix A. At most budget points are collected.
void EnumerateEllipsoidLevel(const SquareMatrix & lower,
                             const Point & center,
                             const Scalar radiusSquare,
                             const UnsignedInteger budget,
                             Point & current,
                             const UnsignedInteger level,
                             const Scalar doneSquare,
                             std::vector<Point> & pointsOut,
                             Bool & truncated)
{
  const UnsignedInteger d = center.getDimension();
  if (truncated) return;
  Scalar shift = 0.0;
  for (UnsignedInteger k = level + 1; k < d; ++k)
    shift += lower(k, level) * (current[k] - center[k]);
  const Scalar remaining = radiusSquare - doneSquare;
  if (remaining < 0.0) return;
  const Scalar root = std::sqrt(remaining);
  const Scalar yLow = (-root - shift) / lower(level, level);
  const Scalar yHigh = (root - shift) / lower(level, level);
  const long long mLow = static_cast<long long>(std::ceil(center[level] + yLow));
  const long long mHigh = static_cast<long long>(std::floor(center[level] + yHigh));
  for (long long m = mLow; m <= mHigh; ++m)
  {
    current[level] = static_cast<Scalar>(m);
    const Scalar residual = lower(level, level) * (current[level] - center[level]) + shift;
    if (level == 0)
    {
      pointsOut.push_back(current);
      if (pointsOut.size() > budget)
      {
        truncated = true;
        return;
      }
    }
    else
      EnumerateEllipsoidLevel(lower, center, radiusSquare, budget, current, level - 1, doneSquare + residual * residual, pointsOut, truncated);
  }
}

// Log-volume of the ellipsoid {m : (m - center)^T A (m - center) <= radiusSquare}
Scalar LogEllipsoidVolume(const UnsignedInteger d,
                          const Scalar logDetA,
                          const Scalar radiusSquare)
{
  return 0.5 * static_cast<Scalar>(d) * std::log(M_PI) - SpecFunc::LogGamma(0.5 * static_cast<Scalar>(d) + 1.0)
         + 0.5 * static_cast<Scalar>(d) * std::log(radiusSquare) - 0.5 * logDetA;
}

// Gauss-Hermite nodes and weights for the N(0,1) expectation by Golub-Welsch:
// the Jacobi matrix has a zero diagonal and sqrt(i) off-diagonal entries,
// its eigenvalues are the nodes and the weights follow from the first row
// of the eigenvector matrix
void GaussHermiteRule(const UnsignedInteger order,
                      Point & nodes,
                      Point & weights)
{
  SymmetricMatrix jacobi(order);
  for (UnsignedInteger i = 1; i < order; ++i)
    jacobi(i, i - 1) = std::sqrt(static_cast<Scalar>(i));
  SquareMatrix vectors(order);
  const Point eigenvalues = jacobi.computeEVInPlace(vectors);
  nodes = eigenvalues;
  weights = Point(order);
  for (UnsignedInteger k = 0; k < order; ++k)
    weights[k] = std::sqrt(SpecFunc::TWOPI) * vectors(0, k) * vectors(0, k);
}

// Tensor Gauss-Hermite order from a point budget: the largest order with
// order^dimension <= budget, at least 2, at most budget^{1/3} to keep the
// Golub-Welsch eigendecomposition negligible
UnsignedInteger GaussHermiteOrder(const UnsignedInteger dimension,
                                  const UnsignedInteger budget)
{
  const Scalar maxOrder = std::cbrt(static_cast<Scalar>(budget));
  const Scalar tensorOrder = std::pow(static_cast<Scalar>(budget), 1.0 / static_cast<Scalar>(dimension));
  return std::max(static_cast<UnsignedInteger>(2),
                  static_cast<UnsignedInteger>(std::min(maxOrder, tensorOrder)));
}

} // anonymous namespace

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
  for (UnsignedInteger i = 0; i < d; ++i)
    logDetSigma += std::log(sigmaEig_[i]);
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

  // Sum over lattice points (truncated for numerical stability)
  // p(x) = sum_{k in Z^d} N(x + k*period; mu, sigma)
  // We sum over k in [-K, K]^d where K is chosen based on sigma

  // Compute sum of Gaussians over an ellipsoidal lattice set, or over the
  // dual Fourier ellipsoid when it holds fewer points. Both truncations keep
  // the terms above SpecFunc::Precision relative to the peak term, so the
  // uniform fallback below only triggers when neither side is affordable.
  const Scalar cut = -2.0 * std::log(SpecFunc::Precision);
  Scalar logDetSigma = 0.0;
  for (UnsignedInteger i = 0; i < d; ++i)
    logDetSigma += std::log(sigmaEig_[i]);
  // Direct ellipsoid: (x - mu + k p)^T Sigma^{-1} (x - mu + k p) <= cut,
  // i.e. (k - c)^T A (k - c) <= cut with A = p^2 Sigma^{-1}, c = (mu - x)/p
  const Scalar logDetDirect = 2.0 * static_cast<Scalar>(d) * std::log(period_) - logDetSigma;
  const Scalar logCountDirect = LogEllipsoidVolume(d, logDetDirect, cut);
  // Fourier ellipsoid: (2 pi m / p)^T Sigma (2 pi m / p) <= cut
  const Scalar logDetFourier = 2.0 * static_cast<Scalar>(d) * std::log(SpecFunc::TWOPI / period_) + logDetSigma;
  const Scalar logCountFourier = LogEllipsoidVolume(d, logDetFourier, cut);
  const Scalar logBudget = std::log(static_cast<Scalar>(maxLatticeTerms_));
  if (std::min(logCountDirect, logCountFourier) > logBudget)
  {
    // Even the cheapest ellipsoidal sum requires too many terms: the density
    // is essentially uniform on the torus, so use the uniform limit 1/period^d
    // which is properly normalized.
    OSS oss;
    oss << "WrappedNormal: ellipsoidal lattice sums exceed WrappedNormal-MaxLatticeTerms (" << maxLatticeTerms_ << "), using the uniform limit";
    LOGWARN(oss.str());
    return -static_cast<Scalar>(d) * std::log(period_);
  }

  Scalar logSum = -SpecFunc::Infinity;
  if (logCountDirect <= logCountFourier)
  {
    // Direct sum over the k-ellipsoid
    SymmetricMatrix shapeSym(d);
    for (UnsignedInteger i = 0; i < d; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
        shapeSym(i, j) = period_ * period_ * sigmaInv_(i, j);
    CovarianceMatrix shapeCov(d);
    for (UnsignedInteger i = 0; i < d; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
        shapeCov(i, j) = shapeSym(i, j);
    const TriangularMatrix chol(shapeCov.computeCholesky());
    SquareMatrix lower(d);
    for (UnsignedInteger i = 0; i < d; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
        lower(i, j) = chol(i, j);
    Point center(d);
    for (UnsignedInteger i = 0; i < d; ++i)
      center[i] = (mu_[i] - point[i]) / period_;
    std::vector<Point> latticePoints;
    Point current(d);
    Bool truncated = false;
    EnumerateEllipsoidLevel(lower, center, cut, maxLatticeTerms_, current, d - 1, 0.0, latticePoints, truncated);
    if (truncated)
    {
      OSS oss;
      oss << "WrappedNormal: direct lattice enumeration exceeds WrappedNormal-MaxLatticeTerms (" << maxLatticeTerms_ << "), using the uniform limit";
      LOGWARN(oss.str());
      return -static_cast<Scalar>(d) * std::log(period_);
    }
    for (UnsignedInteger t = 0; t < latticePoints.size(); ++t)
    {
      const Point latticePoint(latticePoints[t]);
      Point diff(d);
      for (UnsignedInteger i = 0; i < d; ++i)
        diff[i] = point[i] - mu_[i] + latticePoint[i] * period_;

      // Quadratic form
      Scalar quad = 0.0;
      for (UnsignedInteger i = 0; i < d; ++i)
        for (UnsignedInteger j = 0; j < d; ++j)
          quad += diff[i] * sigmaInv_(i, j) * diff[j];

      const Scalar logTerm = -0.5 * quad - logNormalization_;
      if (logSum == -SpecFunc::Infinity)
        logSum = logTerm;
      else
        logSum = std::max(logSum, logTerm) + std::log1p(std::exp(-std::abs(logTerm - logSum)));
    }
  }
  else
  {
    // Dual Fourier series over the m-ellipsoid:
    // p(x) = (1/period^d) sum_m exp(-(2*pi*m/period)^T sigma (2*pi*m/period)/2) cos(2*pi*m.(x-mu)/period)
    SymmetricMatrix shapeSym(d);
    const Scalar omega = SpecFunc::TWOPI / period_;
    for (UnsignedInteger i = 0; i < d; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
        shapeSym(i, j) = omega * omega * sigma_(i, j);
    CovarianceMatrix shapeCov(d);
    for (UnsignedInteger i = 0; i < d; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
        shapeCov(i, j) = shapeSym(i, j);
    const TriangularMatrix chol(shapeCov.computeCholesky());
    SquareMatrix lower(d);
    for (UnsignedInteger i = 0; i < d; ++i)
      for (UnsignedInteger j = 0; j <= i; ++j)
        lower(i, j) = chol(i, j);
    const Point center(d);
    std::vector<Point> latticePoints;
    Point current(d);
    Bool truncated = false;
    EnumerateEllipsoidLevel(lower, center, cut, maxLatticeTerms_, current, d - 1, 0.0, latticePoints, truncated);
    if (truncated)
    {
      OSS oss;
      oss << "WrappedNormal: Fourier lattice enumeration exceeds WrappedNormal-MaxLatticeTerms (" << maxLatticeTerms_ << "), using the uniform limit";
      LOGWARN(oss.str());
      return -static_cast<Scalar>(d) * std::log(period_);
    }
    const Scalar omegaSquare = omega * omega;
    Scalar sum = 0.0;
    for (UnsignedInteger t = 0; t < latticePoints.size(); ++t)
    {
      const Point mode(latticePoints[t]);
      // Quadratic form m^T sigma m
      Scalar quad = 0.0;
      for (UnsignedInteger i = 0; i < d; ++i)
        for (UnsignedInteger j = 0; j < d; ++j)
          quad += mode[i] * sigma_(i, j) * mode[j];
      // Phase 2*pi*m.(x-mu)/period
      Scalar phase = 0.0;
      for (UnsignedInteger i = 0; i < d; ++i)
        phase += mode[i] * (point[i] - mu_[i]);
      sum += std::exp(-0.5 * omegaSquare * quad) * std::cos(omega * phase);
    }
    if (!(sum > 0.0))
    {
      // Truncation drove the alternating sum non-positive in the deep tails:
      // the direct sum is the right tool there
      OSS oss;
      oss << "WrappedNormal: Fourier sum is non-positive, this point is out of scope";
      LOGWARN(oss.str());
      return -SpecFunc::Infinity;
    }
    logSum = std::log(sum) - static_cast<Scalar>(d) * std::log(period_);
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
  // The covariance of the wrapped variable on the fundamental domain:
  // E[XX^T] - E[X]E[X]^T with X = wrap(Y), Y ~ N(mu, sigma), by tensor
  // Gauss-Hermite quadrature over the standard normal antecedent
  const UnsignedInteger d = dimension_;
  const UnsignedInteger budget = ResourceMap::GetAsUnsignedInteger("WrappedNormal-GaussHermiteMaximumPoints");
  const UnsignedInteger order = GaussHermiteOrder(d, budget);
  Point nodes;
  Point weights;
  GaussHermiteRule(order, nodes, weights);
  // Square root of sigma from the cached eigendecomposition
  SquareMatrix root(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
      root(i, j) = sigmaEigVec_(i, j) * std::sqrt(sigmaEig_[j]);
  Point mean(d, 0.0);
  SquareMatrix second(d);
  Scalar totalWeight = 0.0;
  std::vector<UnsignedInteger> counter(d, 0);
  const UnsignedInteger total = static_cast<UnsignedInteger>(std::pow(static_cast<Scalar>(order), static_cast<Scalar>(d)));
  for (UnsignedInteger t = 0; t < total; ++t)
  {
    Scalar weight = 1.0;
    Point normal(d);
    for (UnsignedInteger i = 0; i < d; ++i)
    {
      weight *= weights[counter[i]];
      normal[i] = nodes[counter[i]];
    }
    Point gaussian(d);
    for (UnsignedInteger i = 0; i < d; ++i)
    {
      gaussian[i] = mu_[i];
      for (UnsignedInteger j = 0; j < d; ++j)
        gaussian[i] += root(i, j) * normal[j];
    }
    const Point point(wrap(gaussian));
    for (UnsignedInteger i = 0; i < d; ++i)
    {
      mean[i] += weight * point[i];
      for (UnsignedInteger j = 0; j < d; ++j)
        second(i, j) += weight * point[i] * point[j];
    }
    totalWeight += weight;
    for (UnsignedInteger i = 0; i < d; ++i)
    {
      if (++counter[i] < order) break;
      counter[i] = 0;
    }
  }
  CovarianceMatrix covariance(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
      covariance(i, j) = second(i, j) / totalWeight - (mean[i] / totalWeight) * (mean[j] / totalWeight);
  covariance_ = covariance;
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
  // H(X) = E_{Z ~ N(0,I)}[-logPDF(wrap(mu + A Z))] with A = V sqrt(Lambda)
  // from the cached eigendecomposition, by tensor Gauss-Hermite quadrature
  const UnsignedInteger d = dimension_;
  const UnsignedInteger budget = ResourceMap::GetAsUnsignedInteger("WrappedNormal-GaussHermiteMaximumPoints");
  const UnsignedInteger order = GaussHermiteOrder(d, budget);
  Point nodes;
  Point weights;
  GaussHermiteRule(order, nodes, weights);
  SquareMatrix root(d);
  for (UnsignedInteger i = 0; i < d; ++i)
    for (UnsignedInteger j = 0; j < d; ++j)
      root(i, j) = sigmaEigVec_(i, j) * std::sqrt(sigmaEig_[j]);
  Scalar entropy = 0.0;
  Scalar totalWeight = 0.0;
  std::vector<UnsignedInteger> counter(d, 0);
  const UnsignedInteger total = static_cast<UnsignedInteger>(std::pow(static_cast<Scalar>(order), static_cast<Scalar>(d)));
  for (UnsignedInteger t = 0; t < total; ++t)
  {
    Scalar weight = 1.0;
    Point normal(d);
    for (UnsignedInteger i = 0; i < d; ++i)
    {
      weight *= weights[counter[i]];
      normal[i] = nodes[counter[i]];
    }
    Point gaussian(d);
    for (UnsignedInteger i = 0; i < d; ++i)
    {
      gaussian[i] = mu_[i];
      for (UnsignedInteger j = 0; j < d; ++j)
        gaussian[i] += root(i, j) * normal[j];
    }
    const Scalar logPDF = computeLogPDF(wrap(gaussian));
    if (std::isfinite(logPDF))
    {
      entropy -= weight * logPDF;
      totalWeight += weight;
    }
    for (UnsignedInteger i = 0; i < d; ++i)
    {
      if (++counter[i] < order) break;
      counter[i] = 0;
    }
  }
  return entropy / totalWeight;
}

Bool WrappedNormal::isContinuous() const
{
  return true;
}

void WrappedNormal::save(Advocate & adv) const
{
  DistributionImplementation::save(adv);
  adv.saveAttribute("mu_", mu_);
  adv.saveAttribute("sigma_", sigma_);
  adv.saveAttribute("period_", period_);
  adv.saveAttribute("maxLatticeTerms_", maxLatticeTerms_);
  adv.saveAttribute("logNormalization_", logNormalization_);
  adv.saveAttribute("sigmaInv_", sigmaInv_);
  adv.saveAttribute("sigmaDet_", sigmaDet_);
}

void WrappedNormal::load(Advocate & adv)
{
  DistributionImplementation::load(adv);
  adv.loadAttribute("mu_", mu_);
  adv.loadAttribute("sigma_", sigma_);
  adv.loadAttribute("period_", period_);
  // The class shadows the base dimension_ member: restore it from mu_
  dimension_ = mu_.getDimension();
  if (adv.hasAttribute("maxLatticeTerms_"))
    adv.loadAttribute("maxLatticeTerms_", maxLatticeTerms_);
  else
    maxLatticeTerms_ = ResourceMap::GetAsUnsignedInteger("WrappedNormal-MaxLatticeTerms");
  adv.loadAttribute("logNormalization_", logNormalization_);
  adv.loadAttribute("sigmaInv_", sigmaInv_);
  adv.loadAttribute("sigmaDet_", sigmaDet_);
  // Regenerate the diagonalization cache and the range
  computeNormalization();
  computeRange();
}

END_NAMESPACE_OPENTURNS