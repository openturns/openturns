//                                               -*- C++ -*-
/**
 *  @brief WatsonBarnesCriterion is the second Watson and Barnes infill
 *  criterion of the EGO algorithm
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
#ifndef OPENTURNS_WATSONBARNESCRITERION_HXX
#define OPENTURNS_WATSONBARNESCRITERION_HXX

#include "openturns/EfficientGlobalOptimizationCriterionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WatsonBarnesCriterion
 *
 * Second Watson and Barnes infill criterion (WB2) of the EGO algorithm: the
 * expected improvement over the best value observed so far, summed with the
 * negative conditional mean of the Gaussian process, so that a candidate is
 * rewarded both for being expected to improve and for being predicted low.
 * For a minimization,
 *
 *   WB2(x) = -m(x) + EI(x)
 *
 * where
 *
 *   EI(x) = delta * Phi(delta / s(x)) + s(x) * phi(delta / s(x)),
 *   delta = y_min - m(x),
 *
 * and the mirror WB2(x) = m(x) + EI(x), with delta = m(x) - y_max, for a
 * maximization. The algorithm always maximizes the objective, so the
 * exploitation term is the opposite of the conditional mean in a minimization.
 *
 * The criterion is the second infill rule of:
 * Watson, Alan G., and Randal J. Barnes. "Infill sampling criteria to locate
 * extremes." (1995).
 *
 * Expected improvement definition, the term WB2 adds to the predicted value:
 * Jones, Donald R., Matthias Schonlau, William J. Welch. "Efficient global
 * optimization of expensive black-box functions." Journal of Global
 * Optimization, 13(4), pages 455-492, 1998.
 *
 * Algorithmic difficulty: the two terms of WB2 are summed with a fixed weight
 * of one. Both carry the scale of the objective, and their magnitudes are not
 * related to each other: the expected improvement is bounded by the dispersion
 * of the surrogate, while the conditional mean term spans the whole range of
 * the objective. So the balance between exploitation and exploration depends
 * on the problem rather than on the surrogate, and cannot be tuned: that is
 * exactly the difficulty the scaled variant WB2S addresses, by multiplying the
 * expected improvement by a factor computed from its maximum over the domain
 * found by a multi-start, which needs the joint distribution of the problem and
 * therefore an interface the current criterion does not expose.
 *
 * The criterion is defined for a noiseless objective, like the probability of
 * improvement and the lower confidence bound: it is built on the best value
 * *observed* on the design, which a noise model does not provide. Setting a
 * noise function raises NotYetImplementedException; the augmented expected
 * improvement of ExpectedImprovement handles noisy objectives.
 */

class OT_API WatsonBarnesCriterion
  : public EfficientGlobalOptimizationCriterionImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  WatsonBarnesCriterion();

  /** Virtual constructor */
  WatsonBarnesCriterion * clone() const override;

  /** The Watson and Barnes criterion, to be maximized by the algorithm */
  Function buildObjective() const override;

  /** String converter */
  String __repr__() const override;

} ; /* class WatsonBarnesCriterion */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WATSONBARNESCRITERION_HXX */
