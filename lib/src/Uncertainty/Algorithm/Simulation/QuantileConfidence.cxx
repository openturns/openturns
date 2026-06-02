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
#include <iomanip>
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
  const Scalar p = lowerBounded ? 1.0 - std::pow(1.0 - alpha_, 1.0 * size) : 1.0 - std::pow(alpha_, 1.0 * size);

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
  const UnsignedInteger rank = binomial.computeQuantile(beta_, lowerBounded)[0];
  return rank;
}

Indices QuantileConfidence::computeBilateralRank(const UnsignedInteger size) const
{
  const String method = ResourceMap::GetAsString("QuantileConfidence-Method");
  LOGDEBUG(OSS(false) << "computeBilateralRank(" << size << ")");
  LOGDEBUG(OSS(false) << "method = " << method << ", alpha = " << alpha_ << ", beta = " << beta_);
  if (method == "hybrid") return computeBilateralRankHybrid(size);
  else if (method == "jump") return computeBilateralRankJump(size);
  else if (method == "epsilon") return computeBilateralRankEpsilon(size);
  else throw InvalidArgumentException(HERE) << "Error: invalid value for bilateral rank method: " << method;
}

// Compute argmin (k1, k2) P_X([k1, k2]) under constraint P_X([k1, k2])>=beta, 
// with X~B(n, alpha)
Indices QuantileConfidence::computeBilateralRankHybrid(const UnsignedInteger size) const
{
  LOGDEBUG(OSS(false) << "computeBilateralRankHybrid(" << size << ")");
  const UnsignedInteger minimumSize = computeBilateralMinimumSampleSize();
  if (size < minimumSize)
    throw InvalidArgumentException(HERE) 
        << "Cannot compute bilateral rank as size (" << size 
        << ") is lower than minimum size (" << minimumSize << ")";

  // Consider the Binomial(n=size, alpha).
  // Find the indices (k1, k2) with smallest probability >=beta.
  const Binomial binomial(size, alpha_);

  Bool found = false;
  Scalar pBest = SpecFunc::MaxScalar;
  UnsignedInteger k1Best = 0;
  UnsignedInteger k2Best = size - 1;

  const Scalar probabilityEpsilon = ResourceMap::GetAsScalar("QuantileConfidence-ProbabilityEpsilon");
  UnsignedInteger k1 = 0;
  Scalar p1= binomial.computeCDF(0);
  LOGDEBUG(OSS(false) << "CDF(" << k1 << ")");
  ++countFEval_;

  UnsignedInteger lastK2 = size;
  Scalar p2 = SpecFunc::MaxScalar;

  while (k1 < size)
  {
    // Check that CDF(k1) + beta <= 1, otherwise computeScalarQuantile fails
    if (p1 + beta_ > 1.0)
      break;

    // Compute k2 from p1
    const UnsignedInteger k2 = binomial.computeScalarQuantile(p1 + beta_);
    LOGDEBUG(OSS(false) << "Quantile(" << p1 + beta_ << ") = " << k2);
    ++countQEval_;

    // Ensure the upper rank is within the valid sample index range [0, size - 1]
    if (k2 >= size)
      break;

    // Compute P(k1 < X <= k2) = CDF(k2) - CDF(k1)
    if (k2 != lastK2)
    {
      lastK2 = k2;
      p2 = binomial.computeCDF(k2);
      LOGDEBUG(OSS(false) << "CDF(" << k2 << ")");
      ++countFEval_;
    }
    const Scalar p = p2 - p1;
    if ((p >= beta_) && (p < pBest))
    {
      pBest = p;
      k1Best = k1;
      k2Best = k2;
      found = true;
    }
    // Find the next significant probability jump
    UnsignedInteger ell;
    Scalar pEll;
    if (searchProbabilityJump(size, k1, p1, probabilityEpsilon, ell, pEll))
    {
      k1 = ell;
      p1 = pEll;
    }
    else
      break;
    
  } // while k1
  if (!found)
    throw InvalidArgumentException(HERE)
      << "Cannot find suitable ranks for size=" 
      << size << ", alpha=" << alpha_ << ", beta=" << beta_ <<". "
      << "Increase the size, or decrease the confidence level.";

  LOGDEBUG(OSS(false) << "FEval = " << countFEval_ << ", QEval = " << countQEval_);
  return {k1Best, k2Best};
}

// Compute argmin (k1, k2) P_B((k1, k2]) under constraints P_B((k1, k2]) >= beta,
// 0 <= k1 <= k2 <= n - 1, with B ~ Binomial(n, alpha)
Indices QuantileConfidence::computeBilateralRankEpsilon(const UnsignedInteger size) const
{
  const UnsignedInteger minimumSize = computeBilateralMinimumSampleSize();
  if (size < minimumSize)
    throw InvalidArgumentException(HERE) 
        << "Cannot compute bilateral rank as size (" << size 
        << ") is lower than minimum size (" << minimumSize << ")";

  // Consider the Binomial(n=size, alpha).
  // Find the indices (k1, k2) with smallest probability >=beta.
  const Binomial binomial(size, alpha_);
  Bool found = false;
  Scalar pBest = SpecFunc::MaxScalar;
  UnsignedInteger k1Best = 0;
  UnsignedInteger k2Best = size - 1;
  const Scalar probabilityEpsilon = ResourceMap::GetAsScalar("QuantileConfidence-ProbabilityEpsilon");
  UnsignedInteger k1 = binomial.computeScalarQuantile(probabilityEpsilon);
  LOGDEBUG(OSS(false) << "Quantile(" << probabilityEpsilon << ") = " << k1);
  ++countQEval_;
  Scalar p1 = binomial.computeCDF(k1);
  LOGDEBUG(OSS(false) << "CDF(" << k1 << ")");
  ++countFEval_;
  UnsignedInteger k2 = k1;
  Scalar p2 = p1;
  while (k1 < size)
  {
    // Advance k2 monotonically until the probability mass condition is satisfied
    while (k2 < size && (p2 - p1 < beta_))
    {
      ++k2;
      // Only compute CDF if k2 is within the valid sample index range
      if (k2 < size)
      {
        p2 = binomial.computeCDF(k2);
        LOGDEBUG(OSS(false) << "CDF(" << k2 << ")");
        ++countFEval_;
      }
    }

    // If k2 goes out of bounds without satisfying the constraint, terminate search
    if (k2 >= size || (p2 - p1 < beta_))
      break;

    // Compute P(k1 < B <= k2) = CDF(k2) - CDF(k1)
    const Scalar p = p2 - p1;
    if (p < pBest)
    {
      pBest = p;
      k1Best = k1;
      k2Best = k2;
      found = true;
    }
    // Go on to the next value of k1
    ++k1;
    p1 = binomial.computeCDF(k1);
    LOGDEBUG(OSS(false) << "CDF(" << k1 << ")");
    ++countFEval_;
  } // while k1
  if (!found)
    throw InvalidArgumentException(HERE)
      << "Cannot find suitable ranks for size=" 
      << size << ", alpha=" << alpha_ << ", beta=" << beta_ <<". "
      << "Increase the size, or decrease the confidence level.";

  LOGDEBUG(OSS(false) << "FEval = " << countFEval_ << ", QEval = " << countQEval_);
  return {k1Best, k2Best};
}

// Compute argmin (k1, k2) P_X([k1, k2]) under constraint P_X([k1, k2])>=beta, 
// with X~B(n, alpha)
Indices QuantileConfidence::computeBilateralRankJump(const UnsignedInteger size) const
{
  const Scalar probabilityEpsilon = ResourceMap::GetAsScalar("QuantileConfidence-ProbabilityEpsilon");
  const UnsignedInteger minimumSize = computeBilateralMinimumSampleSize();
  if (size < minimumSize)
    throw InvalidArgumentException(HERE) 
        << "Cannot compute bilateral rank as size (" << size 
        << ") is lower than minimum size (" << minimumSize << ")";

  // Consider the Binomial(n=size, alpha).
  // Find the indices (k1, k2) with smallest probability >=beta.
  const Binomial binomial(size, alpha_);
  Bool found = false;
  Scalar pBest = SpecFunc::MaxScalar;
  UnsignedInteger k1Best = 0;
  UnsignedInteger k2Best = size - 1;
  UnsignedInteger k1 = 0;
  Scalar p1 = binomial.computeCDF(k1);
  LOGDEBUG(OSS(false) << "CDF(" << k1 << ")" << p1);
  ++countFEval_;
  UnsignedInteger lastK2 = size;
  Scalar p2 = SpecFunc::MaxScalar;
  while (k1 < size)
  {
    // Check that CDF(k1) + beta <= 1, otherwise computeScalarQuantile fails
    if (p1 + beta_ > 1.0)
      break;

    // Compute k2 from p1
    const UnsignedInteger k2 = binomial.computeScalarQuantile(p1 + beta_);
    LOGDEBUG(OSS(false) << "Quantile(" << p1 + beta_ << ") = " << k2);
    ++countQEval_;

    // Ensure the upper rank is within the valid sample index range [0, size - 1]
    if (k2 >= size)
      break;

    // Compute P(k1 < X <= k2) = CDF(k2) - CDF(k1)
    if (k2 != lastK2) {
      lastK2 = k2;
      p2 = binomial.computeCDF(k2);
      LOGDEBUG(OSS(false) << "CDF(" << k2 << ")" << p2);
      ++countFEval_;
    }
    const Scalar p = p2 - p1;
    if ((p >= beta_) && (p < pBest))
    {
      pBest = p;
      k1Best = k1;
      k2Best = k2;
      found = true;
    }
    // Find the next significant probability jump
    UnsignedInteger ell;
    Scalar pEll;
    if (searchProbabilityJump(size, k1, p1, probabilityEpsilon, ell, pEll))
    {
      k1 = ell;
      p1 = pEll;
    }
    else
      break;
    
  } // while k1
  if (!found)
    throw InvalidArgumentException(HERE)
      << "Cannot find suitable ranks for size=" 
      << size << ", alpha=" << alpha_ << ", beta=" << beta_ <<". "
      << "Increase the size, or decrease the confidence level.";

  LOGDEBUG(OSS(false) << "FEval = " << countFEval_ << ", QEval = " << countQEval_);
  return {k1Best, k2Best};
}


/** For a given size and k in [0, size - 1], 
 * find the smallest ell greater or equal than k such that 
 * F(ell) > F(k) + epsilon and ell > k.
 * On output, if the algorithm succeeds, then ell <= size - 1.
 * Otherwise, ell = size. */
Bool QuantileConfidence::searchProbabilityJump(const UnsignedInteger size, const UnsignedInteger k, const Scalar pk, const Scalar probabilityEpsilon, UnsignedInteger & ell, Scalar & pEll) const
{
  const Binomial binomial(size, alpha_);
  const Scalar probabilityReference = pk + probabilityEpsilon;

  if (probabilityReference >= 1.0)
  {
    ell = size;
    return false;
  }

  // Phase 1: Exponential search to find an exact valid bracket (low, high]
  UnsignedInteger low = k;
  UnsignedInteger high = k + 1;
  Scalar pHigh;

  while (true)
  {
    pHigh = binomial.computeCDF(high);
    LOGDEBUG(OSS(false) << "CDF(" << high << ") = " << pHigh);
    ++countFEval_;

    if (pHigh > probabilityReference)
      break;

    low = high;
    const UnsignedInteger diff = high - k;
    // Safely double the search step while preventing integer overflow
    high = (size - high < diff) ? size : high + diff;
  }

  // Phase 2: Standard binary search within the invariant interval (low, high]
  // Find the smallest ell such that F(ell) > F(k) + epsilon.
  while (high - low > 1)
  {
    // Overflow-safe midpoint calculation
    const UnsignedInteger center = low + (high - low) / 2;
    const Scalar probabilityCenter = binomial.computeCDF(center);
    LOGDEBUG(OSS(false) << "CDF(" << center << ") = " << probabilityCenter);
    ++countFEval_;

    if (probabilityCenter > probabilityReference)
    {
      high = center;
      pHigh = probabilityCenter;
    }
    else
      low = center;
  }

  ell = high;
  pEll = pHigh;
  return ell < size;
}

// compute interval of the form [X_k; +inf[ or ]-inf; X_k] from unilateral rank k
Interval QuantileConfidence::computeUnilateralConfidenceInterval(const Sample & sample, const Bool lowerBounded) const
{
  Scalar coverageOut = -1.0;
  return computeUnilateralConfidenceIntervalWithCoverage(sample, coverageOut, lowerBounded);
}

// compute interval of the form [X_k; +inf[ or ]-inf; X_k] from unilateral rank k and the actual coverage
Interval QuantileConfidence::computeUnilateralConfidenceIntervalWithCoverage(const Sample & sample, Scalar & coverageOut, const Bool lowerBounded) const
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
  coverageOut = computeUnilateralCoverage(sample.getSize(), k, lowerBounded);
  return Interval(lowerBound, upperBound, finiteLowerBound, finiteUpperBound);
}

// compute interval of the form [X_k1; X_k2] from bilateral ranks k1, k2
Interval QuantileConfidence::computeBilateralConfidenceInterval(const Sample & sample) const
{
  Scalar coverageOut = -1.0;
  return computeBilateralConfidenceIntervalWithCoverage(sample, coverageOut);
}

// compute interval of the form [X_k1; X_k2] from bilateral ranks k1, k2 with actual coverage
Interval QuantileConfidence::computeBilateralConfidenceIntervalWithCoverage(const Sample & sample, Scalar & coverageOut) const
{
  if (sample.getSize() == 0)
    throw InvalidArgumentException(HERE)
      << "Expected a non-empty sample, but size is zero";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE)
      << "Expected a sample of dimension 1 but dimension = "
      << sample.getDimension();
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
  Binomial binomial(size, alpha_);  
  Scalar cdf;

  if (lowerBounded)
    // Lower bound coverage: 1 - P(Y <= rank)
    cdf = binomial.computeComplementaryCDF(rank);
  else
    // Upper bound coverage: P(Y <= rank)
    cdf = binomial.computeCDF(rank);
  
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
  Binomial binomial(size, alpha_);
  
  // P(k1 + 1 <= Y <= k2) = P(Y <= k2) - P(Y <= k1)
  Scalar cdf_upper = binomial.computeCDF(rank2);
  Scalar cdf_lower = binomial.computeCDF(rank1);
  
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


UnsignedInteger QuantileConfidence::computeUnilateralMinimumSampleSize(const UnsignedInteger tail_rank, const Bool lowerBounded) const
{
  // Here we have to find the minimal value of N such that
  // 1-\sum_{i=N-r}^N Binomial(i, N)\alpha^i(1-\alpha)^{N-i}>=\beta
  // where:
  //   \alpha = alpha_
  //   \beta  = beta_
  //   r      = rank
  // It rewrites F_{N,alpha}(N-r-1)>=beta where F_{N,alpha} is the CDF of the
  // Binomial(N, alpha) distribution.
  // Easy case: rank=0, the quantile bound is given by the largest upper statistics. The equation to solve is N=\min{n|1-\alpha^n>=\beta}
  Scalar nApprox = 0.0;
  const Function wilksConstraint(QuantileConfidenceEvaluation(alpha_, tail_rank, lowerBounded).clone());
  if (tail_rank == 0)
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
    UnsignedInteger nMax = static_cast<UnsignedInteger>((tail_rank + 0.5 * (alpha_ * aBeta * aBeta + std::abs(aBeta) * std::sqrt(alpha_ * (4.0 * tail_rank + alpha_ * aBeta * aBeta)))) / (1.0 - alpha_));
    // This loop must end as wilksConstraint->1 when n->inf
    while (wilksConstraint(Point({0.0 + nMax}))[0] < beta_)
      // At the beginning of the loop nMax is >= 1 so it increases strictly
      nMax += nMax;
    nApprox = Brent().solve(wilksConstraint, beta_, tail_rank, nMax);
  } // rank > 0
  // Here, nApprox can be very close to an integer (in the sense of: the value of the constraint evaluated at the nearest integer is very close to beta_), in which case the correct answer is round(nApprox), or the answer is the next integer value
  const UnsignedInteger nInf = std::round(std::max(1.0 * (tail_rank + 1), nApprox));
  const Scalar constraintInf = wilksConstraint(Point({0.0 + nInf}))[0];
  if (std::abs(constraintInf - beta_) < std::sqrt(SpecFunc::Precision)) return nInf;
  const UnsignedInteger minimumSampleSize = std::max(tail_rank + 1, static_cast<UnsignedInteger>(std::ceil(nApprox)));
  return minimumSampleSize;
}

// Find the minimal value of N such that 1 - alpha^n - (1 - alpha)^n >= beta
UnsignedInteger QuantileConfidence::computeBilateralMinimumSampleSize() const
{
  const Scalar gamma = std::max(alpha_, 1.0 - alpha_);
  const Scalar nMin = std::ceil(std::log1p(-beta_) / std::log(gamma));
  const Scalar nMax = std::ceil((std::log1p(-beta_) - std::log(2)) / std::log(gamma));
  const SymbolicFunction residualFunction(Description({"n", "alpha", "beta"}), Description({"1 - alpha^n - (1 - alpha)^n - beta"}));
  const ParametricFunction residualParametric(residualFunction, Indices({1, 2}), Point({alpha_, beta_}));
  Brent solver;
  const Scalar root = solver.solve(residualParametric, 0.0, nMin - 1, nMax);
  return std::ceil(root);
}


// Compute k1, k2 such that lim n=>inf P(X_k1<x_alpha<X_k2)=beta, see [delmas2006] proposition 12.2.13 page 257
Indices QuantileConfidence::computeAsymptoticBilateralRank(const UnsignedInteger size) const
{
  const Scalar z = DistFunc::qNormal((1.0 + beta_) * 0.5);
  const Scalar delta = z * std::sqrt(alpha_ * (1.0 - alpha_) * size);
  const UnsignedInteger k1 = std::max(0.0, std::floor(size * alpha_ - delta - 1.0));
  const UnsignedInteger k2 = std::min(size - 1.0, std::floor(size * alpha_ + delta - 1.0));
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
  alpha_ = alpha;
}

Scalar QuantileConfidence::getAlpha() const
{
  return alpha_;
}

/* Confidence level accessor */
void QuantileConfidence::setBeta(const Scalar beta)
{
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
