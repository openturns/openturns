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
  if (!(alpha >= 0.0) || !(alpha <= 1.0))
    throw InvalidArgumentException(HERE) << "Quantile level must be in [0, 1]";
  if (!(beta >= 0.0) || !(beta <= 1.0))
    throw InvalidArgumentException(HERE) << "Confidence level must be in [0, 1]";
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
UnsignedInteger QuantileConfidence::computeUnilateralRank(const UnsignedInteger size, const Bool tail) const
{
  const UnsignedInteger minimumSize = computeUnilateralMinimumSampleSize(0, tail);
  if (size < minimumSize)
    throw InvalidArgumentException(HERE) << "Cannot compute unilateral rank as size (" << size << ") is lower than minimum size (" << minimumSize << ")";

  const Binomial binomial(size, alpha_);
  const Scalar p = tail ? 1.0 - std::pow(alpha_, 1.0 * size) : 1.0 - std::pow(1.0 - alpha_, 1.0 * size);

  if (p < beta_)
  {
    if (tail)
      throw InvalidArgumentException(HERE) << "Cannot compute rank as parameters do not satisfy 1 - alpha^n >= beta";
    else
      throw InvalidArgumentException(HERE) << "Cannot compute rank as parameters do not satisfy 1 - (1 - alpha)^n >= beta";
  }

  const UnsignedInteger rank = binomial.computeQuantile(beta_, tail)[0];
  return rank;
}

// compute argmin (k1, k2) P_X([k1, k2]) under constraint P_X([k1, k2])>=beta, with X~B(n, alpha)
Indices QuantileConfidence::computeBilateralRank(const UnsignedInteger size) const
{
  const UnsignedInteger minimumSize = computeBilateralMinimumSampleSize();
  if (size < minimumSize)
    throw InvalidArgumentException(HERE) << "Cannot compute bilateral rank as size (" << size << ") is lower than minimum size (" << minimumSize << ")";

  // find the indices of interval ]k1, k2] with smallest probability >=beta according to a Binomial(n=size, alpha)
  const Binomial binomial(size, alpha_);
  Scalar pBest = SpecFunc::MaxScalar;
  UnsignedInteger k1Best = size;
  UnsignedInteger k2Best = size;
  Scalar p1Prev = 0.0;
  UnsignedInteger k1 = 0;
  while (k1 < size)
  {
    // find upper bound of cdf increment location
    Scalar p1 = binomial.computeCDF(k1);
    UnsignedInteger inc = 1;
    UnsignedInteger k1Prev = k1;
    while (p1 == p1Prev)
    {
      k1Prev = k1;
      k1 += inc;
      p1 = binomial.computeCDF(k1);
      inc *= 2;
    }

    // find exact cdf increment location
    UnsignedInteger k1Max = k1;
    UnsignedInteger k1Min = k1Prev;
    while (k1Max - k1Min > 1)
    {
      const UnsignedInteger k1Mid = (k1Min + k1Max) / 2;
      const Scalar p1Mid = binomial.computeCDF(k1Mid);
      if (p1Mid > p1Prev)
        k1Max = k1Mid;
      else
        k1Min = k1Mid;
    }
    k1 = k1Max;

    // we have to stop when the probability will cross the bound
    if (p1 + beta_ > 1.0)
      break;

    // we know P([k1, k2])>=beta which gives k2 directly from k1(p1)
    const UnsignedInteger k2 = binomial.computeScalarQuantile(p1 + beta_);
    p1Prev = p1;

    // we compute P(k1<X<=k2)=P(k1+1<=X<=k2), CDF(k2)-CDF(k1) also works
    const Scalar p = binomial.computeProbability(Interval(k1 + 1, k2));
    LOGDEBUG(OSS() << "k1=" << k1 << " k2=" << k2 << " p=" << p);
    if ((p >= beta_) && (p < pBest))
    {
      pBest = p;
      k1Best = k1;
      k2Best = k2;
    }
  } // while k1
  return {k1Best, k2Best};
}

// compute interval of the form [X_k; +inf[ or ]-inf; X_k] from unilateral rank k
Interval QuantileConfidence::computeUnilateralConfidenceInterval(const Sample & sample, const Bool tail) const
{
  Scalar coverageOut = -1.0;
  return computeUnilateralConfidenceIntervalWithCoverage(sample, coverageOut, tail);
}

// compute interval of the form [X_k; +inf[ or ]-inf; X_k] from unilateral rank k and the actual coverage
Interval QuantileConfidence::computeUnilateralConfidenceIntervalWithCoverage(const Sample & sample, Scalar & coverageOut, const Bool tail) const
{
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Expected a sample of dimension 1";
  Point lowerBound(1, -SpecFunc::MaxScalar);
  Point upperBound(1, SpecFunc::MaxScalar);
  Interval::BoolCollection finiteLowerBound(1, false);
  Interval::BoolCollection finiteUpperBound(1, false);
  const Sample sortedSample(sample.sort());
  const UnsignedInteger k = computeUnilateralRank(sample.getSize(), tail);
  const Binomial binomial(sample.getSize(), getAlpha());
  if (tail)
  {
    lowerBound[0] = sortedSample(k, 0);
    finiteLowerBound[0] = true;
    coverageOut = binomial.computeComplementaryCDF(k);
  }
  else
  {
    upperBound[0] = sortedSample(k, 0);
    finiteUpperBound[0] = true;
    coverageOut = binomial.computeCDF(k);
  }
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
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Expected a sample of dimension 1";
  const Indices rank(computeBilateralRank(sample.getSize()));
  const Binomial binomial(sample.getSize(), getAlpha());
  coverageOut = binomial.computeCDF(rank[1]) - binomial.computeCDF(rank[0]);
  const Sample sortedSample(sample.sort());
  return Interval(Point({sortedSample(rank[0], 0)}), Point({sortedSample(rank[1], 0)}));
}


namespace
{
class QuantileConfidenceEvaluation: public EvaluationImplementation
{
public:
  QuantileConfidenceEvaluation(const Scalar alpha,
                               const UnsignedInteger rank,
                               const Bool tail)
    : EvaluationImplementation()
    , alpha_(alpha)
    , rank_(rank)
    , tail_(tail)
  {
    // Nothing to do
  }

  QuantileConfidenceEvaluation * clone() const override
  {
    return new QuantileConfidenceEvaluation(*this);
  }

  Point operator() (const Point & point) const override
  {
    return {DistFunc::pBeta(rank_ + 1, point[0] - rank_, tail_ ? 1.0 - alpha_ : alpha_)};
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
  Bool tail_ = true;
}; // class QuantileConfidenceEvaluation
} // Anonymous namespace


UnsignedInteger QuantileConfidence::computeUnilateralMinimumSampleSize(const UnsignedInteger rank, const Bool tail) const
{
  // Here we have to find the minimal value of N such that
  // 1-\sum_{i=N-r}^N Binomial(i, N)\alpha^i(1-\alpha)^{N-i}>=\beta
  // where:
  //   \alpha = alpha_
  //   \beta  = beta_
  //   r      = rank
  // It rewrites F_{N,alpha}(N-r-1)>=beta where F_{N,alpha} is the CDF of the
  // Binomial(N, alpha) ditribution.
  // Easy case: rank=0, the quantile bound is given by the largest upper statistics. The equation to solve is N=\min{n|1-\alpha^n>=\beta}
  Scalar nApprox = 0.0;
  const Function wilksConstraint(QuantileConfidenceEvaluation(alpha_, rank, tail).clone());
  if (rank == 0)
  {
    if (tail)
      nApprox = std::log1p(-beta_) / std::log(alpha_);
    else
      nApprox = std::log1p(-beta_) / std::log1p(-alpha_);
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
    UnsignedInteger nMax = static_cast<UnsignedInteger>((rank + 0.5 * (alpha_ * aBeta * aBeta + std::abs(aBeta) * std::sqrt(alpha_ * (4.0 * rank + alpha_ * aBeta * aBeta)))) / (1.0 - alpha_));
    // This loop must end as wilksConstraint->1 when n->inf
    while (wilksConstraint(Point({0.0 + nMax}))[0] < beta_)
      // At the beginning of the loop nMax is >= 1 so it increases strictly
      nMax += nMax;
    nApprox = Brent().solve(wilksConstraint, beta_, rank, nMax);
  } // rank > 0
  // Here, nApprox can be very close to an integer (in the sense of: the value of the constraint evaluated at the nearest integer is very close to beta_), in which case the correct answer is round(nApprox), or the answer is the next integer value
  const UnsignedInteger nInf = std::round(std::max(1.0 * rank, nApprox));
  const Scalar constraintInf = wilksConstraint(Point({0.0 + nInf}))[0];
  if (std::abs(constraintInf - beta_) < std::sqrt(SpecFunc::Precision)) return nInf;
  return std::ceil(nApprox);
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
