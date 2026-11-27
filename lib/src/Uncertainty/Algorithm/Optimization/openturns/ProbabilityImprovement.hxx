//                                               -*- C++ -*-
/**
 *  @brief ProbabilityImprovement is the probability of improvement criterion
 *  of the EGO algorithm
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
#ifndef OPENTURNS_PROBABILITYIMPROVEMENT_HXX
#define OPENTURNS_PROBABILITYIMPROVEMENT_HXX

#include "openturns/EfficientGlobalOptimizationCriterionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ProbabilityImprovement
 *
 * Probability of improvement criterion of the EGO algorithm: the probability,
 * under the current Gaussian process, that a new evaluation of the objective
 * beats the best value observed so far. For a minimization,
 *
 *   P[I](x) = Phi((y_min - m(x)) / s(x)),
 *
 * and the mirror Phi((m(x) - y_max) / s(x)) for a maximization, where m(x)
 * and s(x) are the conditional mean and standard deviation of the Gaussian
 * process at x, and Phi is the cumulative distribution function of the
 * standard normal distribution.
 *
 * The criterion was proposed as an infill rule for response surface models:
 * Kushner, Harold J. "A new method of locating the maximum point of an
 * arbitrary multipeak curve in the presence of noise." Journal of Research of
 * the National Bureau of Standards, Section B, 68 (1964), pages 97-106. Its
 * use within efficient global optimization, and the expected improvement it
 * is derived from, are described in: Jones, Donald R., Matthias Schonlau,
 * William J. Welch. "Efficient global optimization of expensive black-box
 * functions." Journal of Global Optimization, 13(4), pages 455-492, 1998.
 *
 * The expected improvement is the first moment of the improvement random
 * variable, max(y_min - F(x), 0); the probability of improvement only states
 * that its sign is favorable, and ignores how much it improves. It is a
 * probability, so it saturates: Phi tends to one as soon as the improvement
 * margin is a couple of standard deviations above zero, which leaves a wide
 * plateau for the inner solver to rank, and uncertainty does not push it
 * above Phi(0) = 1/2, so the rule is dominated by exploitation. Worse, for a
 * differentiable covariance model the conditional standard deviation vanishes
 * at a measured point at the same order as the improvement margin does, so
 * their ratio tends to a positive constant on the side where the surrogate
 * predicts a better value than the best one observed: the maximum of the
 * criterion is approached at the measured point of the best value, and the
 * enrichment concentrates there, adding points arbitrarily close to an
 * already evaluated one. The literature usually multiplies the criterion by an
 * improvement factor to break that plateau; the plain criterion is
 * implemented here, as the benchmark definition states it, and the
 * concentration is documented rather than hidden.
 *
 * The criterion is defined for a noiseless objective: it is a probability with
 * respect to the best value *observed* on the design, which a noise model does
 * not provide. Setting a noise function raises NotYetImplementedException; the
 * augmented expected improvement of ExpectedImprovement handles noisy
 * objectives.
 */

class OT_API ProbabilityImprovement
  : public EfficientGlobalOptimizationCriterionImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  ProbabilityImprovement();

  /** Virtual constructor */
  ProbabilityImprovement * clone() const override;

  /** The probability of improvement, to be maximized by the algorithm */
  Function buildObjective() const override;

  /** String converter */
  String __repr__() const override;

} ; /* class ProbabilityImprovement */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PROBABILITYIMPROVEMENT_HXX */
