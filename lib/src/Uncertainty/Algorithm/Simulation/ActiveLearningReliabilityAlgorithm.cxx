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

#include "openturns/ActiveLearningReliabilityAlgorithm.hxx"
#include "openturns/GaussianProcessFitter.hxx"
#include "openturns/GaussianProcessRegression.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/EventSimulation.hxx"
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
#include "openturns/SimulationResult.hxx"
#include "openturns/ActiveLearningReliabilityFunction.hxx"
#include "openturns/ActiveLearningReliabilityResult.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include <cmath>
#include <mutex>
BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ActiveLearningReliabilityAlgorithm
 */

CLASSNAMEINIT(ActiveLearningReliabilityAlgorithm)

static const Factory<ActiveLearningReliabilityAlgorithm> Factory_ActiveLearningReliabilityAlgorithm;

  /** Default constructor */
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm()
  : EventSimulationImplementation()
  , defaultEvent_(getEvent())
  , functionCallNumber_(0)
  , maximumIterations_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultMaximumIterations"))
  , convergenceUncertaintyFactor_(ResourceMap::GetAsScalar("ActiveLearningReliabilityAlgorithm-DefaultConvergenceUncertaintyFactor"))
  , simulationAlgorithmSeed_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultSimulationAlgorithmSeed"))
  {
    // Default members must be valid for Study save/load of a default instance.
    // Both handles share their implementation until first mutation.
    defaultSimulation_ = ProbabilitySimulationAlgorithm(defaultEvent_);
    simulation_ = defaultSimulation_;
    // Validate the ResourceMap entry with the same rule as the setter
    setMaximumIterations(maximumIterations_);
  }

/** Extract the surrogate specification from a fitter. Only the fitter
 * result exposes the covariance model and the trend basis, and getResult()
 * is non-const (it auto-runs an untrained fitter), so work on a local copy
 * and leave the caller object untouched. A single copy serves both
 * extractions to pay at most one implicit fit. */
static void ExtractSurrogateSpec(const GaussianProcessFitter & fitter,
                                 CovarianceModel & covarianceModel,
                                 Basis & basis)
{
  GaussianProcessFitter fitterCopy(fitter);
  const GaussianProcessFitterResult fitterResult(fitterCopy.getResult());
  covarianceModel = fitterResult.getCovarianceModel();
  basis = fitterResult.getBasis();
}

/** Generic constructor: a single overload for any simulator type.
 * Per-concrete-type overloads are voluntarily absent: the simulator is
 * held by value through the EventSimulation handle and retargeted at each
 * iteration through setEvent (copy-on-write), so new simulators need no
 * algorithm change.
 * The design of experiment and the surrogate specification are sourced from
 * the fitter, the true event from the simulation. */
ActiveLearningReliabilityAlgorithm::ActiveLearningReliabilityAlgorithm(const GaussianProcessFitter & fitter,
                                                                       const EventSimulation & simulation,
                                                                       const ActiveLearningReliabilityFunction & criterion)
  : EventSimulationImplementation(simulation.getEvent())
  , defaultEvent_(simulation.getEvent())
  , defaultSimulation_(simulation)
  , criterion_(criterion)
  , initialInputDoE_(fitter.getInputSample())
  , initialOutputDoE_(fitter.getOutputSample())
  , inputDoE_(initialInputDoE_)
  , outputDoE_(initialOutputDoE_)
  , functionCallNumber_(0)
  , maximumIterations_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultMaximumIterations"))
  , convergenceUncertaintyFactor_(ResourceMap::GetAsScalar("ActiveLearningReliabilityAlgorithm-DefaultConvergenceUncertaintyFactor"))
  , simulationAlgorithmSeed_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultSimulationAlgorithmSeed"))
{
  // Single extraction of the surrogate specification (one implicit fit at most)
  if (inputDoE_.getSize() == 0) throw InvalidArgumentException(HERE) << "ActiveLearningReliabilityAlgorithm: input design of experiment is empty";
  if (inputDoE_.getSize() != outputDoE_.getSize()) throw InvalidArgumentException(HERE) << "ActiveLearningReliabilityAlgorithm: input and output designs have different sizes (" << inputDoE_.getSize() << " vs " << outputDoE_.getSize() << ")";
  if (outputDoE_.getDimension() != 1) throw InvalidArgumentException(HERE) << "ActiveLearningReliabilityAlgorithm: output design must be 1-dimensional";
  // Validate the ResourceMap entry with the same rule as the setter
  setMaximumIterations(maximumIterations_);
  simulation_ = defaultSimulation_;
  try {
    simulation_.setKeepSample(true);
    defaultSimulation_.setKeepSample(true);
  } catch (const NotYetImplementedException &) {
    throw InvalidArgumentException(HERE)
      << "ActiveLearningReliabilityAlgorithm: the simulator does not support setKeepSample. "
      "Consider using one of these or modify the simulator to support setKeepSample.";
  }
  ExtractSurrogateSpec(fitter, covarianceModel_, basis_);
  // The simulator handles share their implementation until first mutation:
  // setEvent/run/setKeepSample detach through copy-on-write, so the caller
  // object is never mutated. The criterion handle shares its
  // implementation until first mutation (copy-on-write on setGaussianProcessRegression).
  // Sample retention is explicitly enabled above: the active learning loop
  // needs the input sample of each inner probability simulation.
}

/* Virtual constructor */
ActiveLearningReliabilityAlgorithm * ActiveLearningReliabilityAlgorithm::clone() const
{
  return new ActiveLearningReliabilityAlgorithm(*this);
}

/* String converter */
String ActiveLearningReliabilityAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << getClassName()
      << " derived from " << EventSimulationImplementation::__repr__()
      << " maximumIterations=" << maximumIterations_
      << " functionCallNumber=" << functionCallNumber_
      << " hasConverged=" << (hasConverged_ ? "true" : "false");
  return oss;
}

/* Event accessor: run() and the uncertainty diagnostics read defaultEvent_,
   so the inherited setEvent() alone would leave them on the previous event.
   The event is also propagated to the wrapped simulator and to the learning
   criterion, whose reliability threshold must follow the new event, and the
   results of a previous run are invalidated. */
void ActiveLearningReliabilityAlgorithm::setEvent(const RandomVector & event)
{
  EventSimulationImplementation::setEvent(event);
  defaultEvent_ = event;
  defaultSimulation_.setEvent(event);
  simulation_ = defaultSimulation_;
  // The criterion selects the enrichment points with respect to the same
  // limit state as the event: keep its threshold synchronized.
  criterion_.setReliabilityThreshold(event.getThreshold());
  // Invalidate the results of a previous run: they describe the previous event
  hasConverged_ = false;
  activeLearningReliabilityResult_ = ActiveLearningReliabilityResult();
  EventSimulationImplementation::setResult(ProbabilitySimulationResultImplementation());
}

/* Single GPR refit from the current design, shared by run() and the diagnostics */
GaussianProcessRegressionResult ActiveLearningReliabilityAlgorithm::fitGaussianProcess() const
{
  GaussianProcessFitter fitter(inputDoE_, outputDoE_, covarianceModel_, basis_);
  fitter.run();
  GaussianProcessRegression algo(fitter.getResult());
  algo.run();
  return algo.getResult();
}

/* Class to retrieve GP plus uncertainty as Function*/
class GPWithUncertainty : public EvaluationImplementation {
public:
  GPWithUncertainty(const GaussianProcessRegressionResult & gprResult, const Scalar kFactor)
    : kFactor_(kFactor)
    , gprCov_(gprResult)
    , gprMetamodel_(gprResult.getMetaModel())
    {}

  GPWithUncertainty * clone() const override
  {
    return new GPWithUncertainty(*this);
  }
  Point operator()(const Point & x) const override
  {
    const Scalar mean = gprMetamodel_(x)[0];
    const Scalar variance = gprCov_.getConditionalMarginalVariance(x);
    return Point(1, mean + kFactor_ * std::sqrt(std::max(variance, 0.0)));
  }
  // Mandatory Function interface
  Sample operator()(const Sample & x) const override
  {
    // Calculation of mean
    const Sample mean = gprMetamodel_(x);
    // Calculation of variance
    const Sample variance = gprCov_.getConditionalMarginalVariance(x);
    // Calculation of k * std
    Sample result(mean.getSize(), mean.getDimension());
    for (UnsignedInteger i = 0; i < mean.getSize(); ++i)
    {
      Scalar stdDev = std::sqrt(std::max(variance(i, 0), 0.0));
      result(i, 0) = mean(i, 0) + kFactor_ * stdDev;
    }
    return result;
  }

  UnsignedInteger getInputDimension() const override
  {
    return gprMetamodel_.getInputDimension();
  }

  UnsignedInteger getOutputDimension() const override
  {
    return 1;
  }

protected:
  Scalar kFactor_ = 0.0;
  GaussianProcessConditionalCovariance gprCov_;
  Function gprMetamodel_;
};

/* Saves the ambient random generator state, applies the run seed once,
   and restores the saved state on destruction, even on exception:
   single-threaded runs stay reproducible without permanently perturbing
   the process-wide stream shared with other threads. The caller holds
   GetActiveLearningSimulationMutex() for the whole lifetime of the guard. */
class RandomGeneratorSeedGuard {
public:
  explicit RandomGeneratorSeedGuard(const UnsignedInteger seed)
    : savedState_(RandomGenerator::GetState())
  {
    RandomGenerator::SetSeed(seed);
  }
  ~RandomGeneratorSeedGuard()
  {
    try
    {
      RandomGenerator::SetState(savedState_);
    }
    catch (...)
    {
      // Nothing to do: never throw in a destructor
    }
  }
private:
  RandomGeneratorState savedState_;
};

/* Process-wide mutex protecting the reseeded inner runs of AK algorithms:
   RandomGenerator is a single global MersenneTwister with no internal lock,
   so two algorithms resetting it concurrently would interleave draws. run()
   holds it for its whole duration, which also protects the save/seed/restore
   sequence of the seed guard; a second run is rejected (see
   ActiveLearningRunLock). Function-static for copy-safety (the algorithm is
   cloned). */
static std::mutex & GetActiveLearningSimulationMutex()
{
  static std::mutex mutex;
  return mutex;
}

/* Lock held by run() for its whole duration. The mutex is not recursive and
   the wrapped model or the criterion may call back into user code. A run that
   finds the lock already taken (nested run on the same thread, or a run in
   another thread) reports an error instead of blocking: a thread that waits
   on the mutex may hold the interpreter lock needed by a Python callback of
   the running algorithm, which would deadlock. */
class ActiveLearningRunLock {
public:
  ActiveLearningRunLock()
    : lock_(GetActiveLearningSimulationMutex(), std::defer_lock)
  {
    // Check the thread-local flag before touching the mutex: a nested run on
    // the same thread must not call try_lock on a mutex it already owns.
    if (isRunning) throw InternalException(HERE) << "ActiveLearningReliabilityAlgorithm::run() must not be called recursively: "
        "a model or a criterion function must not start another active learning run";
    if (!lock_.try_lock()) throw InternalException(HERE) << "ActiveLearningReliabilityAlgorithm::run() is already "
        "running in another thread: concurrent active learning runs are not supported";
    isRunning = true;
  }
  ~ActiveLearningRunLock()
  {
    isRunning = false;
  }
private:
  static thread_local Bool isRunning;
  std::unique_lock<std::mutex> lock_;
};

thread_local Bool ActiveLearningRunLock::isRunning = false;

/* Run an inner simulator with the exact same stream at each call, whatever its
   dynamic type and without testing for it: reset to the run seed, then run.
   The caller must hold GetActiveLearningSimulationMutex() (run() does so for
   its whole duration), so no other AK run can interleave a reset or draws. */
static void RunInnerWithFixedStream(EventSimulation & simulation,
                                    const UnsignedInteger seed)
{
  RandomGenerator::SetSeed(seed);
  simulation.run();
}

/* Check convergence based on probability uncertainty due to GP error */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceProbabilityWithUncertainty(const GaussianProcessRegressionResult & gprResult)
{
  const Point probabilitiesWithUncertainty = computeProbabilityWithUncertainty(gprResult);
  const Scalar meanProbability = probabilitiesWithUncertainty[0];
  const Scalar minusProbability = probabilitiesWithUncertainty[1];
  const Scalar plusProbability =  probabilitiesWithUncertainty[2];
  // The relative widening of the interval is undefined for a zero estimate
  // (probability of 1e-300 or below, or a degenerate surrogate): report
  // non-convergence instead of building an infinite or NaN ratio.
  if (!(std::isfinite(meanProbability)) || !(meanProbability > 0.0)
      || !(std::isfinite(minusProbability)) || !(std::isfinite(plusProbability)))
    return false;
  const Bool convergenceProbability = std::abs((plusProbability - minusProbability) / meanProbability) <= convergenceCriterionThreshold_;

  return convergenceProbability;
}

/* Check convergence based on reliability index uncertainty due to GP error */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceReliabilityIndexWithUncertainty(const GaussianProcessRegressionResult & gprResult)
{
  const Point probabilitiesWithUncertainty = computeProbabilityWithUncertainty(gprResult);
  const Scalar meanProbability = probabilitiesWithUncertainty[0];
  const Scalar minusProbability = probabilitiesWithUncertainty[1];
  const Scalar plusProbability =  probabilitiesWithUncertainty[2];
  // qNormal diverges at 0 and 1, and the relative variation of the
  // reliability index is undefined when the index is zero (probability 0.5)
  const Scalar meanReliabilityIndex = - DistFunc::qNormal(meanProbability);
  const Scalar minusReliabilityIndex = - DistFunc::qNormal(minusProbability);
  const Scalar plusReliabilityIndex = - DistFunc::qNormal(plusProbability);
  if (!(std::isfinite(meanReliabilityIndex)) || !(std::abs(meanReliabilityIndex) > 0.0)
      || !(std::isfinite(minusReliabilityIndex)) || !(std::isfinite(plusReliabilityIndex)))
    return false;
  const Bool convergenceReliabilityIndex = std::abs((plusReliabilityIndex - minusReliabilityIndex) / meanReliabilityIndex) <= convergenceCriterionThreshold_;

  return convergenceReliabilityIndex;
}

/* Compute three probability with GP uncertainty : minus, mean and plus k * std */
Point ActiveLearningReliabilityAlgorithm::computeProbabilityWithUncertainty(const GaussianProcessRegressionResult & gprResult)
{
  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();

  // Creation of functions for threshold event
  GPWithUncertainty GPMean(gprResult, 0.);
  GPWithUncertainty GPMinus(gprResult, - convergenceUncertaintyFactor_);
  GPWithUncertainty GPPlus(gprResult, + convergenceUncertaintyFactor_);

  // Computation of probability with GP mean
  CompositeRandomVector meanEventRV = CompositeRandomVector(GPMean,
                                                            RandomVector(inputDistribution));
  ThresholdEvent meanEvent = ThresholdEvent(meanEventRV,
                                            defaultEvent_.getOperator(),
                                            defaultEvent_.getThreshold());

  EventSimulation simulationMean = defaultSimulation_;

  simulationMean.setEvent(meanEvent);
  RunInnerWithFixedStream(simulationMean, simulationAlgorithmSeed_);
  Scalar meanProbability = simulationMean.getResult().getProbabilityEstimate();

  // Computation of probability with GP mean - k sigma
  CompositeRandomVector minusEventRV(GPMinus,
                                     RandomVector(inputDistribution));

  ThresholdEvent eventMinus = ThresholdEvent(minusEventRV,
                                             defaultEvent_.getOperator(),
                                             defaultEvent_.getThreshold());

  EventSimulation simulationMinus = defaultSimulation_;
  simulationMinus.setEvent(eventMinus);
  RunInnerWithFixedStream(simulationMinus, simulationAlgorithmSeed_);
  Scalar minusProbability = simulationMinus.getResult().getProbabilityEstimate();

  // Computation of probability with GP mean + k sigma
  CompositeRandomVector plusEventRV(GPPlus,
                                    RandomVector(inputDistribution));

  ThresholdEvent eventPlus = ThresholdEvent(plusEventRV,
                                            defaultEvent_.getOperator(),
                                            defaultEvent_.getThreshold());

  EventSimulation simulationPlus = defaultSimulation_;
  simulationPlus.setEvent(eventPlus);
  RunInnerWithFixedStream(simulationPlus, simulationAlgorithmSeed_);
  Scalar plusProbability = simulationPlus.getResult().getProbabilityEstimate();

  Point probabilitiesWithUncertainty = Point({meanProbability, minusProbability, plusProbability});

  return probabilitiesWithUncertainty;
}

/* Check convergence based on stability of history */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceStability(const Scalar currentValue,
                                                                   const Scalar previousValue)
{
  if (!(std::abs(currentValue) > 0.0)) return !(std::abs(previousValue) > 0.0);
  return std::abs(currentValue - previousValue) / std::abs(currentValue) <= convergenceCriterionThreshold_;
}

// Set type of convergence
void ActiveLearningReliabilityAlgorithm::setConvergenceCriterion(const UnsignedInteger typeConvergence)
{
  if (typeConvergence > 4)
    throw InvalidArgumentException(HERE) << "ActiveLearningReliability algorithm convergence criterion (" << typeConvergence << ") must be in [0-4]";

  convergenceCriterion_ = typeConvergence;
}

// Set thresholds for active learning and convergence
void ActiveLearningReliabilityAlgorithm::setConvergenceUncertaintyFactor(const Scalar convergenceUncertaintyFactor)
{
  if (!(convergenceUncertaintyFactor >= 0.0) || !std::isfinite(convergenceUncertaintyFactor))
  {
    throw InvalidArgumentException(HERE) << "convergenceUncertaintyFactor (" << convergenceUncertaintyFactor << ") must be finite and nonnegative";
  }
  convergenceUncertaintyFactor_ = convergenceUncertaintyFactor;
}

void ActiveLearningReliabilityAlgorithm::setConvergenceCriterionThreshold(const Scalar convergenceCriterionThreshold)
{
  if (!(convergenceCriterionThreshold >= 0.0) || !std::isfinite(convergenceCriterionThreshold))
  {
    throw InvalidArgumentException(HERE) << "convergenceCriterionThreshold (" << convergenceCriterionThreshold << ") must be finite and nonnegative";
  }
  convergenceCriterionThreshold_ = convergenceCriterionThreshold;
}

void ActiveLearningReliabilityAlgorithm::setMaximumIterations(const UnsignedInteger maximumIterations)
{
  if (maximumIterations < 1)
  {
    throw InvalidArgumentException(HERE) << "maximumIterations (" << maximumIterations << ") must be at least 1";
  }
  maximumIterations_ = maximumIterations;
}

UnsignedInteger ActiveLearningReliabilityAlgorithm::getMaximumIterations() const
{
  return maximumIterations_;
}

EventSimulation ActiveLearningReliabilityAlgorithm::getSimulationAlgorithm() const
{
  return simulation_;
}

void ActiveLearningReliabilityAlgorithm::setCriterion(const ActiveLearningReliabilityFunction & criterion)
{
  criterion_ = criterion;
}

ActiveLearningReliabilityFunction ActiveLearningReliabilityAlgorithm::getCriterion() const
{
  return criterion_;
}

CovarianceModel ActiveLearningReliabilityAlgorithm::getCovarianceModel() const
{
  return covarianceModel_;
}

Basis ActiveLearningReliabilityAlgorithm::getBasis() const
{
  return basis_;
}

// Getter of functionCallNumber
UnsignedInteger ActiveLearningReliabilityAlgorithm::getFunctionCallNumber() const
{
  return functionCallNumber_;
}

// Convergence flag of the last run
Bool ActiveLearningReliabilityAlgorithm::getHasConverged() const
{
  return hasConverged_;
}

// Getter of DoE inputs and outputs
Sample ActiveLearningReliabilityAlgorithm::getInputDoE() const
{
  return inputDoE_;
}

Sample ActiveLearningReliabilityAlgorithm::getOutputDoE() const
{
  return outputDoE_;
}

// Accessor to Results

void ActiveLearningReliabilityAlgorithm::setResult(const ActiveLearningReliabilityResult &activeLearningReliabilityResult)
{
   activeLearningReliabilityResult_ = activeLearningReliabilityResult;
}


ActiveLearningReliabilityResult ActiveLearningReliabilityAlgorithm::getResult() const
{
  return activeLearningReliabilityResult_;
}

// Accessor to the per-iteration simulation results stored in the result
ActiveLearningReliabilityResult::SimulationResultsCollection ActiveLearningReliabilityAlgorithm::getSimulationResults() const
{
  return activeLearningReliabilityResult_.getSimulationResults();
}


// Accessor to simulationAlgorithmSeed

void ActiveLearningReliabilityAlgorithm::setSimulationAlgorithmSeed(const UnsignedInteger seed)
{
  simulationAlgorithmSeed_ = seed;
}

UnsignedInteger ActiveLearningReliabilityAlgorithm::getSimulationAlgorithmSeed() const
{
  return simulationAlgorithmSeed_;
}


/** Maximum sample size accessor: forwarded to the wrapped simulator */
void ActiveLearningReliabilityAlgorithm::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)
{
  defaultSimulation_.setMaximumOuterSampling(maximumOuterSampling);
}


UnsignedInteger ActiveLearningReliabilityAlgorithm::getMaximumOuterSampling() const
{
  return defaultSimulation_.getMaximumOuterSampling();
}

/** Maximum coefficient of variation accessor: forwarded to the wrapped simulator */
void ActiveLearningReliabilityAlgorithm::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)
{
  defaultSimulation_.setMaximumCoefficientOfVariation(maximumCoefficientOfVariation);
}

Scalar ActiveLearningReliabilityAlgorithm::getMaximumCoefficientOfVariation() const
{
  return defaultSimulation_.getMaximumCoefficientOfVariation();
}

/** Convergence strategy accessor: forwarded to the wrapped simulator */
void ActiveLearningReliabilityAlgorithm::setConvergenceStrategy(const HistoryStrategy & convergenceStrategy)
{
  defaultSimulation_.setConvergenceStrategy(convergenceStrategy);
}

HistoryStrategy ActiveLearningReliabilityAlgorithm::getConvergenceStrategy() const
{
  return defaultSimulation_.getConvergenceStrategy();
}

/** Maximum standard deviation accessor: forwarded to the wrapped simulator */
void ActiveLearningReliabilityAlgorithm::setMaximumStandardDeviation(const Scalar maximumStandardDeviation)
{
  defaultSimulation_.setMaximumStandardDeviation(maximumStandardDeviation);
}
Scalar ActiveLearningReliabilityAlgorithm::getMaximumStandardDeviation() const
{
  return defaultSimulation_.getMaximumStandardDeviation();
}

/** Block size accessor: forwarded to the wrapped simulator */
void ActiveLearningReliabilityAlgorithm::setBlockSize(const UnsignedInteger blockSize)
{
  defaultSimulation_.setBlockSize(blockSize);
}
UnsignedInteger ActiveLearningReliabilityAlgorithm::getBlockSize() const
{
  return defaultSimulation_.getBlockSize();
}



/* Run of the algorithm: restart from the initial design, fit the GPR, enrich
   until the learning criterion converges or the iteration budget is exhausted.
   Budget exhaustion exits with hasConverged()=false. Each inner run resets the
   generator to the run seed; the whole run holds the process-wide mutex, a
   concurrent or nested run is rejected, and the ambient generator state is
   restored on exit.
   The candidate pool is the input sample of the current inner simulation: it
   is redrawn at each iteration by adaptive simulators (SubsetSampling,
   cross-entropy importance sampling), and fixed by non-adaptive ones. */
void ActiveLearningReliabilityAlgorithm::run()
{
  // A Study may load a value that did not go through setMaximumIterations()
  if (maximumIterations_ < 1)
    throw InvalidArgumentException(HERE) << "maximumIterations (" << maximumIterations_ << ") must be at least 1";
  // The seed guard and every inner run share the process-wide random
  // generator, so the mutex is held for the whole run
  ActiveLearningRunLock runLock;

  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();
  const Function model(defaultEvent_.getFunction());
  // Single seeding point of the run, restored on exit even on exception
  const RandomGeneratorSeedGuard seedGuard(simulationAlgorithmSeed_);

  // Restart semantics: each run starts again from the initial design
  functionCallNumber_ = 0;
  probabilityHistory_ = Point();
  reliabilityIndexHistory_ = Point();
  inputDoE_ = initialInputDoE_;
  outputDoE_ = initialOutputDoE_;
  simulation_ = defaultSimulation_;
  activeLearningReliabilityResult_ = ActiveLearningReliabilityResult();
  hasConverged_ = false;

  Bool convergenceStatus = false;
  ActiveLearningReliabilityResult::SimulationResultsPersistentCollection simulationResults;
  GaussianProcessRegressionResult finalGPRResult;

  while ((!convergenceStatus) && (functionCallNumber_ < maximumIterations_))
  {
    // Fit the current surrogate once per iteration; its result serves both
    // the probability estimate and the convergence checks
    finalGPRResult = fitGaussianProcess();

    //Estimate probability with current GP
    Function newGPRmetamodel =  finalGPRResult.getMetaModel();
    CompositeRandomVector newRandomVector = CompositeRandomVector(newGPRmetamodel,
                                                                  RandomVector(inputDistribution));

    ThresholdEvent newEvent = ThresholdEvent(newRandomVector,
                                             defaultEvent_.getOperator(),
                                             defaultEvent_.getThreshold());

    EventSimulation currentSimulation = defaultSimulation_;
    currentSimulation.setEvent(newEvent);
    RunInnerWithFixedStream(currentSimulation, simulationAlgorithmSeed_);

    simulation_ = currentSimulation;
    Sample candidatePool = currentSimulation.getInputSample();

    // Compute active learning values on the candidate pool
    criterion_.setGaussianProcessRegression(finalGPRResult);
    Sample activeLearningValues = criterion_(candidatePool, inputDoE_);

    // Store history
    Scalar currentProbabilityEstimate = currentSimulation.getResult().getProbabilityEstimate();
    probabilityHistory_.add(currentProbabilityEstimate);
    Scalar currentReliabilityIndex = - DistFunc::qNormal(currentProbabilityEstimate);
    reliabilityIndexHistory_.add(currentReliabilityIndex);
    // Store the inner result with full dynamic type (cloned on wrap,
    // later runs never mutate stored elements)
    // Restricted history bridge (friend of EventSimulationImplementation):
    // keeps the full dynamic type without public API exposure
    simulationResults.add(currentSimulation.getImplementation()->getHistoryResult());

    // Check convergence with the selected criterion
    switch (convergenceCriterion_)
    {
      case PROBABILITY_UNCERTAINTY:
        // check convergence of uncertainty of probability estimate
        convergenceStatus = checkConvergenceProbabilityWithUncertainty(finalGPRResult);
        break;
      case RELIABILITY_INDEX_UNCERTAINTY:
        // check convergence of uncertainty of reliability index
        convergenceStatus = checkConvergenceReliabilityIndexWithUncertainty(finalGPRResult);
        break;
      case ACTIVE_LEARNING:
        // check convergence of active learning
        convergenceStatus = criterion_.checkConvergenceLearning(activeLearningValues);
        break;
      case PROBABILITY_STABILITY:
      {
        const UnsignedInteger historySize = probabilityHistory_.getSize();
        if (historySize > 1)
          convergenceStatus = checkConvergenceStability(probabilityHistory_[historySize - 1],
                                                        probabilityHistory_[historySize - 2]);
        break;
      }
      case RELIABILITY_INDEX_STABILITY:
      {
        const UnsignedInteger historySize = reliabilityIndexHistory_.getSize();
        if (historySize > 1)
          convergenceStatus = checkConvergenceStability(reliabilityIndexHistory_[historySize - 1],
                                                        reliabilityIndexHistory_[historySize - 2]);
        break;
      }
      default:
        throw InvalidArgumentException(HERE) << "Unknown convergence criterion (" << convergenceCriterion_ << ")";
    }

    // Add infill sample if convergence is not reached
    if (!convergenceStatus)
    {
      // Get input sample to evaluate from the candidate pool
      Sample infillInputSample = criterion_.getInfillSample(candidatePool, activeLearningValues);
      // A criterion may return several points; an empty sample would loop
      // forever while consuming the budget
      if (infillInputSample.getSize() == 0)
        throw InvalidArgumentException(HERE) << "The active learning criterion returned an empty infill sample";
      inputDoE_.add(infillInputSample);
      Sample infillOutputSample = model(infillInputSample);
      outputDoE_.add(infillOutputSample);
      functionCallNumber_ += infillInputSample.getSize();
    }
  }

  hasConverged_ = convergenceStatus;

  // Budget exhausted: the last iteration recorded its estimate before its
  // final enrichment. Refit the surrogate with that point and run one more
  // inner simulation, so that the reported estimate and the per-iteration
  // history match the final design of experiment.
  if (!hasConverged_)
  {
    finalGPRResult = fitGaussianProcess();

    Function finalGPRmetamodel = finalGPRResult.getMetaModel();
    CompositeRandomVector finalRandomVector = CompositeRandomVector(finalGPRmetamodel,
                                                                    RandomVector(inputDistribution));
    ThresholdEvent finalEvent = ThresholdEvent(finalRandomVector,
                                               defaultEvent_.getOperator(),
                                               defaultEvent_.getThreshold());

    EventSimulation finalSimulation = defaultSimulation_;
    finalSimulation.setEvent(finalEvent);
    RunInnerWithFixedStream(finalSimulation, simulationAlgorithmSeed_);
    simulation_ = finalSimulation;

    const Scalar finalProbabilityEstimate = finalSimulation.getResult().getProbabilityEstimate();
    probabilityHistory_.add(finalProbabilityEstimate);
    reliabilityIndexHistory_.add(- DistFunc::qNormal(finalProbabilityEstimate));
    simulationResults.add(finalSimulation.getImplementation()->getHistoryResult());
  }

  const UnsignedInteger historySize = probabilityHistory_.getSize();
  const Scalar currentProbabilityEstimate = probabilityHistory_[historySize - 1];
  const Scalar currentReliabilityIndex = reliabilityIndexHistory_[historySize - 1];
  const Scalar currentVarianceEstimate = simulation_.getResult().getVarianceEstimate();
  const UnsignedInteger currentOuterSampling = simulation_.getResult().getOuterSampling();
  const UnsignedInteger currentBlockSize = simulation_.getResult().getBlockSize();

  const Point probaCI = computeProbabilityWithUncertainty(finalGPRResult);
  // probaCI holds {mean, minus, plus}: the interval spans the minus and
  // plus estimates, in increasing order, and the reliability index
  // beta = -Phi^{-1}(p) is decreasing in p
  const Scalar lowerProbability = std::min(probaCI[1], probaCI[2]);
  const Scalar upperProbability = std::max(probaCI[1], probaCI[2]);
  const Interval probabilityCI = Interval(Point(1, lowerProbability), Point(1, upperProbability));
  const Interval reliabilityCI = Interval(Point(1, - DistFunc::qNormal(upperProbability)), Point(1, - DistFunc::qNormal(lowerProbability)));


  ActiveLearningReliabilityResult results(defaultEvent_,
                                          currentProbabilityEstimate,
                                          currentVarianceEstimate,
                                          currentOuterSampling,
                                          currentBlockSize,
                                          currentReliabilityIndex,
                                          finalGPRResult,
                                          probabilityHistory_,
                                          reliabilityIndexHistory_,
                                          functionCallNumber_,
                                          probabilityCI,
                                          reliabilityCI);
  results.setHasConverged(hasConverged_);
  results.setSimulationResults(simulationResults);
  setResult(results);
  // Mirror the probability estimate into the base result: readers through a
  // generic EventSimulation handle only see the base getResult()
  EventSimulationImplementation::setResult(results);

}

/* Method save() stores the object through the StorageManager */
void ActiveLearningReliabilityAlgorithm::save(Advocate & adv) const
{
  EventSimulationImplementation::save(adv);
  adv.saveAttribute("initialInputDoE_", initialInputDoE_);
  adv.saveAttribute("initialOutputDoE_", initialOutputDoE_);
  adv.saveAttribute("inputDoE_", inputDoE_);
  adv.saveAttribute("outputDoE_", outputDoE_);
  adv.saveAttribute("covarianceModel_", covarianceModel_);
  adv.saveAttribute("basis_", basis_);
  adv.saveAttribute("defaultEvent_", defaultEvent_);
  // Polymorphic simulator: the handle persists the dynamic type through the
  // Factory, so new simulators round-trip without touching this method
  adv.saveAttribute("simulation_", defaultSimulation_);
  // Polymorphic criterion: the handle persists the dynamic type through the
  // Factory, so new criteria round-trip without touching this method
  adv.saveAttribute("criterion_", criterion_);
  adv.saveAttribute("functionCallNumber_", functionCallNumber_);
  adv.saveAttribute("probabilityHistory_", probabilityHistory_);
  adv.saveAttribute("reliabilityIndexHistory_", reliabilityIndexHistory_);
  adv.saveAttribute("convergenceCriterion_", convergenceCriterion_);
  adv.saveAttribute("maximumIterations_", maximumIterations_);
  adv.saveAttribute("hasConverged_", hasConverged_);
  adv.saveAttribute("convergenceCriterionThreshold_", convergenceCriterionThreshold_);
  adv.saveAttribute("convergenceUncertaintyFactor_", convergenceUncertaintyFactor_);
  adv.saveAttribute("simulationAlgorithmSeed_", simulationAlgorithmSeed_);
  adv.saveAttribute("activeLearningReliabilityResult_", activeLearningReliabilityResult_);
}

/* Method load() reloads the object from the StorageManager */
void ActiveLearningReliabilityAlgorithm::load(Advocate & adv)
{
  EventSimulationImplementation::load(adv);
  adv.loadAttribute("initialInputDoE_", initialInputDoE_);
  adv.loadAttribute("initialOutputDoE_", initialOutputDoE_);
  adv.loadAttribute("inputDoE_", inputDoE_);
  adv.loadAttribute("outputDoE_", outputDoE_);
  adv.loadAttribute("covarianceModel_", covarianceModel_);
  adv.loadAttribute("basis_", basis_);
  adv.loadAttribute("defaultEvent_", defaultEvent_);
  // Simulator rebuild through its handle: the Factory restores the dynamic
  // type, so new simulators round-trip without touching this method
  adv.loadAttribute("simulation_", defaultSimulation_);
  simulation_ = defaultSimulation_;
  // Criterion rebuild through its handle: the Factory restores the dynamic
  // type, so new criteria round-trip without touching this method
  adv.loadAttribute("criterion_", criterion_);
  adv.loadAttribute("functionCallNumber_", functionCallNumber_);
  adv.loadAttribute("probabilityHistory_", probabilityHistory_);
  adv.loadAttribute("reliabilityIndexHistory_", reliabilityIndexHistory_);
  adv.loadAttribute("convergenceCriterion_", convergenceCriterion_);
  adv.loadAttribute("maximumIterations_", maximumIterations_);
  // A stored value may bypass the setter validation
  if (maximumIterations_ < 1)
    throw InvalidArgumentException(HERE) << "maximumIterations (" << maximumIterations_ << ") must be at least 1";
  if (adv.hasAttribute("hasConverged_"))
    adv.loadAttribute("hasConverged_", hasConverged_);
  adv.loadAttribute("convergenceCriterionThreshold_", convergenceCriterionThreshold_);
  adv.loadAttribute("convergenceUncertaintyFactor_", convergenceUncertaintyFactor_);
  adv.loadAttribute("simulationAlgorithmSeed_", simulationAlgorithmSeed_);
  adv.loadAttribute("activeLearningReliabilityResult_", activeLearningReliabilityResult_);
}

END_NAMESPACE_OPENTURNS
