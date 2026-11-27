//                                               -*- C++ -*-
/**
 *  @brief ExpectedImprovement is the expected improvement criterion of the EGO
 *  algorithm, with its augmented variant for noisy objectives
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
#ifndef OPENTURNS_EXPECTEDIMPROVEMENT_HXX
#define OPENTURNS_EXPECTEDIMPROVEMENT_HXX

#include "openturns/EfficientGlobalOptimizationCriterionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExpectedImprovement
 *
 * Expected improvement criterion, the historical enrichment rule of the EGO
 * algorithm.
 *
 * Without a noise function, the reference level is the best objective value
 * observed so far, and the criterion is the usual expected improvement of the
 * Gaussian process over that value.
 *
 * With a noise function, the true best value is unknown, so the reference level
 * is a quantile of the conditional prediction over the current design,
 * min or max of m(x_i) + c * s(x_i) according to the direction of the problem,
 * c being the AEI tradeoff. The expected improvement is then scaled by the
 * fraction of the predictive variance that is not noise, which is the augmented
 * expected improvement (AEI) of Huang et al. (2006).
 */
class OT_API ExpectedImprovement
  : public EfficientGlobalOptimizationCriterionImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  ExpectedImprovement();

  /** Constructor with the AEI tradeoff constant */
  explicit ExpectedImprovement(const Scalar aeiTradeoff);

  /** Virtual constructor */
  ExpectedImprovement * clone() const override;

  /** AEI tradeoff constant accessor */
  void setAEITradeoff(const Scalar aeiTradeoff);

  Scalar getAEITradeoff() const;

  /** The improvement criterion, to be maximized by the algorithm */
  Function buildObjective() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

  /** Reference level of the improvement, computed on the current state */
  Scalar computeReferenceLevel() const;

  /** AEI tradeoff constant u(x) = m(x) + c * s(x) */
  Scalar aeiTradeoff_ = 0.0;

} ; /* class ExpectedImprovement */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXPECTEDIMPROVEMENT_HXX */
