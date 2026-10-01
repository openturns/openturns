//                                               -*- C++ -*-
/**
 *  @brief class for ActiveLearningReliabilityResult
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
#ifndef OPENTURNS_ACTIVELEARNINGRELIABILITYRESULT_HXX
#define OPENTURNS_ACTIVELEARNINGRELIABILITYRESULT_HXX


#include "openturns/ProbabilitySimulationResult.hxx"
#include "openturns/GenericSimulationResult.hxx"
#include "openturns/GaussianProcessRegressionResult.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityResult
 *
 * ActiveLearningReliabilityResult is a class to store all the results provided by ActiveLearningReliabilityAlgorithm.
 * It derives from ProbabilitySimulationResult so that a generic EventSimulation
 * handle sees the failure probability estimate through the base result
 * (mirror rule), while the active-learning details remain available here.
 */
class OT_API ActiveLearningReliabilityResult
  : public ProbabilitySimulationResult
{

  CLASSNAME
public:

  typedef Collection<GenericSimulationResult> SimulationResultsCollection;
  typedef PersistentCollection<GenericSimulationResult> SimulationResultsPersistentCollection;

  /** Default constructor */
  ActiveLearningReliabilityResult();

  /** Standard constructor */
  ActiveLearningReliabilityResult(const Scalar probabilityEstimate,
                                  const Scalar reliabilityIndex,
                                  const GaussianProcessRegressionResult & gprResult,
                                  const Point & probabilityHistory,
                                  const Point & reliabilityIndexHistory,
                                  const UnsignedInteger functionCallNumber,
                                  const Interval probabilityCI,
                                  const Interval reliabilityIndexCI);

  /** Full constructor with event and simulation metadata for the base result */
  ActiveLearningReliabilityResult(const RandomVector & event,
                                  const Scalar probabilityEstimate,
                                  const Scalar varianceEstimate,
                                  const UnsignedInteger outerSampling,
                                  const UnsignedInteger blockSize,
                                  const Scalar reliabilityIndex,
                                  const GaussianProcessRegressionResult & gprResult,
                                  const Point & probabilityHistory,
                                  const Point & reliabilityIndexHistory,
                                  const UnsignedInteger functionCallNumber,
                                  const Interval probabilityCI,
                                  const Interval reliabilityIndexCI);
  /** Virtual constructor */
  ActiveLearningReliabilityResult * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  // Reliability Index
  Scalar getReliabilityIndex() const;

  void setReliabilityIndex(const Scalar reliabilityIndex);

  // GPR Result
  GaussianProcessRegressionResult getGprResult() const;

  void setGprResult(const GaussianProcessRegressionResult & gprResult);

  // Probability History
  Point getProbabilityHistory() const;

  void setProbabilityHistory(const Point & probabilityHistory);

  // Reliability Index History
  Point getReliabilityIndexHistory() const;

  void setReliabilityIndexHistory(const Point & reliabilityIndexHistory);

  // Function call number
  UnsignedInteger getFunctionCallNumber() const;

  void setFunctionCallNumber(const UnsignedInteger functionCallNumber);

  // Per-iteration inner simulation results, generic level (always available)
  SimulationResultsCollection getSimulationResults() const;

  void setSimulationResults(const SimulationResultsCollection & simulationResults);

  // Convergence flag: true if the learning criterion converged within budget
  Bool getHasConverged() const;

  void setHasConverged(const Bool hasConverged);

  // Probability confidence interval
  Interval getProbabilityConfidenceInterval() const;

  void setProbabilityConfidenceInterval(const Interval probabilityCI);

  // Reliability index confidence interval
  Interval getReliabilityIndexConfidenceInterval() const ;

  void setReliabilityIndexConfidenceInterval(const Interval reliabilityIndexCI);

protected:

  // Reliability index associated to the probability estimate
  Scalar reliabilityIndex_ = 0.0;

  // Gaussian Process Regressor
  GaussianProcessRegressionResult gprResult_;

  // Probability and reliability index histories
  Point probabilityHistory_;
  Point reliabilityIndexHistory_;

  // Number of true model evaluations performed during enrichment
  UnsignedInteger functionCallNumber_ = 0;

  // Per-iteration inner simulation results with full dynamic type
  SimulationResultsPersistentCollection simulationResults_;

  // Convergence flag
  Bool hasConverged_ = false;

  // Probability confidence interval
  Interval probabilityCI_;

  // Reliability index confidence interval
  Interval reliabilityIndexCI_;

}; /* class ActiveLearningReliabilityResult */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ACTIVELEARNINGRELIABILITYRESULT_HXX */
