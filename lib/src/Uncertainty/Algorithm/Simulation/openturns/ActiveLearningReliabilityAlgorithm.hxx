//                                               -*- C++ -*-
/**
 *  @brief ActiveLearningReliabilityAlgorithm implements general purpose
 *  active learning scheme for reliability algorithms
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

#ifndef OPENTURNS_ACTIVELEARNINGRELIABILITYALGORITHM_HXX
#define OPENTURNS_ACTIVELEARNINGRELIABILITYALGORITHM_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/GaussianProcessFitter.hxx"
#include "openturns/CovarianceModel.hxx"
#include "openturns/Basis.hxx"
#include "openturns/ActiveLearningReliabilityFunction.hxx"
#include "openturns/ActiveLearningReliabilityResult.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class ActiveLearningReliabilityAlgorithm
 *
 * Generic active-learning reliability algorithm (AK family).
 * It owns a single wrapped EventSimulation of any concrete type
 * (deep-copied at construction, retargeted at each iteration through
 * the virtual setEvent) and a single learning criterion, and enriches
 * the design of experiment until the criterion converges or the
 * iteration budget is exhausted. The design of experiment and the
 * surrogate specification are sourced from a GaussianProcessFitter,
 * the true event from the wrapped simulation.
*/

class OT_API ActiveLearningReliabilityAlgorithm
  : public EventSimulation
{

  CLASSNAME
public:

  typedef Collection<Scalar> ScalarCollection;

  /** Default constructor */
  ActiveLearningReliabilityAlgorithm();

  /** Single constructor from a parameterized fitter, any simulator
      through the base type (no per-type overload) and a criterion.
      The design and surrogate specification come from the fitter,
      the true event from the simulation. */
  ActiveLearningReliabilityAlgorithm(const GaussianProcessFitter & fitter,
                                     const EventSimulation & simulation,
                                     const ActiveLearningReliabilityFunction & criterion);

  /** Virtual constructor */
  ActiveLearningReliabilityAlgorithm * clone() const override;

  /** String converter */
  String __repr__() const override;

  void run() override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  enum convergenceCriterion {PROBABILITY_UNCERTAINTY = 0,
                             RELIABILITY_INDEX_UNCERTAINTY = 1,
                             ACTIVE_LEARNING = 2,
                             PROBABILITY_STABILITY = 3,
                             RELIABILITY_INDEX_STABILITY = 4};

  // Accessor to convergence attributes
  void setConvergenceCriterion(const UnsignedInteger convergenceCriterion = ACTIVE_LEARNING);

  void setConvergenceUncertaintyFactor(const Scalar convergenceUncertaintyFactor);

  void setConvergenceCriterionThreshold(const Scalar convergenceCriterionThreshold);

  void setSimulationBudget(const UnsignedInteger simulationBudget);

  /** Maximum number of enrichment iterations (active-learning budget) */
  void setMaximumIterations(const UnsignedInteger maximumIterations);
  UnsignedInteger getMaximumIterations() const;

  void setSimulationAlgorithmSeed(const UnsignedInteger seed);

  UnsignedInteger getSimulationAlgorithmSeed() const;

  Pointer<EventSimulation> getSimulationAlgorithm() const;

  /** Learning criterion accessor (cloned on set, single owner) */
  void setCriterion(const ActiveLearningReliabilityFunction & criterion);
  ActiveLearningReliabilityFunction getCriterion() const;

  /** Surrogate specification accessors */
  CovarianceModel getCovarianceModel() const;
  Basis getBasis() const;

  UnsignedInteger getFunctionCallNumber() const;

  /** Convergence flag of the last run */
  Bool getHasConverged() const;

  Sample getInputDoE() const;

  Sample getOutputDoE() const;

  /** Accessor to results: hides the base result (as NAIS does) and mirrors
      the probability estimate into the base result at the end of run() */
  ActiveLearningReliabilityResult getResult() const;

protected:

  /** Accessor to results */
  void setResult(const ActiveLearningReliabilityResult & activeLearningReliabilityResult);

  /** Convergence checks */
  Bool checkConvergenceProbabilityWithUncertainty();

  Bool checkConvergenceReliabilityIndexWithUncertainty();

  Bool checkConvergenceStability(const Scalar currentValue,
                                 const Scalar previousValue);

  Point computeProbabilityWithUncertainty();

  /** Single GPR refit point used by run() and the uncertainty diagnostics */
  GaussianProcessRegressionResult fitGaussianProcess() const;

  RandomVector defaultEvent_;
  Pointer<EventSimulation> p_defaultSimulationAlgorithm_;
  Pointer<EventSimulation> p_simulationAlgorithm_;
  Pointer<ActiveLearningReliabilityFunction> p_activeLearningFunction;

  Sample currentInputSample_;
  Sample inputDoE_;
  Sample outputDoE_;
  CovarianceModel covarianceModel_;
  Basis basis_;
  UnsignedInteger functionCallNumber_ = 0;
  Point probabilityHistory_;
  Point reliabilityIndexHistory_;
  UnsignedInteger convergenceCriterion_ = 2; // by default convergenceCriterion_ is set to active learning
  UnsignedInteger simulationBudget_ = 0;
  Scalar convergenceCriterionThreshold_ = 0.0;
  UnsignedInteger maximumIterations_ = 0;
  Bool hasConverged_ = false;

  Scalar convergenceUncertaintyFactor_ = 0.0;
  UnsignedInteger simulationAlgorithmSeed_ = 0;

  ActiveLearningReliabilityResult activeLearningReliabilityResult_;

  /** Maximum sample size accessor */
  void setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling);
  UnsignedInteger getMaximumOuterSampling() const;

  /** Maximum coefficient of variation accessor */
  void setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation) override;
  Scalar getMaximumCoefficientOfVariation() const;

  /** Convergence strategy accessor */
  void setConvergenceStrategy(const HistoryStrategy & convergenceStrategy);
  HistoryStrategy getConvergenceStrategy() const;

  /** Maximum standard deviation accessor */
  void setMaximumStandardDeviation(const Scalar maximumStandardDeviation);
  Scalar getMaximumStandardDeviation() const;

  /** Block size accessor */
  void setBlockSize(const UnsignedInteger blockSize) override;
  UnsignedInteger getBlockSize() const;

} ; /* class ActiveLearningReliabilityAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYALGORITHM_HXX */
