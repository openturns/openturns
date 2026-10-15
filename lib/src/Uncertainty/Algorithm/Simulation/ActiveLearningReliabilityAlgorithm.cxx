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
  , convergenceCriterion_(2)
  , simulationBudget_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultMaximumIterations"))
  , convergenceCriterionThreshold_(0.1)
  , maximumIterations_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultMaximumIterations"))
  , hasConverged_(false)
  , convergenceUncertaintyFactor_(ResourceMap::GetAsScalar("ActiveLearningReliabilityAlgorithm-DefaultConvergenceUncertaintyFactor"))
  , simulationAlgorithmSeed_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultSimulationAlgorithmSeed"))
  {
    // Default members must be valid for Study save/load of a default instance.
    // Both handles share their implementation until first mutation.
    defaultSimulation_ = ProbabilitySimulationAlgorithm(defaultEvent_);
    simulation_ = defaultSimulation_;
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
  , inputDoE_(fitter.getInputSample())
  , outputDoE_(fitter.getOutputSample())
  , functionCallNumber_(0)
  , convergenceCriterion_(2)
  , simulationBudget_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultMaximumIterations"))
  , convergenceCriterionThreshold_(0.1)
  , maximumIterations_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultMaximumIterations"))
  , hasConverged_(false)
  , convergenceUncertaintyFactor_(ResourceMap::GetAsScalar("ActiveLearningReliabilityAlgorithm-DefaultConvergenceUncertaintyFactor"))
  , simulationAlgorithmSeed_(ResourceMap::GetAsUnsignedInteger("ActiveLearningReliabilityAlgorithm-DefaultSimulationAlgorithmSeed"))
{
  // Single extraction of the surrogate specification (one implicit fit at most)
  ExtractSurrogateSpec(fitter, covarianceModel_, basis_);
  if (inputDoE_.getSize() == 0) throw InvalidArgumentException(HERE) << "ActiveLearningReliabilityAlgorithm: input design of experiment is empty";
  if (inputDoE_.getSize() != outputDoE_.getSize()) throw InvalidArgumentException(HERE) << "ActiveLearningReliabilityAlgorithm: input and output designs have different sizes (" << inputDoE_.getSize() << " vs " << outputDoE_.getSize() << ")";
  if (outputDoE_.getDimension() != 1) throw InvalidArgumentException(HERE) << "ActiveLearningReliabilityAlgorithm: output design must be 1-dimensional";
  // The simulator handles share their implementation until first mutation:
  // setEvent/run/setKeepSample detach through copy-on-write, so the caller
  // object is never mutated. The criterion handle shares its
  // implementation until first mutation (copy-on-write on setGaussianProcessRegression).
  // The simulators keep their own sample policy: the scoring pool is drawn
  // separately, so keepSample is left untouched.
  simulation_ = defaultSimulation_;
  simulation_.setKeepSample(true);
  defaultSimulation_.setKeepSample(true);
   
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
  // Interface obligatoire pour Function
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
  Scalar kFactor_;
  GaussianProcessConditionalCovariance gprCov_;
  Function gprMetamodel_;
};

/* Saves the ambient random generator state, applies the run seed once,
   and restores the saved state on destruction, even on exception:
   single-threaded runs stay reproducible without permanently perturbing
   the process-wide stream shared with other threads */
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

/* Process-wide mutex serializing the reseeded inner runs of concurrent AK
   algorithms: RandomGenerator is a single global MersenneTwister with no
   internal lock, so two algorithms resetting it concurrently would interleave
   draws. Function-static for copy-safety (the algorithm is cloned). */
static std::mutex & GetActiveLearningSimulationMutex()
{
  static std::mutex mutex;
  return mutex;
}

/* Run an inner simulator with the exact same stream at each call, whatever its
   dynamic type and without testing for it: reset to the run seed under the
   process mutex, then run while holding it so no other AK run can interleave
   a reset or draws. Single-threaded runs are exactly reproducible;
   concurrent draws from other threads remain best-effort. */
static void RunInnerWithFixedStream(EventSimulation & simulation,
                                    const UnsignedInteger seed)
{
  std::lock_guard<std::mutex> lock(GetActiveLearningSimulationMutex());
  RandomGenerator::SetSeed(seed);
  simulation.run();
}

/* Check convergence based on probability uncertainty due to GP error */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceProbabilityWithUncertainty()
{
  const Point probabilitiesWithUncertainty = computeProbabilityWithUncertainty();
  const Scalar meanProbability = probabilitiesWithUncertainty[0];
  const Scalar minusProbability = probabilitiesWithUncertainty[1];
  const Scalar plusProbability =  probabilitiesWithUncertainty[2];
  const Bool convergenceProbability = abs((plusProbability - minusProbability) / meanProbability) <= convergenceCriterionThreshold_;

  return convergenceProbability;
}

/* Check convergence based on reliability index uncertainty due to GP error */
Bool ActiveLearningReliabilityAlgorithm::checkConvergenceReliabilityIndexWithUncertainty()
{
  const Point probabilitiesWithUncertainty = computeProbabilityWithUncertainty();
  const Scalar meanProbability = probabilitiesWithUncertainty[0];
  const Scalar meanReliabilityIndex = - DistFunc::qNormal(meanProbability);
  const Scalar minusProbability = probabilitiesWithUncertainty[1];
  const Scalar minusReliabilityIndex = - DistFunc::qNormal(minusProbability);
  const Scalar plusProbability =  probabilitiesWithUncertainty[2];
  const Scalar plusReliabilityIndex = - DistFunc::qNormal(plusProbability);
  const Bool convergenceReliabilityIndex = abs((plusReliabilityIndex - minusReliabilityIndex) / meanReliabilityIndex) <= convergenceCriterionThreshold_;

  return convergenceReliabilityIndex;
}

/* Compute three probability with GP uncertainty : minus, mean and plus k * std */
Point ActiveLearningReliabilityAlgorithm::computeProbabilityWithUncertainty()
{
  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();
  const GaussianProcessRegressionResult GPRResult = fitGaussianProcess();
  GaussianProcessConditionalCovariance gpcc(GPRResult);

  // Creation of functions for threshold event
  GPWithUncertainty GPMean(GPRResult, 0.);
  GPWithUncertainty GPMinus(GPRResult, - convergenceUncertaintyFactor_);
  GPWithUncertainty GPPlus(GPRResult, + convergenceUncertaintyFactor_);

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
  return abs(currentValue - previousValue)/ currentValue <= convergenceCriterionThreshold_;
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
  if (convergenceUncertaintyFactor < 0)
  {
    throw InvalidArgumentException(HERE) << "convergenceUncertaintyFactor (" << convergenceUncertaintyFactor << ") must be in greater than 0";
  }
  convergenceUncertaintyFactor_ = convergenceUncertaintyFactor;
}

void ActiveLearningReliabilityAlgorithm::setConvergenceCriterionThreshold(const Scalar convergenceCriterionThreshold)
{
  if (convergenceCriterionThreshold < 0)
  {
    throw InvalidArgumentException(HERE) << "convergenceCriterionThreshold (" << convergenceCriterionThreshold << ") must be in greater than 0";
  }
  convergenceCriterionThreshold_ = convergenceCriterionThreshold;
}

void ActiveLearningReliabilityAlgorithm::setSimulationBudget(const UnsignedInteger simulationBudget)
{
  // Deprecated alias of setMaximumIterations, kept for compatibility
  setMaximumIterations(simulationBudget);
}

void ActiveLearningReliabilityAlgorithm::setMaximumIterations(const UnsignedInteger maximumIterations)
{
  if (maximumIterations < 1)
  {
    throw InvalidArgumentException(HERE) << "maximumIterations (" << maximumIterations << ") must be greater than 1";
  }
  maximumIterations_ = maximumIterations;
  simulationBudget_ = maximumIterations;
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


// Accessor to simulationAlgorithmSeed

void ActiveLearningReliabilityAlgorithm::setSimulationAlgorithmSeed(const UnsignedInteger seed)
{
  simulationAlgorithmSeed_ = seed;
}

UnsignedInteger ActiveLearningReliabilityAlgorithm::getSimulationAlgorithmSeed() const
{
  return simulationAlgorithmSeed_;
}


/** Maximum sample size accessor */
void ActiveLearningReliabilityAlgorithm::setMaximumOuterSampling(const UnsignedInteger )
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::setMaximumOuterSampling(const UnsignedInteger maximumOuterSampling)";
}


UnsignedInteger ActiveLearningReliabilityAlgorithm::getMaximumOuterSampling() const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::getMaximumOuterSampling()";
}

/** Maximum coefficient of variation accessor */
void ActiveLearningReliabilityAlgorithm::setMaximumCoefficientOfVariation(const Scalar )
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::setMaximumCoefficientOfVariation(const Scalar maximumCoefficientOfVariation)";
}

Scalar ActiveLearningReliabilityAlgorithm::getMaximumCoefficientOfVariation() const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::getMaximumCoefficientOfVariation()";
}

/** Convergence strategy accessor */
void ActiveLearningReliabilityAlgorithm::setConvergenceStrategy(const HistoryStrategy & )
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::setConvergenceStrategy(const HistoryStrategy & convergenceStrategy)";
}

HistoryStrategy ActiveLearningReliabilityAlgorithm::getConvergenceStrategy() const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::getConvergenceStrategy()";
}

/** Maximum standard deviation accessor */
void ActiveLearningReliabilityAlgorithm::setMaximumStandardDeviation(const Scalar )
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::setMaximumStandardDeviation(const Scalar maximumStandardDeviation)";
}
Scalar ActiveLearningReliabilityAlgorithm::getMaximumStandardDeviation() const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::getMaximumStandardDeviation()";
}

/** Block size accessor */
void ActiveLearningReliabilityAlgorithm::setBlockSize(const UnsignedInteger )
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::setBlockSize(const UnsignedInteger blockSize)";
}
UnsignedInteger ActiveLearningReliabilityAlgorithm::getBlockSize() const
{
  throw NotYetImplementedException(HERE) << "In ActiveLearningReliabilityAlgorithm::getBlockSize()";
}



/* Run of the algorithm: fit GPR,  enrich until the learning criterion converges
   or the iteration budget is exhausted. Budget exhaustion exits with
   hasConverged()=false. The candidate pool is drawn once per run; each inner run resets to the run seed under a
   process-wide mutex so every iteration consumes the exact same stream,
   and the ambient generator state is restored on exit. */
void ActiveLearningReliabilityAlgorithm::run()
{
  const Distribution inputDistribution = defaultEvent_.getImplementation()->getAntecedent().getDistribution();
  const Function model(defaultEvent_.getFunction());
  // Single seeding point of the run, restored on exit even on exception
  const RandomGeneratorSeedGuard seedGuard(simulationAlgorithmSeed_);

  Bool convergenceStatus = false;
  hasConverged_ = false;
  ActiveLearningReliabilityResult::SimulationResultsPersistentCollection simulationResults;

  while ((!convergenceStatus) && (functionCallNumber_ < maximumIterations_))
    {

      // Estimate probability with GP

      const GaussianProcessRegressionResult newGPRResult = fitGaussianProcess();

      //Estimate probability with current GP
      Function newGPRmetamodel =  newGPRResult.getMetaModel();
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
      
      // Compute active learning values on the fixed candidate pool
      criterion_.setGaussianProcessRegression(newGPRResult);
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

      // Check convergence
      if (convergenceCriterion_ == 0)
      {
        // check convergence of uncertainty of probability estimate
        convergenceStatus = checkConvergenceProbabilityWithUncertainty();
      }
      else if (convergenceCriterion_ == 1)
      {
        // check convergence of uncertainty of reliability index
        convergenceStatus = checkConvergenceReliabilityIndexWithUncertainty();
      }
      else if (convergenceCriterion_ == 2)
      {
        // check convergence of active learning
        convergenceStatus = criterion_.checkConvergenceLearning(activeLearningValues);
      }
      else if (convergenceCriterion_ == 3)
      {
        UnsignedInteger historySize = probabilityHistory_.getSize();
        if (historySize > 1)
        {
          Indices index(2);
          index[0] = historySize - 1;
          index[1] = historySize - 2;
          ScalarCollection probabilityEstimate = probabilityHistory_.select(index);

          convergenceStatus = checkConvergenceStability(probabilityEstimate[0], probabilityEstimate[1]);
        }
      }
      else if (convergenceCriterion_ == 4)
      {
        UnsignedInteger historySize = probabilityHistory_.getSize();
        if (historySize > 1)
        {
          Indices index(2);
          index[0] = historySize - 1;
          index[1] = historySize - 2;

          ScalarCollection reliabilityIndexEstimate = reliabilityIndexHistory_.select(index);

          convergenceStatus = checkConvergenceStability(reliabilityIndexEstimate[0], reliabilityIndexEstimate[1]);
        }
      }

      // Add infill sample if convergence is not reached
      if (!convergenceStatus)
      {
        // Get input sample to evaluate from the fixed candidate pool
        Sample infillInputSample = criterion_.getInfillSample(candidatePool, activeLearningValues);

        inputDoE_.add(infillInputSample);
        Sample infillOutputSample = model(infillInputSample);
        outputDoE_.add(infillOutputSample);
        functionCallNumber_ += 1;
      }
    }

    hasConverged_ = convergenceStatus;
    // storage of results
    const GaussianProcessRegressionResult newGPRResult = fitGaussianProcess();

    UnsignedInteger historySize = probabilityHistory_.getSize();
    Indices index(2);
    index[0] = historySize - 1;
    Scalar currentProbabilityEstimate = probabilityHistory_.select(index)[0];
    Scalar currentReliabilityIndex = reliabilityIndexHistory_.select(index)[0];
    const Scalar currentVarianceEstimate = simulation_.getResult().getVarianceEstimate();
    const UnsignedInteger currentOuterSampling = simulation_.getResult().getOuterSampling();
    const UnsignedInteger currentBlockSize = simulation_.getResult().getBlockSize();

    Point probaCI = computeProbabilityWithUncertainty();
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
                                            newGPRResult,
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
  simulationBudget_ = maximumIterations_;
  if (adv.hasAttribute("hasConverged_"))
    adv.loadAttribute("hasConverged_", hasConverged_);
  adv.loadAttribute("convergenceCriterionThreshold_", convergenceCriterionThreshold_);
  adv.loadAttribute("convergenceUncertaintyFactor_", convergenceUncertaintyFactor_);
  adv.loadAttribute("simulationAlgorithmSeed_", simulationAlgorithmSeed_);
  adv.loadAttribute("activeLearningReliabilityResult_", activeLearningReliabilityResult_);
}

END_NAMESPACE_OPENTURNS
