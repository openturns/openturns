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
 * (held by value through the EventSimulation handle, retargeted at each
 * iteration through setEvent with copy-on-write) and a single learning
 * criterion, and enriches
 * the design of experiment until the criterion converges or the
 * iteration budget is exhausted. The design of experiment and the
 * surrogate specification are sourced from a GaussianProcessFitter,
 * the true event from the wrapped simulation.
 *
 * The enrichment loop follows the AK-MCS principle, see:
 * Echard B., Gayton N., Lemaire M., AK-MCS: an active learning reliability
 * method combining Kriging and Monte Carlo simulation, Structural Safety,
 * 33(2), 145-154, 2011.
 * The learning criterion is interchangeable, see:
 * Moustapha M., Sudret B., Surrogate-assisted reliability-based design
 * optimization: a survey and a unified modular framework, Structural and
 * Multidisciplinary Optimization, 60(5), 2157-2176, 2019.
*/

class OT_API ActiveLearningReliabilityAlgorithm
  : public EventSimulationImplementation
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

  /** Event accessor: updates both the base event and the event used by run() */
  void setEvent(const RandomVector & event) override;

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

  /** Maximum number of enrichment iterations (active-learning budget) */
  void setMaximumIterations(const UnsignedInteger maximumIterations);
  UnsignedInteger getMaximumIterations() const;

  void setSimulationAlgorithmSeed(const UnsignedInteger seed);

  UnsignedInteger getSimulationAlgorithmSeed() const;

  EventSimulation getSimulationAlgorithm() const;

  /** Learning criterion accessor (value semantics, dynamic type preserved) */
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

  /** Accessor to the per-iteration simulation results stored in the result */
  ActiveLearningReliabilityResult::SimulationResultsCollection getSimulationResults() const;

  /** The simulation settings below are forwarded to the wrapped simulator:
      they configure the inner probability simulations, not the active
      learning loop, whose budget is set by setMaximumIterations(). */
  void setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling);
  UnsignedInteger getMaximumOuterSampling() const;

  void setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation) override;
  Scalar getMaximumCoefficientOfVariation() const;

  void setConvergenceStrategy(const HistoryStrategy & convergenceStrategy);
  HistoryStrategy getConvergenceStrategy() const;

  void setMaximumStandardDeviation(const Scalar maximumStandardDeviation);
  Scalar getMaximumStandardDeviation() const;

  void setBlockSize(const UnsignedInteger blockSize) override;
  UnsignedInteger getBlockSize() const;

protected:

  /** Accessor to results */
  void setResult(const ActiveLearningReliabilityResult & activeLearningReliabilityResult);

  /** Convergence checks, using the Gaussian process already fitted by run() */
  Bool checkConvergenceProbabilityWithUncertainty(const GaussianProcessRegressionResult & gprResult);

  Bool checkConvergenceReliabilityIndexWithUncertainty(const GaussianProcessRegressionResult & gprResult);

  Bool checkConvergenceStability(const Scalar currentValue,
                                 const Scalar previousValue);

  Point computeProbabilityWithUncertainty(const GaussianProcessRegressionResult & gprResult);

  /** Single GPR refit point used by run() and the uncertainty diagnostics */
  GaussianProcessRegressionResult fitGaussianProcess() const;

  RandomVector defaultEvent_;
  EventSimulation defaultSimulation_;
  EventSimulation simulation_;
  ActiveLearningReliabilityFunction criterion_;

  /** Initial design, restored at the beginning of each run() */
  Sample initialInputDoE_;
  Sample initialOutputDoE_;
  Sample currentInputSample_;
  Sample inputDoE_;
  Sample outputDoE_;
  CovarianceModel covarianceModel_;
  Basis basis_;
  UnsignedInteger functionCallNumber_ = 0;
  Point probabilityHistory_;
  Point reliabilityIndexHistory_;
  UnsignedInteger convergenceCriterion_ = ACTIVE_LEARNING;
  Scalar convergenceCriterionThreshold_ = 0.1;
  UnsignedInteger maximumIterations_ = 0;
  Bool hasConverged_ = false;

  Scalar convergenceUncertaintyFactor_ = 0.0;
  UnsignedInteger simulationAlgorithmSeed_ = 0;

  ActiveLearningReliabilityResult activeLearningReliabilityResult_;

} ; /* class ActiveLearningReliabilityAlgorithm */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYALGORITHM_HXX */
