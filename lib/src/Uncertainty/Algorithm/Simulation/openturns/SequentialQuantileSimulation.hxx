//                                               -*- C++ -*-
/**
 *  @brief Sequential quantile simulation
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
#ifndef OPENTURNS_SEQUENTIALQUANTILESIMULATION_HXX
#define OPENTURNS_SEQUENTIALQUANTILESIMULATION_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SequentialQuantileSimulation
 *
 * Quantile estimator with an anytime-valid confidence sequence.  The
 * algorithm maintains a distribution-free confidence band for the
 * distribution function from the Dvoretzky-Kiefer-Wolfowitz-Massart
 * inequality, inverts the band into a confidence interval for the
 * quantile, and stops when the interval half-width reaches the target or
 * when the evaluation budget is exhausted.  A union bound over the
 * geometric grid of candidate sample sizes keeps the simultaneous
 * coverage at least beta, so the interval remains valid at the
 * data-dependent stopping time.
 *
 * References:
 * A. Dvoretzky, J. Kiefer and J. Wolfowitz. Asymptotic minimax
 * character of the sample distribution function and of the classical
 * multinomial estimator. The Annals of Mathematical Statistics,
 * 27(3):642-669, 1956. The article states the inequality that gives the
 * time-uniform band for the distribution function.
 * P. Massart. The tight constant in the Dvoretzky-Kiefer-Wolfowitz
 * inequality. The Annals of Probability, 18(3):1269-1283, 1990. The
 * article gives the constant 2 used by the band.
 * Y. S. Chow and H. Robbins. On the asymptotic theory of fixed-width
 * sequential confidence intervals for the mean. The Annals of
 * Mathematical Statistics, 36(2):457-462, 1965. The article analyses
 * fixed-width sequential stopping, which motivates the budget and
 * target rules of the algorithm.
 * S. R. Howard and A. Ramdas. Sequential estimation of quantiles with
 * applications to A/B testing and best-arm identification. Bernoulli,
 * 28(3):1704-1728, 2022. The article proposes a tighter quantile
 * confidence sequence under bounded or sub-Gaussian assumptions; it is
 * not implemented here, and the distribution-free band remains the
 * coverage assumption recorded in the result.
 */
class OT_API SequentialQuantileSimulation
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  SequentialQuantileSimulation();

  /** Constructor with parameters */
  SequentialQuantileSimulation(const RandomVector & output,
                               const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                               const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  SequentialQuantileSimulation * clone() const override;

  /** Target half-width accessor */
  void setTargetHalfWidth(const Scalar halfWidth);
  Scalar getTargetHalfWidth() const;
  Bool hasTargetHalfWidth() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Run the sequential experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  Scalar targetHalfWidth_ = ResourceMap::GetAsScalar("SequentialQuantileSimulation-DefaultTargetHalfWidth");

}; /* class SequentialQuantileSimulation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SEQUENTIALQUANTILESIMULATION_HXX */
