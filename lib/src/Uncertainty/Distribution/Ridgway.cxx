//                                               -*- C++ -*-
/**
 *  @brief Ridgway algorithm (SMC) to compute orthant probabilities of
 *         multivariate Normal distributions.
 *
 *  Implements Algorithm 3 from:
 *    Ridgway, J. (2016). Computation of Gaussian orthant probabilities in
 *    high dimension. Statistics and Computing, 26(4), 899-916.
 *    https://arxiv.org/abs/1411.1314
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
#include <vector>
#include <algorithm>

#include "openturns/Ridgway.hxx"
#include "openturns/Exception.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/LowDiscrepancySequence.hxx"
#include "openturns/SobolSequence.hxx"
#include "openturns/HaltonSequence.hxx"
#include "openturns/ReverseHaltonSequence.hxx"
#include "openturns/FaureSequence.hxx"
#include "openturns/HaselgroveSequence.hxx"
#include "openturns/TBBImplementation.hxx"
#include "openturns/TriangularMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

namespace {

// Draw from N(0,1) truncated to [lower, upper] using inverse CDF
Scalar truncatedNormalDraw(const Scalar lower, const Scalar upper)
{
  if (!(upper > lower)) return 0.0;
  const Scalar ei = DistFunc::pNormal(lower);
  const Scalar fi = DistFunc::pNormal(upper);
  if (!(fi > ei)) return 0.0;
  return DistFunc::qNormal(ei + (fi - ei) * RandomGenerator::Generate());
}

// Systematic resampling (Algorithm 4 in the paper)
// Given normalized weights (summing to 1), returns ancestor indices
void systematicResampling(const Point& normWeights,
                          std::vector<UnsignedInteger>& indices)
{
  const UnsignedInteger M = normWeights.getSize();
  Point cumulative(M);
  cumulative[0] = normWeights[0];
  for (UnsignedInteger i = 1; i < M; ++i)
    cumulative[i] = cumulative[i - 1] + normWeights[i];

  const Scalar step = 1.0 / M;
  const Scalar u = RandomGenerator::Generate() * step;

  UnsignedInteger j = 0;
  for (UnsignedInteger i = 0; i < M; ++i)
  {
    const Scalar threshold = u + i * step;
    while (cumulative[j] < threshold && j < M - 1)
      ++j;
    indices[i] = j;
  }
}

// One Gibbs sweep for particle eta at time t (Section 4.3.1)
// Updates all components 0..t-1 of eta in place
void gibbsMove(Point& eta,
               const TriangularMatrix& L,
               const Point& a,
               const Point& b,
               const UnsignedInteger t)
{
  for (UnsignedInteger i = 0; i < t; ++i)
  {
    Scalar lower = SpecFunc::LowestScalar;
    Scalar upper = SpecFunc::Infinity;

    // Intersect constraints from dimensions k = i, ..., t-1
    for (UnsignedInteger k = i; k < t; ++k)
    {
      // Compute sum_{j < k, j != i} L(k,j) * eta[j]
      Scalar sum = 0.0;
      for (UnsignedInteger j = 0; j < k; ++j)
      {
        if (j != i) sum += L(k, j) * eta[j];
      }

      const Scalar lk = (a[k] - sum) / L(k, i);
      const Scalar uk = (b[k] - sum) / L(k, i);

      if (L(k, i) > 0.0)
      {
        if (lk > lower) lower = lk;
        if (uk < upper) upper = uk;
      }
      else if (L(k, i) < 0.0)
      {
        if (uk > lower) lower = uk;
        if (lk < upper) upper = lk;
      }
    }

    if (lower < upper)
      eta[i] = truncatedNormalDraw(lower, upper);
  }
}

// Compute log(sum_i exp(logWeights[i])) with the log-sum-exp trick
Scalar logSumExp(const Point& logWeights)
{
  const UnsignedInteger M = logWeights.getSize();
  Scalar maxVal = logWeights[0];
  for (UnsignedInteger i = 1; i < M; ++i)
    if (logWeights[i] > maxVal) maxVal = logWeights[i];

  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < M; ++i)
    sum += std::exp(logWeights[i] - maxVal);

  return maxVal + std::log(sum);
}

} // anonymous namespace

namespace
{
LowDiscrepancySequence GetRidgwaySequence(const UnsignedInteger dimension)
{
  const String name(ResourceMap::GetAsString("Ridgway-LowDiscrepancySequence"));
  if (name == "HaltonSequence") return HaltonSequence(dimension);
  if (name == "ReverseHaltonSequence") return ReverseHaltonSequence(dimension);
  if (name == "FaureSequence") return FaureSequence(dimension);
  if (name == "HaselgroveSequence") return HaselgroveSequence(dimension);
  // default: SobolSequence, also handles "SobolSequence", with HaltonSequence as a fallback if the dimension is too large for SobolSequence
  if (dimension <= SobolSequence::MaximumDimension)
    return SobolSequence(dimension, "MULTIDIGIT");
  return HaltonSequence(dimension, "RANDOM");
}
} // anonymous namespace

Scalar mvn_orthant_probability(
    const Point& a, const Point& b,
    const Point& mu, const TriangularMatrix& L,
    const UnsignedInteger M,
    const Scalar alpha)
{
  const UnsignedInteger d = L.getDimension();

  if (d == 0)
    throw InvalidDimensionException(HERE) << "Dimension must be at least 1, here d=0";
  if (a.getDimension() != d)
    throw InvalidDimensionException(HERE) << "Lower bound a has dimension "
        << a.getDimension() << ", expected " << d;
  if (b.getDimension() != d)
    throw InvalidDimensionException(HERE) << "Upper bound b has dimension "
        << b.getDimension() << ", expected " << d;
  if (mu.getDimension() != d)
    throw InvalidDimensionException(HERE) << "Mean mu has dimension "
        << mu.getDimension() << ", expected " << d;
  if (M == 0)
    throw InvalidArgumentException(HERE) << "Number of particles must be positive, here M=0";
  if (!(alpha > 0.0))
    throw InvalidArgumentException(HERE) << "ESS threshold must be positive, here alpha=" << alpha;

  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (a[i] > b[i])
      throw InvalidArgumentException(HERE) << "Lower bound a[" << i << "]=" << a[i]
          << " exceeds upper bound b[" << i << "]=" << b[i];
    if (L(i, i) <= 0.0)
      throw InvalidArgumentException(HERE) << "Cholesky diagonal L(" << i << "," << i
          << ")=" << L(i, i) << " must be positive";
  }

  // Center the bounds
  Point a0(d), b0(d);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    a0[i] = a[i] - mu[i];
    b0[i] = b[i] - mu[i];
  }

  // 1D case: direct computation
  if (d == 1)
  {
    return DistFunc::pNormal(b0[0] / L(0, 0))
         - DistFunc::pNormal(a0[0] / L(0, 0));
  }

  // Particle system: M particles of dimension d
  std::vector<Point> particles(M, Point(d, 0.0));
  Point logWeights(M, 0.0);

  // Algorithm 3: Initialize first dimension
  // The normalizing constant of π_1 is Z_1 = Φ(B_1)
  const Scalar initProb = DistFunc::pNormal(b0[0] / L(0, 0))
                        - DistFunc::pNormal(a0[0] / L(0, 0));
  if (!(initProb > 0.0)) return 0.0;
  Scalar logZ = std::log(initProb);
  struct InitParticlesFunctor {
    std::vector<Point>& particles_;
    const TriangularMatrix& L_;
    const Point& a0_;
    const Point& b0_;
    InitParticlesFunctor(std::vector<Point>& particles, const TriangularMatrix& L, const Point& a0, const Point& b0)
      : particles_(particles)
      , L_(L)
      , a0_(a0)
      , b0_(b0) {}
    void operator()(const TBBImplementation::BlockedRange<UnsignedInteger>& r) const {
      for (UnsignedInteger m = r.begin(); m != r.end(); ++m) {
        const Scalar lower = a0_[0] / L_(0, 0);
        const Scalar upper = b0_[0] / L_(0, 0);
        if (upper > lower)
          particles_[m][0] = truncatedNormalDraw(lower, upper);
      }
    }
  };
  TBBImplementation::ParallelFor(0, M, InitParticlesFunctor(particles, L, a0, b0));

  // Iterate over remaining dimensions
  for (UnsignedInteger t = 1; t < d; ++t)
  {
    // Compute normalized weights and ESS
    const Scalar logSum = logSumExp(logWeights);
    Scalar sumNormSq = 0.0;
    for (UnsignedInteger i = 0; i < M; ++i)
    {
      const Scalar w = std::exp(logWeights[i] - logSum);
      sumNormSq += w * w;
    }
    const Scalar essRatio = 1.0 / (static_cast<Scalar>(M) * sumNormSq);

    // Resample if ESS falls below threshold alpha
    if (essRatio < alpha)
    {
      // Update marginal likelihood estimate
      logZ += logSum - std::log(static_cast<Scalar>(M));

      // Normalize weights for resampling
      Point normWeights(M);
      for (UnsignedInteger i = 0; i < M; ++i)
        normWeights[i] = std::exp(logWeights[i] - logSum);

      // Systematic resampling
      std::vector<UnsignedInteger> ancestors(M);
      systematicResampling(normWeights, ancestors);

      // Copy resampled particles
      std::vector<Point> newParticles(M, Point(d, 0.0));
      for (UnsignedInteger i = 0; i < M; ++i)
        newParticles[i] = particles[ancestors[i]];
      particles = newParticles;

      // Reset weights
      std::fill(logWeights.begin(), logWeights.end(), 0.0);

      // Gibbs moves to rejuvenate particles
      struct GibbsMoveFunctor {
        std::vector<Point>& particles_;
        const TriangularMatrix& L_;
        const Point& a0_;
        const Point& b0_;
        const UnsignedInteger t_;
        GibbsMoveFunctor(std::vector<Point>& particles, const TriangularMatrix& L, const Point& a0, const Point& b0, const UnsignedInteger t)
          : particles_(particles)
          , L_(L)
          , a0_(a0)
          , b0_(b0)
          , t_(t) {}
        void operator()(const TBBImplementation::BlockedRange<UnsignedInteger>& r) const {
          for (UnsignedInteger i = r.begin(); i != r.end(); ++i)
            gibbsMove(particles_[i], L_, a0_, b0_, t_);
        }
      };
      TBBImplementation::ParallelFor(0, M, GibbsMoveFunctor(particles, L, a0, b0, t));
    }

    // Extend to dimension t (Algorithm 3 inner loop)
    struct ExtendDimensionFunctor {
      std::vector<Point>& particles_;
      Point& logWeights_;
      const TriangularMatrix& L_;
      const Point& a0_;
      const Point& b0_;
      const UnsignedInteger t_;
      ExtendDimensionFunctor(std::vector<Point>& particles, Point& logWeights, const TriangularMatrix& L, const Point& a0, const Point& b0, const UnsignedInteger t)
        : particles_(particles)
        , logWeights_(logWeights)
        , L_(L)
        , a0_(a0)
        , b0_(b0)
        , t_(t) {}
      void operator()(const TBBImplementation::BlockedRange<UnsignedInteger>& r) const {
        for (UnsignedInteger m = r.begin(); m != r.end(); ++m) {
          Scalar y = 0.0;
          for (UnsignedInteger j = 0; j < t_; ++j)
            y += L_(t_, j) * particles_[m][j];
          const Scalar lower = (a0_[t_] - y) / L_(t_, t_);
          const Scalar upper = (b0_[t_] - y) / L_(t_, t_);
          if (upper > lower) {
            const Scalar prob = DistFunc::pNormal(upper) - DistFunc::pNormal(lower);
            logWeights_[m] += std::log(prob);
            particles_[m][t_] = truncatedNormalDraw(lower, upper);
          } else {
            logWeights_[m] = SpecFunc::LowestScalar;
          }
        }
      }
    };
    TBBImplementation::ParallelFor(0, M, ExtendDimensionFunctor(particles, logWeights, L, a0, b0, t));
  }

  // Final estimate: Z * (1/M) * sum(w_T)
  const Scalar logSum = logSumExp(logWeights);
  const Scalar logResult = logZ + logSum - std::log(static_cast<Scalar>(M));
  return SpecFunc::Clip01(std::exp(logResult));
}

Scalar mvn_orthant_probability(
    const Point& a, const Point& b,
    const Point& mu, const CovarianceMatrix& sigma,
    const UnsignedInteger M,
    const Scalar alpha)
{
  return mvn_orthant_probability(a, b, mu, sigma.computeCholesky(), M, alpha);
}

Scalar mvn_orthant_probability(
    const Point& a, const Point& b,
    const TriangularMatrix& L,
    const UnsignedInteger M,
    const Scalar alpha)
{
  const UnsignedInteger d = L.getDimension();
  return mvn_orthant_probability(a, b, Point(d, 0.0), L, M, alpha);
}

Scalar mvn_orthant_probability(
    const Point& a, const Point& b,
    const CovarianceMatrix& sigma,
    const UnsignedInteger M,
    const Scalar alpha)
{
  return mvn_orthant_probability(a, b, Point(sigma.getDimension(), 0.0), sigma, M, alpha);
}

Scalar mvt_orthant_probability(
    const Point& a, const Point& b,
    const Point& mu, const TriangularMatrix& L,
    const Scalar nu,
    const UnsignedInteger M,
    const Scalar alpha,
    const UnsignedInteger N)
{
  const UnsignedInteger d = L.getDimension();

  if (d == 0)
    throw InvalidDimensionException(HERE) << "Dimension must be at least 1, here d=0";
  if (a.getDimension() != d)
    throw InvalidDimensionException(HERE) << "Lower bound a has dimension "
        << a.getDimension() << ", expected " << d;
  if (b.getDimension() != d)
    throw InvalidDimensionException(HERE) << "Upper bound b has dimension "
        << b.getDimension() << ", expected " << d;
  if (mu.getDimension() != d)
    throw InvalidDimensionException(HERE) << "Mean mu has dimension "
        << mu.getDimension() << ", expected " << d;
  if (!(nu > 0.0))
    throw InvalidArgumentException(HERE) << "Degrees of freedom must be positive, here nu=" << nu;
  if (M == 0)
    throw InvalidArgumentException(HERE) << "Number of particles must be positive, here M=0";
  if (!(alpha > 0.0))
    throw InvalidArgumentException(HERE) << "ESS threshold must be positive, here alpha=" << alpha;
  if (N == 0)
    throw InvalidArgumentException(HERE) << "Number of Student samples must be positive, here N=0";

  for (UnsignedInteger i = 0; i < d; ++i)
  {
    if (a[i] > b[i])
      throw InvalidArgumentException(HERE) << "Lower bound a[" << i << "]=" << a[i]
          << " exceeds upper bound b[" << i << "]=" << b[i];
    if (L(i, i) <= 0.0)
      throw InvalidArgumentException(HERE) << "Cholesky diagonal L(" << i << "," << i
          << ")=" << L(i, i) << " must be positive";
  }

  // Center the bounds
  Point a0(d), b0(d);
  for (UnsignedInteger i = 0; i < d; ++i)
  {
    a0[i] = a[i] - mu[i];
    b0[i] = b[i] - mu[i];
  }

  // 1D case: direct computation using Student-t CDF
  if (d == 1)
  {
    return DistFunc::pStudent(nu, b0[0] / L(0, 0))
         - DistFunc::pStudent(nu, a0[0] / L(0, 0));
  }

  // Generate low discrepancy points for the Student outer integration
  const LowDiscrepancySequence seq(GetRidgwaySequence(1));
  const Sample sobolPoints(seq.generate(N));

  struct Reduce
  {
    const Sample& sobolPoints_;
    const Point& a0_;
    const Point& b0_;
    const TriangularMatrix& L_;
    const UnsignedInteger M_;
    const Scalar alpha_;
    const UnsignedInteger d_;
    const Scalar nu_;
    Scalar sum_;

    Reduce(const Sample& sobolPoints, const Point& a0, const Point& b0,
           const TriangularMatrix& L, const UnsignedInteger M, const Scalar alpha,
           const UnsignedInteger d, const Scalar nu)
      : sobolPoints_(sobolPoints)
      , a0_(a0)
      , b0_(b0)
      , L_(L)
      , M_(M)
      , alpha_(alpha)
      , d_(d)
      , nu_(nu)
      , sum_(0.0) {}

    Reduce(const Reduce& other, TBBImplementation::Split)
      : sobolPoints_(other.sobolPoints_)
      , a0_(other.a0_)
      , b0_(other.b0_)
      , L_(other.L_)
      , M_(other.M_)
      , alpha_(other.alpha_)
      , d_(other.d_)
      , nu_(other.nu_)
      , sum_(0.0) {}

    void operator()(const TBBImplementation::BlockedRange<UnsignedInteger>& r)
    {
      for (UnsignedInteger k = r.begin(); k != r.end(); ++k)
      {
        const Scalar s = DistFunc::qChi(nu_, sobolPoints_(k, 0)) / std::sqrt(nu_);
        Point a_s(d_), b_s(d_);
        for (UnsignedInteger i = 0; i < d_; ++i)
        {
          a_s[i] = a0_[i] * s;
          b_s[i] = b0_[i] * s;
        }
        sum_ += mvn_orthant_probability(a_s, b_s, L_, M_, alpha_);
      }
    }

    void join(const Reduce& other) { sum_ += other.sum_; }
  };

  Reduce body(sobolPoints, a0, b0, L, M, alpha, d, nu);
  TBBImplementation::ParallelReduce(0, N, body);
  return SpecFunc::Clip01(body.sum_ / static_cast<Scalar>(N));
}

Scalar mvt_orthant_probability(
    const Point& a, const Point& b,
    const Point& mu, const CovarianceMatrix& sigma,
    const Scalar nu,
    const UnsignedInteger M,
    const Scalar alpha,
    const UnsignedInteger N)
{
  return mvt_orthant_probability(a, b, mu, sigma.computeCholesky(), nu, M, alpha, N);
}

Scalar mvt_orthant_probability(
    const Point& a, const Point& b,
    const TriangularMatrix& L,
    const Scalar nu,
    const UnsignedInteger M,
    const Scalar alpha,
    const UnsignedInteger N)
{
  const UnsignedInteger d = L.getDimension();
  return mvt_orthant_probability(a, b, Point(d, 0.0), L, nu, M, alpha, N);
}

Scalar mvt_orthant_probability(
    const Point& a, const Point& b,
    const CovarianceMatrix& sigma,
    const Scalar nu,
    const UnsignedInteger M,
    const Scalar alpha,
    const UnsignedInteger N)
{
  const UnsignedInteger d = sigma.getDimension();
  return mvt_orthant_probability(a, b, Point(d, 0.0), sigma, nu, M, alpha, N);
}

END_NAMESPACE_OPENTURNS
