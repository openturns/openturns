//                                               -*- C++ -*-
/**
 *  @brief LowerConfidenceBound is the lower confidence bound criterion of the
 *  EGO algorithm
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
#ifndef OPENTURNS_LOWERCONFIDENCEBOUND_HXX
#define OPENTURNS_LOWERCONFIDENCEBOUND_HXX

#include "openturns/EfficientGlobalOptimizationCriterionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LowerConfidenceBound
 *
 * Lower confidence bound criterion of the EGO algorithm: a pessimistic
 * estimate of the objective at x, the conditional mean of the Gaussian process
 * lowered by a multiple of its conditional standard deviation, to be minimized
 *
 *   LCB(x) = m(x) - k * s(x)
 *
 * where k is the conservative factor relative to the standard deviation.
 * Because the algorithm always maximizes the objective returned by
 * buildObjective(), a minimization uses -m(x) + k * s(x), and a maximization
 * mirrors it into m(x) + k * s(x), so the value recorded for the criterion is
 * the negated confidence bound, not an improvement.
 *
 * The criterion is the confidence-bound infill rule of
 * Cox, Dennis D., and Susan John. "A statistical method for global
 * optimization." [Proceedings] 1992 IEEE International Conference on Systems,
 * Man, and Cybernetics. IEEE, 1992.
 *
 * Algorithmic difficulties:
 *
 * - the conservative factor k is the whole exploration/exploitation tradeoff
 *   in a single number: a small k turns the criterion into a plain predicted
 *   value rule (exploitation), a large k into a variance rule (exploration).
 *   There is no canonical value, so k is a declared parameter with a default,
 *   not a derived quantity.
 *
 * - the criterion does not use the best value observed so far: LCB is a bound
 *   on the objective value itself, not a comparison against the best observed
 *   value. Its value therefore lives on the scale of the objective, whereas
 *   the expected improvement and the probability of improvement live on the
 *   scale of the improvement, so the criterion values of a run are not
 *   comparable across criteria, and for LCB not even across iterations, since
 *   the surrogate refits and m, s move with it. A convergence test on the
 *   criterion value would be a new tuning constant, so LCB declares no
 *   convergence and leaves the stopping rules to the algorithm.
 *
 * - with k * s as the only exploration term, the maximizer of the objective
 *   can be an already measured point, where s vanishes and the objective is
 *   the negated measured value. The expected improvement avoids this by
 *   vanishing with the predictive variance; the confidence bound does not, so
 *   the enrichment can add a point already evaluated, which makes the refit
 *   ill-conditioned. Deduplicating the enrichment point belongs to the
 *   algorithm, not to the criterion.
 *
 * The criterion refuses a noise function, like the probability of improvement:
 * the benchmark flags both as not compatible with the noise model and leaves
 * the question open. The confidence bound is arguably the least affected,
 * since a Gaussian process fitted with an observation noise already contains
 * that noise in s(x), which would make k * s a bound on the observed values;
 * whether the bound should instead be on the true objective, which would need
 * a noise-variance discount, is an unresolved design question, not an
 * implementation choice.
 */

class OT_API LowerConfidenceBound
  : public EfficientGlobalOptimizationCriterionImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  LowerConfidenceBound();

  /** Constructor with the conservative factor */
  explicit LowerConfidenceBound(const Scalar conservativeFactor);

  /** Virtual constructor */
  LowerConfidenceBound * clone() const override;

  /** Conservative factor accessor */
  void setConservativeFactor(const Scalar conservativeFactor);

  Scalar getConservativeFactor() const;

  /** The confidence bound to maximize, to be maximized by the algorithm */
  Function buildObjective() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Conservative factor of the bound u(x) = m(x) - k * s(x) */
  Scalar conservativeFactor_ = 0.0;

} ; /* class LowerConfidenceBound */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LOWERCONFIDENCEBOUND_HXX */
