//                                               -*- C++ -*-
/**
 *  @brief Conditional sampler shared by the threshold quantile algorithms
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
#ifndef OPENTURNS_QUANTILECONDITIONALSAMPLER_HXX
#define OPENTURNS_QUANTILECONDITIONALSAMPLER_HXX

#include "openturns/Function.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/ComparisonOperator.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuantileConditionalSampler
 *
 * This internal component performs the Metropolis-Hastings transitions
 * used by the subset and last-particle quantile estimators.  The
 * algorithm works in the standard space of the input distribution: the
 * output is composed with the inverse isoprobabilistic transformation
 * and the target measure is the standard normal distribution.  A
 * transition is a componentwise symmetric random walk followed by an
 * accept/reject step that enforces the threshold event.  The kernel is
 * the one used by SubsetSampling, factored here so that both threshold
 * quantile algorithms share it.
 */
class OT_API QuantileConditionalSampler
{
public:
  /** Default constructor */
  QuantileConditionalSampler();

  /** Constructor from the scalar output */
  QuantileConditionalSampler(const RandomVector & output,
                             const Scalar proposalRange,
                             const UnsignedInteger burnIn);

  /** Build a composite representation of a scalar output */
  static CompositeRandomVector buildCompositeOutput(const RandomVector & output);

  /** Standard-space population */
  Sample generatePopulation(const UnsignedInteger size) const;

  /** Evaluate the output in standard space */
  Sample evaluate(const Sample & standardSample) const;
  Scalar evaluate(const Point & standardPoint) const;

  /** Draw one point from the conditional distribution */
  Point sample(const Point & start,
               const ComparisonOperator & op,
               const Scalar threshold) const;

  /** Dimension accessor */
  UnsignedInteger getDimension() const;

  /** Proposal range accessor */
  Scalar getProposalRange() const;
  void setProposalRange(const Scalar proposalRange);

  /** Burn-in accessor */
  UnsignedInteger getBurnIn() const;
  void setBurnIn(const UnsignedInteger burnIn);

  /** MCMC diagnostics */
  void resetDiagnostics() const;
  Scalar getAcceptanceRate() const;
  UnsignedInteger getTransitionCount() const;
  UnsignedInteger getAcceptedTransitionCount() const;
  UnsignedInteger getEvaluationCount() const;

  /** String converter */
  String __repr__() const;

private:
  /** One constrained Metropolis-Hastings transition */
  Point transition(const Point & start,
                   const ComparisonOperator & op,
                   const Scalar threshold) const;

  Function standardFunction_;
  UnsignedInteger dimension_ = 1;
  Scalar proposalRange_ = 2.0;
  UnsignedInteger burnIn_ = 1;
  mutable UnsignedInteger proposedTransitions_ = 0;
  mutable UnsignedInteger acceptedTransitions_ = 0;
  mutable UnsignedInteger evaluationCount_ = 0;

}; /* class QuantileConditionalSampler */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILECONDITIONALSAMPLER_HXX */
