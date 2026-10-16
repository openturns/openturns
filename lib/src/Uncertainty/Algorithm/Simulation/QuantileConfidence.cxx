//                                               -*- C++ -*-
/**
 *  @brief Quantile confidence interval computation
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
#include <limits>
#include "openturns/QuantileConfidence.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Binomial.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Brent.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/EvaluationImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(QuantileConfidence)

static const Factory<QuantileConfidence> Factory_QuantileConfidence;

/*
 * @class QuantileConfidence
*/

QuantileConfidence::QuantileConfidence()
  : PersistentObject()
{
  // Nothing to to do
}

/* Constructor with parameters */
QuantileConfidence::QuantileConfidence(const Scalar alpha, const Scalar beta)
  : PersistentObject()
  , alpha_(alpha)
  , beta_(beta)
{
  if (!(alpha >= 0.0 && alpha <= 1.0))
    throw InvalidArgumentException(HERE)
      << "Quantile level must be in [0, 1], but alpha = " << alpha;
  if (!(beta >= 0.0 && beta <= 1.0))
    throw InvalidArgumentException(HERE)
      << "Confidence level must be in [0, 1], but beta = " << beta;
}

/* Virtual constructor */
QuantileConfidence * QuantileConfidence::clone() const
{
  return new QuantileConfidence(*this);
}

/* String converter */
String QuantileConfidence::__repr__() const
{
  return OSS() << GetClassName()
         << " alpha=" << alpha_
         << " beta=" << beta_;
}

String QuantileConfidence::__str__(const String & /*offset*/) const
{
  return OSS() << GetClassName() << "(alpha=" << alpha_ << ", beta=" << beta_ << ")";
}

// compute rank k given by the beta-level quantile of B(n, alpha)
UnsignedInteger QuantileConfidence::computeUnilateralRank(const UnsignedInteger size, const Bool lowerBounded) const
{
  const UnsignedInteger minimumSize = computeUnilateralMinimumSampleSize(0, lowerBounded);
  if (size < minimumSize)
    throw InvalidArgumentException(HERE)
        << "Cannot compute unilateral rank as size (" << size
        << ") is lower than minimum size (" << minimumSize << ")";

  const Binomial binomial(size, alpha_);
  // Best achievable coverage with an in-sample order statistic:
  // CCDF(0) = 1 - (1 - alpha)^n for a lower bound, CDF(n - 1) = 1 - alpha^n for an upper bound.
  // Use the Binomial CDF directly instead of pow() to avoid underflow for large sizes.
  const Scalar p = lowerBounded ? binomial.computeComplementaryCDF(0) : binomial.computeCDF(size - 1);

  if (p < beta_)
  {
    if (lowerBounded)
      throw InvalidArgumentException(HERE)
          << "Cannot compute rank as parameters do not satisfy 1 - (1 - alpha)^n >= beta. "
          << "Increase the size, or decrease the confidence level.";
    else
      throw InvalidArgumentException(HERE)
          << "Cannot compute rank as parameters do not satisfy 1 - alpha^n >= beta. "
          << "Increase the size, or decrease the confidence level.";
  }
  UnsignedInteger rank = binomial.computeQuantile(beta_, lowerBounded)[0];
  // Degenerate case beta == 0 and lower bound: the upper-tail quantile is n, out of sample.
  // Any rank is valid, return the largest in-sample rank.
  if (rank >= size)
  {
    if (beta_ == 0.0)
      return size - 1;
    throw InternalException(HERE) << "Cannot compute unilateral rank: quantile " << rank << " out of sample of size " << size;
  }
  return rank;
}

// compute argmin (k1, k2) P_Xd(]k1, k2]) under constraint P_Xd(]k1, k2])>=beta, with Xd~B(n, alpha)
// and 0 <= k1 <= k2 < n (0-based ranks)
Indices QuantileConfidence::computeBilateralRank(const UnsignedInteger size) const
{
  const UnsignedInteger minimumSize = computeBilateralMinimumSampleSize();
  if (size < minimumSize)
    throw InvalidArgumentException(HERE) << "Cannot compute bilateral rank as size (" << size << ") is lower than minimum size (" << minimumSize << ")";

  const Binomial binomial(size, alpha_);
  Scalar pBest = 2.0;
  Indices kBest;
  UnsignedInteger k1 = 0;
  while (k1 < size)
  {
    // update CDF at k1, stop when it crosses the bound
    const Scalar p1 = binomial.computeCDF(k1);
    if (p1 + beta_ >= 1.0)
      break;

    // we know P(]k1, k2])>=beta which gives k2 directly from k1(p1)
    // clamp the probability into [0, 1] to protect the quantile from round-off
    const Scalar probK2 = SpecFunc::Clip01(p1 + beta_);
    const UnsignedInteger k2 = binomial.computeScalarQuantile(probK2);
    const Scalar p2 = binomial.computeCDF(k2);

    // we compute P(k1<Xd<=k2)=CDF(k2)-CDF(k1) and check for improvement
    const Scalar p = p2 - p1;
    LOGDEBUG(OSS() << "k1=" << k1 << " k2=" << k2 << " p=" << p);
    if ((p >= beta_) && (p < pBest))
    {
      pBest = p;
      kBest = {k1, k2};
    }

    // k1 advances right before the next CDF bump at qB(p2-beta), ensure it stays stricly increasing
    // use integer arithmetic: q - 1 would underflow for q == 0, and p2 - beta_ may be slightly negative by round-off
    const Scalar probNext = SpecFunc::Clip01(p2 - beta_);
    const UnsignedInteger qNext = static_cast<UnsignedInteger>(binomial.computeScalarQuantile(probNext));
    const UnsignedInteger qDec = (qNext > 0 ? qNext - 1 : 0);
    k1 = std::max(qDec, k1 + 1);
  }

  if (!kBest.getSize())
    throw InternalException(HERE) << "Cannot compute bilateral rank: no valid interval found for size=" << size;
  return kBest;
}

// compute interval of the form [X_k; +inf[ or ]-inf; X_k] from unilateral rank k
Interval QuantileConfidence::computeUnilateralConfidenceInterval(const Sample & sample, const Bool lowerBounded) const
{
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE)
      << "Expected a non-empty sample, but size is zero";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE)
      << "Expected a sample of dimension 1, but dimension = "
      << sample.getDimension();
  Point lowerBound(1, -SpecFunc::MaxScalar);
  Point upperBound(1, SpecFunc::MaxScalar);
  Interval::BoolCollection finiteLowerBound(1, false);
  Interval::BoolCollection finiteUpperBound(1, false);
  const Sample sortedSample(sample.sort());
  const UnsignedInteger k = computeUnilateralRank(sample.getSize(), lowerBounded);
  if (lowerBounded)
  {
    lowerBound[0] = sortedSample(k, 0);
    finiteLowerBound[0] = true;
  }
  else
  {
    upperBound[0] = sortedSample(k, 0);
    finiteUpperBound[0] = true;
  }
  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
}

// compute interval of the form [X_k; +inf[ or ]-inf; X_k] from unilateral rank k and the actual coverage
Interval QuantileConfidence::computeUnilateralConfidenceIntervalWithCoverage(const Sample & sample, Scalar & coverageOut, const Bool lowerBounded) const
{
  LOGWARN("QuantileConfidence.computeUnilateralConfidenceIntervalWithCoverage(Sample) is deprecated, use QuantileConfidence.computeUnilateralCoverage(UnsignedInteger, UnsignedInteger, Bool)");
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE)
      << "Expected a non-empty sample, but size is zero";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE)
      << "Expected a sample of dimension 1, but dimension = "
      << sample.getDimension();
  // Compute the rank only once: computeUnilateralConfidenceInterval() would recompute it
  const UnsignedInteger k = computeUnilateralRank(sample.getSize(), lowerBounded);
  coverageOut = computeUnilateralCoverage(sample.getSize(), k, lowerBounded);
  Point lowerBound(1, -SpecFunc::MaxScalar);
  Point upperBound(1, SpecFunc::MaxScalar);
  Interval::BoolCollection finiteLowerBound(1, false);
  Interval::BoolCollection finiteUpperBound(1, false);
  const Sample sortedSample(sample.sort());
  if (lowerBounded)
  {
    lowerBound[0] = sortedSample(k, 0);
    finiteLowerBound[0] = true;
  }
  else
  {
    upperBound[0] = sortedSample(k, 0);
    finiteUpperBound[0] = true;
  }
  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
}

// compute interval of the form [X_k1; X_k2] from bilateral ranks k1, k2
Interval QuantileConfidence::computeBilateralConfidenceInterval(const Sample & sample) const
{
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE)
      << "Expected a non-empty sample, but size is zero";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE)
      << "Expected a sample of dimension 1 but dimension = "
      << sample.getDimension();
  const Indices rank(computeBilateralRank(sample.getSize()));
  const Sample sortedSample(sample.sort());
  return Interval(Point({sortedSample(rank[0], 0)}), Point({sortedSample(rank[1], 0)}));
}

// compute interval of the form [X_k1; X_k2] from bilateral ranks k1, k2 with actual coverage
Interval QuantileConfidence::computeBilateralConfidenceIntervalWithCoverage(const Sample & sample, Scalar & coverageOut) const
{
  LOGWARN("QuantileConfidence.computeBilateralConfidenceIntervalWithCoverage(Sample) is deprecated, use QuantileConfidence.computeBilateralCoverage(UnsignedInteger, UnsignedInteger, UnsignedInteger)");
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE)
      << "Expected a non-empty sample, but size is zero";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE)
      << "Expected a sample of dimension 1 but dimension = "
      << sample.getDimension();
  // Compute the ranks only once: computeBilateralConfidenceInterval() would recompute them
  const Indices rank(computeBilateralRank(sample.getSize()));
  coverageOut = computeBilateralCoverage(sample.getSize(), rank[0], rank[1]);
  const Sample sortedSample(sample.sort());
  return Interval(Point({sortedSample(rank[0], 0)}), Point({sortedSample(rank[1], 0)}));
}

Scalar QuantileConfidence::computeUnilateralCoverage(const UnsignedInteger size, const UnsignedInteger rank, const Bool lowerBounded) const
{
  if (rank >= size)
    throw InvalidArgumentException(HERE)
        << "The rank must be strictly less than the sample size, but rank = "
        << rank << ", size = " << size;

  // Y ~ Binomial(size, alpha)
  const Binomial binomial(size, alpha_);
  // Lower bound coverage: 1 - P(Y <= rank), upper bound coverage: P(Y <= rank)
  const Scalar cdf = lowerBounded ? binomial.computeComplementaryCDF(rank) : binomial.computeCDF(rank);
  return cdf;
}

Scalar QuantileConfidence::computeBilateralCoverage(const UnsignedInteger size, const UnsignedInteger rank1, const UnsignedInteger rank2) const
{
  if (rank1 > rank2)
    throw InvalidArgumentException(HERE)
      << "The lower rank (" << rank1
      << ") must be less than or equal to the upper rank (" << rank2 << ").";
  if (rank2 >= size)
    throw InvalidArgumentException(HERE)
      << "The upper rank (" << rank2
      << ") must be strictly less than the sample size (" << size << ").";

  // Y ~ Binomial(size, alpha)
  const Binomial binomial(size, alpha_);

  // P(k1 + 1 <= Y <= k2) = P(Y <= k2) - P(Y <= k1)
  const Scalar cdf_upper = binomial.computeCDF(rank2);
  const Scalar cdf_lower = binomial.computeCDF(rank1);

  return cdf_upper - cdf_lower;
}

namespace
{
class QuantileConfidenceEvaluation: public EvaluationImplementation
{
public:
  QuantileConfidenceEvaluation(const Scalar alpha,
                               const UnsignedInteger rank,
                               const Bool lowerBounded)
    : EvaluationImplementation()
    , alpha_(alpha)
    , rank_(rank)
    , lowerBounded_(lowerBounded)
  {
    // Nothing to do
  }

  QuantileConfidenceEvaluation * clone() const override
  {
    return new QuantileConfidenceEvaluation(*this);
  }

  Point operator() (const Point & point) const override
  {
    return {DistFunc::pBeta(rank_ + 1, point[0] - rank_, lowerBounded_ ? alpha_ : 1.0 - alpha_)};
  }

  UnsignedInteger getInputDimension() const override
  {
    return 1;
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

private:
  Scalar alpha_ = 0.0;
  UnsignedInteger rank_ = 0;
  Bool lowerBounded_ = true;
}; // class QuantileConfidenceEvaluation
} // Anonymous namespace


UnsignedInteger QuantileConfidence::computeUnilateralMinimumSampleSize(const UnsignedInteger rank, const Bool lowerBounded) const
{
  // Here we have to find the minimal value of N such that
  // 1-\sum_{i=N-r}^N Binomial(i, N)\alpha^i(1-\alpha)^{N-i}>=\beta
  // where:
  //   \alpha = alpha_
  //   \beta  = beta_
  //   r      = rank
  // It rewrites F_{N,alpha}(N-r-1)>=beta where F_{N,alpha} is the CDF of the
  // Binomial(N, alpha) distribution.
  // The Wilks formula requires alpha in (0, 1) and beta in (0, 1).
  if (!(alpha_ > 0.0 && alpha_ < 1.0))
    throw InvalidArgumentException(HERE)
      << "Cannot compute minimum sample size with alpha = " << alpha_ << ", expected alpha in (0, 1)";
  if (!(beta_ > 0.0 && beta_ < 1.0))
  {
    if (beta_ == 0.0)
      return rank + 1;
    throw InvalidArgumentException(HERE)
      << "Cannot compute minimum sample size with beta = " << beta_ << ", expected beta in (0, 1)";
  }
  // Easy case: rank=0, the quantile bound is given by the largest upper statistics. The equation to solve is N=\min{n|1-\alpha^n>=\beta}
  Scalar nApprox = 0.0;
  const Function wilksConstraint(QuantileConfidenceEvaluation(alpha_, rank, lowerBounded).clone());
  if (rank == 0)
  {
    if (lowerBounded)
      nApprox = std::log1p(-beta_) / std::log1p(-alpha_);
    else
      nApprox = std::log1p(-beta_) / std::log(alpha_);
  }
  else
  {
    // Search for the minimal sample size
    // First, search for an upper bound
    // Here we use the relation F_{N,alpha}(N - r - 1) = pBeta(N - k, k + 1, 1 - alpha)
    // where k = N - r - 1
    // We compute a reasonable guess for n using a Normal approximation:
    // n*alpha+q_beta*sqrt(n*alpha*(1-alpha))=n-r:
    const Scalar aBeta = DistFunc::qNormal(beta_);
    const Scalar p = lowerBounded ? alpha_ : (1.0 - alpha_);
    const Scalar a = std::abs(aBeta) * std::sqrt(1.0 - p);
    Scalar nMaxAsDouble = (rank + 0.5 * a * a + 0.5 * a * std::sqrt(a * a + 4.0 * rank)) / p;
    if (!std::isfinite(nMaxAsDouble))
      throw InternalException(HERE) << "Cannot compute initial upper bound for minimum sample size";
    // The Normal guess may fall below rank + 1 for large p and small rank: clamp it,
    // the doubling loop below then brackets the root. Cap the initial guess to
    // avoid overflow of the narrowing cast for extreme ranks.
    const UnsignedInteger maxInit = std::numeric_limits<UnsignedInteger>::max() / 2;
    const Scalar clampedGuess = std::clamp(nMaxAsDouble, static_cast<Scalar>(rank + 1), static_cast<Scalar>(maxInit));
    UnsignedInteger nMax = static_cast<UnsignedInteger>(std::ceil(clampedGuess));
    if (nMax < rank + 1) nMax = rank + 1;
    // This loop must end as wilksConstraint->1 when n->inf
    // guard against overflow of the doubling sequence
    const UnsignedInteger maxSize = std::numeric_limits<UnsignedInteger>::max() / 2;
    while (wilksConstraint(Point({0.0 + nMax}))[0] < beta_)
    {
      if (nMax > maxSize)
        throw InternalException(HERE) << "Cannot bracket minimum sample size: upper bound overflow";
      // At the beginning of the loop nMax is >= 1 so it increases strictly
      nMax += nMax;
    }
    // The continuous Beta extension is only defined for n > rank (b = n - rank > 0),
    // so bracket the Brent root on [rank + 1, nMax]
    const UnsignedInteger nLower = rank + 1;
    if (nMax <= nLower)
      nApprox = nLower;
    else
      nApprox = Brent().solve(wilksConstraint, beta_, nLower, nMax);
  } // rank > 0
  // nApprox is a real root of the continuous extension; the integer answer is
  // ceil(nApprox) possibly corrected by at most one unit due to solver tolerance.
  // Adjust with exact integer Binomial evaluations to guarantee minimality.
  if (!(nApprox >= 0.0) || !std::isfinite(nApprox))
    throw InternalException(HERE) << "Cannot compute minimum sample size: invalid root approximation";
  const Scalar maxSizeAsDouble = static_cast<Scalar>(std::numeric_limits<UnsignedInteger>::max() - 1);
  if (nApprox > maxSizeAsDouble)
    throw InternalException(HERE) << "Cannot compute minimum sample size: required size exceeds representable range";
  UnsignedInteger candidate = std::max(rank + 1, static_cast<UnsignedInteger>(std::ceil(nApprox - 1.0e-12)));
  // Guard against a pathological nApprox below rank + 1
  if (candidate < rank + 1) candidate = rank + 1;
  auto unilateralCoverageAt = [&](const UnsignedInteger n) -> Scalar
  {
    const Binomial binomial(n, alpha_);
    if (lowerBounded)
      return binomial.computeComplementaryCDF(rank);
    return binomial.computeCDF(n - 1 - rank);
  };
  while (candidate > rank + 1 && unilateralCoverageAt(candidate - 1) >= beta_)
    --candidate;
  const UnsignedInteger maxSize = std::numeric_limits<UnsignedInteger>::max() - 1;
  while (unilateralCoverageAt(candidate) < beta_)
  {
    if (candidate >= maxSize)
      throw InternalException(HERE) << "Cannot compute minimum sample size: upper bound overflow";
    ++candidate;
  }
  return candidate;
}

// Find the minimal value of N such that 1 - alpha^n - (1 - alpha)^n >= beta
UnsignedInteger QuantileConfidence::computeBilateralMinimumSampleSize() const
{
  if (!(alpha_ > 0.0 && alpha_ < 1.0))
    throw InvalidArgumentException(HERE)
      << "Cannot compute bilateral minimum sample size with alpha = " << alpha_ << ", expected alpha in (0, 1)";
  if (beta_ == 0.0)
    return 1;
  if (!(beta_ > 0.0 && beta_ < 1.0))
    throw InvalidArgumentException(HERE)
      << "Cannot compute bilateral minimum sample size with beta = " << beta_ << ", expected beta in (0, 1)";
  const Scalar gamma = std::max(alpha_, 1.0 - alpha_);
  const Scalar nMin = std::ceil(std::log1p(-beta_) / std::log(gamma));
  const Scalar nMax = std::ceil((std::log1p(-beta_) - std::log(2)) / std::log(gamma));
  const SymbolicFunction residualFunction(Description({"n", "alpha", "beta"}), Description({"1 - alpha^n - (1 - alpha)^n - beta"}));
  const ParametricFunction residualParametric(residualFunction, Indices({1, 2}), Point({alpha_, beta_}));
  Brent solver;
  const Scalar root = solver.solve(residualParametric, 0.0, nMin - 1, nMax);
  if (!std::isfinite(root))
    throw InternalException(HERE) << "Cannot compute bilateral minimum sample size: invalid root";
  if (root > static_cast<Scalar>(std::numeric_limits<UnsignedInteger>::max() - 1))
    throw InternalException(HERE) << "Cannot compute bilateral minimum sample size: required size exceeds representable range";
  // ceil(root) may overshoot by one unit due to solver tolerance: adjust with exact integer coverage
  UnsignedInteger candidate = static_cast<UnsignedInteger>(std::ceil(root - 1.0e-12));
  if (candidate < 1) candidate = 1;
  auto bilateralCoverageAt = [&](const UnsignedInteger n) -> Scalar
  {
    if (n == 0) return -1.0;
    const Binomial binomial(n, alpha_);
    // P(X_(0) <= x_alpha <= X_(n-1)) = CDF(n - 1) - CDF(0)
    return binomial.computeCDF(n - 1) - binomial.computeCDF(0);
  };
  while (candidate > 1 && bilateralCoverageAt(candidate - 1) >= beta_)
    --candidate;
  while (bilateralCoverageAt(candidate) < beta_)
  {
    if (candidate >= std::numeric_limits<UnsignedInteger>::max() - 1)
      throw InternalException(HERE) << "Cannot compute bilateral minimum sample size: upper bound overflow";
    ++candidate;
  }
  return candidate;
}


// Compute k1, k2 such that lim n=>inf P(X_k1<x_alpha<X_k2)=beta, see [delmas2006] proposition 12.2.13 page 257
Indices QuantileConfidence::computeAsymptoticBilateralRank(const UnsignedInteger size) const
{
  if (size == 0)
    throw InvalidArgumentException(HERE) << "Cannot compute asymptotic bilateral rank with size 0";
  const Scalar z = DistFunc::qNormal((1.0 + beta_) * 0.5);
  const Scalar delta = z * std::sqrt(alpha_ * (1.0 - alpha_) * size);
  const Scalar sup = static_cast<Scalar>(size - 1);
  // floor() may be negative for small size * alpha: clamp into [0, size - 1] before the narrowing cast
  const UnsignedInteger k1 = static_cast<UnsignedInteger>(std::clamp(std::floor(size * alpha_ - delta - 1.0), 0.0, sup));
  const UnsignedInteger k2 = static_cast<UnsignedInteger>(std::clamp(std::floor(size * alpha_ + delta - 1.0), 0.0, sup));
  return {k1, k2};
}

// compute interval of the form [X_k1; X_k2] from asymptotic bilateral ranks k1, k2
Interval QuantileConfidence::computeAsymptoticBilateralConfidenceInterval(const Sample & sample) const
{
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE)
      << "Expected a non-empty sample, but size is zero";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE)
      << "Expected a sample of dimension 1, but dimension = "
      << sample.getDimension();
  const Indices rank(computeAsymptoticBilateralRank(sample.getSize()));
  const Sample sortedSample(sample.sort());
  return Interval(Point({sortedSample(rank[0], 0)}), Point({sortedSample(rank[1], 0)}));
}

/* Quantile level accessor */
void QuantileConfidence::setAlpha(const Scalar alpha)
{
  if (!(alpha >= 0.0 && alpha <= 1.0))
    throw InvalidArgumentException(HERE)
      << "Quantile level must be in [0, 1], but alpha = " << alpha;
  alpha_ = alpha;
}

Scalar QuantileConfidence::getAlpha() const
{
  return alpha_;
}

/* Confidence level accessor */
void QuantileConfidence::setBeta(const Scalar beta)
{
  if (!(beta >= 0.0 && beta <= 1.0))
    throw InvalidArgumentException(HERE)
      << "Confidence level must be in [0, 1], but beta = " << beta;
  beta_ = beta;
}

Scalar QuantileConfidence::getBeta() const
{
  return beta_;
}


/* Method save() stores the object through the StorageManager */
void QuantileConfidence::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "alpha_", alpha_ );
  adv.saveAttribute( "beta_", beta_ );
}

/* Method load() reloads the object from the StorageManager */
void QuantileConfidence::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "alpha_", alpha_ );
  adv.loadAttribute( "beta_", beta_ );
}

END_NAMESPACE_OPENTURNS
