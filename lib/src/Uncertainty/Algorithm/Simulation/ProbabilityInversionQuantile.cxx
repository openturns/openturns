//                                               -*- C++ -*-
/**
 *  @brief Generic probability-inversion quantile estimator
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
#include <cmath>
#include <algorithm>
#include <limits>

#include "openturns/ProbabilityInversionQuantile.hxx"
#include "openturns/QuantileRootSolver.hxx"
#include "openturns/QuantileSimulationAlgorithm.hxx"
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
#include "openturns/ProbabilitySimulationResult.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/FixedExperiment.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/DistFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ProbabilityInversionQuantile)

static const Factory<ProbabilityInversionQuantile> Factory_ProbabilityInversionQuantile;

namespace
{
/** Empirical inverse at level alpha, using the same convention as the
    fixed-sample quantile estimators. */
Scalar inversionPilotQuantile(const Sample & sample, const Scalar alpha)
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The pilot sample is empty";
  const Sample sortedSample(sample.sort());
  UnsignedInteger index = static_cast<UnsignedInteger>(std::ceil(alpha * size));
  if (index > 0)
    --index;
  if (index >= size)
    index = size - 1;
  return sortedSample(index, 0);
}

/** Robust scale of the pilot sample used as the first bracket step. */
Scalar inversionPilotScale(const Sample & sample)
{
  const Scalar standardDeviation = sample.computeStandardDeviation()[0];
  const Scalar interquartileRange = sample.computeQuantile(0.75)[0] - sample.computeQuantile(0.25)[0];
  Scalar scale = std::max(standardDeviation, interquartileRange);
  if (!std::isfinite(scale) || !(scale > 0.0))
    scale = 1.0;
  return scale;
}

/** Student t-interval from independent replicate estimates. */
Interval inversionReplicateInterval(const Point & estimates,
                                    const Scalar beta,
                                    Scalar & estimate,
                                    Scalar & standardError)
{
  const UnsignedInteger size = estimates.getSize();
  if (size < 2)
    throw InvalidArgumentException(HERE)
        << "At least two replicates are required for a replicate interval, but replicateCount = " << size;
  Scalar mean = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
    mean += estimates[i];
  mean /= size;
  Scalar variance = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar delta = estimates[i] - mean;
    variance += delta * delta;
  }
  variance /= (size - 1);
  standardError = std::sqrt(std::max(0.0, variance) / size);
  const Scalar t = DistFunc::qStudent(1.0 * (size - 1), 0.5 * (1.0 + beta));
  estimate = mean;
  const Scalar halfWidth = t * standardError;
  return Interval(Point(1, mean - halfWidth), Point(1, mean + halfWidth));
}

} // anonymous namespace

/* Default constructor */
ProbabilityInversionQuantile::ProbabilityInversionQuantile()
  : QuantileSimulationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
ProbabilityInversionQuantile::ProbabilityInversionQuantile(const EventSimulation & algorithm,
                                                           const Scalar alpha,
                                                           const Scalar beta)
  : QuantileSimulationImplementation()
{
  setAlpha(alpha);
  setConfidenceLevel(beta);
  setAlgorithm(algorithm);
}

/* Virtual constructor */
ProbabilityInversionQuantile * ProbabilityInversionQuantile::clone() const
{
  return new ProbabilityInversionQuantile(*this);
}

/* Wrapped algorithm accessor */
void ProbabilityInversionQuantile::setAlgorithm(const EventSimulation & algorithm)
{
  RandomVector event;
  try
  {
    event = algorithm.getEvent();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE)
        << "The wrapped algorithm has no event";
  }
  if (!event.isEvent())
    throw InvalidArgumentException(HERE)
        << "The wrapped algorithm event is not an event";
  if (!event.isComposite())
    throw InvalidArgumentException(HERE)
        << "The wrapped algorithm event must be a composed event on a scalar composite random vector";

  ComparisonOperator op;
  try
  {
    op = event.getOperator();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE)
        << "The wrapped algorithm event must be a threshold event with a comparison operator";
  }
  const String operatorName(op.getImplementation()->getClassName());
  if ((operatorName == "Greater") || (operatorName == "GreaterOrEqual"))
    upperTail_ = true;
  else if ((operatorName == "Less") || (operatorName == "LessOrEqual"))
    upperTail_ = false;
  else
    throw InvalidArgumentException(HERE)
        << "Unsupported event comparison operator " << operatorName
        << ", expected Greater, GreaterOrEqual, Less or LessOrEqual";

  // The event stores the input vector as its antecedent and the limit
  // state as its function.  Rebuild the scalar output random vector
  // g(X) from these two components.
  const RandomVector output(new CompositeRandomVector(event.getFunction(), event.getAntecedent()));
  if (output.getDimension() != 1)
    throw InvalidArgumentException(HERE)
        << "The wrapped algorithm event must be defined on a scalar random vector, but its output dimension is "
        << output.getDimension();

  algorithm_ = algorithm;
  hasAlgorithm_ = true;
  thresholdOperator_ = op;
  output_ = output;

  // The base class requires a positive sample size.  For an inversion
  // the sample size is only a reporting quantity: the wrapped engine
  // decides how many points it draws for each probability evaluation.
  if (sampleSize_ == 0)
  {
    const UnsignedInteger blockSize = std::max<UnsignedInteger>(1, algorithm_.getBlockSize());
    const UnsignedInteger outerSampling = std::max<UnsignedInteger>(1, algorithm_.getMaximumOuterSampling());
    const UnsignedInteger maximumValue = std::numeric_limits<UnsignedInteger>::max() / blockSize;
    const UnsignedInteger nominalSize = (outerSampling > maximumValue) ? maximumValue : outerSampling * blockSize;
    setSampleSize(std::max<UnsignedInteger>(2, nominalSize));
  }
}

EventSimulation ProbabilityInversionQuantile::getAlgorithm() const
{
  return algorithm_;
}

Bool ProbabilityInversionQuantile::hasAlgorithm() const
{
  return hasAlgorithm_;
}

/* Common-random-number request accessor */
void ProbabilityInversionQuantile::setUseCommonRandomNumbers(const Bool useCommonRandomNumbers)
{
  useCommonRandomNumbers_ = useCommonRandomNumbers;
}

Bool ProbabilityInversionQuantile::getUseCommonRandomNumbers() const
{
  return useCommonRandomNumbers_;
}

/* Root-finding tolerance accessor */
void ProbabilityInversionQuantile::setRootTolerance(const Scalar tolerance)
{
  if (!std::isfinite(tolerance) || !(tolerance > 0.0))
    throw InvalidArgumentException(HERE)
        << "The root-finding tolerance must be finite and positive, but tolerance = "
        << tolerance;
  rootTolerance_ = tolerance;
}

Scalar ProbabilityInversionQuantile::getRootTolerance() const
{
  return rootTolerance_;
}

/* Maximum root-finding iteration accessor */
void ProbabilityInversionQuantile::setMaximumRootIteration(const UnsignedInteger maximumIteration)
{
  if (maximumIteration == 0)
    throw InvalidArgumentException(HERE)
        << "The maximum root-finding iteration count must be positive";
  maximumRootIteration_ = maximumIteration;
}

UnsignedInteger ProbabilityInversionQuantile::getMaximumRootIteration() const
{
  return maximumRootIteration_;
}

/* Pilot sample size accessor */
void ProbabilityInversionQuantile::setPilotSampleSize(const UnsignedInteger pilotSampleSize)
{
  if (pilotSampleSize == 0)
    throw InvalidArgumentException(HERE)
        << "The pilot sample size must be positive";
  pilotSampleSize_ = pilotSampleSize;
}

UnsignedInteger ProbabilityInversionQuantile::getPilotSampleSize() const
{
  return pilotSampleSize_;
}

/* Replicate count accessor */
void ProbabilityInversionQuantile::setReplicateCount(const UnsignedInteger replicateCount)
{
  if (replicateCount == 0)
    throw InvalidArgumentException(HERE)
        << "The replicate count must be positive";
  replicateCount_ = replicateCount;
}

UnsignedInteger ProbabilityInversionQuantile::getReplicateCount() const
{
  return replicateCount_;
}

/* Whether the wrapped engine is a plain Monte Carlo simulation */
Bool ProbabilityInversionQuantile::isPlainMonteCarlo() const
{
  if (!hasAlgorithm_)
    return false;
  const ProbabilitySimulationAlgorithm * p_algorithm =
      dynamic_cast<const ProbabilitySimulationAlgorithm *>(algorithm_.getImplementation().get());
  if (!p_algorithm)
    return false;
  const WeightedExperiment experiment(p_algorithm->getExperiment());
  const MonteCarloExperiment * p_experiment =
      dynamic_cast<const MonteCarloExperiment *>(experiment.getImplementation().get());
  return p_experiment && experiment.isIID() && experiment.hasUniformWeights();
}

/* Whether the wrapped engine evaluates a fixed sample */
Bool ProbabilityInversionQuantile::hasFixedExperiment() const
{
  if (!hasAlgorithm_)
    return false;
  const ProbabilitySimulationAlgorithm * p_algorithm =
      dynamic_cast<const ProbabilitySimulationAlgorithm *>(algorithm_.getImplementation().get());
  if (!p_algorithm)
    return false;
  const WeightedExperiment experiment(p_algorithm->getExperiment());
  return dynamic_cast<const FixedExperiment *>(experiment.getImplementation().get()) != nullptr;
}

/* Nominal sample size of the wrapped engine */
UnsignedInteger ProbabilityInversionQuantile::getNominalSampleSize() const
{
  return std::max<UnsignedInteger>(2, sampleSize_);
}

/* Evaluate the wrapped engine at the given threshold */
ProbabilityInversionQuantile::Evaluation
ProbabilityInversionQuantile::evaluate(const Scalar threshold) const
{
  Evaluation evaluation;
  // Clone the implementation: the user-provided engine is never modified.
  EventSimulation engine(*(algorithm_.getImplementation()));
  engine.setEvent(ThresholdEvent(output_, thresholdOperator_, threshold));
  engine.run();
  const ProbabilitySimulationResult engineResult(engine.getResult());
  evaluation.eventProbability = engineResult.getProbabilityEstimate();
  if (!std::isfinite(evaluation.eventProbability))
    throw NotDefinedException(HERE)
        << "The wrapped algorithm returned a non-finite probability estimate at threshold "
        << threshold;
  evaluation.tailProbability = upperTail_
                               ? evaluation.eventProbability
                               : 1.0 - evaluation.eventProbability;
  // A negative standard deviation is the convention for "not available".
  const Scalar standardDeviation = engineResult.getStandardDeviation();
  if (std::isfinite(standardDeviation) && (standardDeviation >= 0.0))
  {
    evaluation.standardDeviation = standardDeviation;
    evaluation.hasStandardDeviation = true;
  }
  return evaluation;
}

/* Invert the probability: sign bisection for a deterministic fixed
   experiment, confidence-bound bisection otherwise */
ProbabilityInversionQuantile::Inversion
ProbabilityInversionQuantile::invert(const Scalar start,
                                     const Scalar step,
                                     const Bool useConfidenceBounds) const
{
  Inversion inversion;
  inversion.history = Sample(0, 2);
  const Scalar target = 1.0 - alpha_;
  Scalar lastValue = 0.0;
  Bool lastHasStandardDeviation = false;
  Scalar lastStandardDeviation = 0.0;
  const QuantileRootSolver::FunctionType objective =
      [this, target, &inversion, &lastValue, &lastHasStandardDeviation, &lastStandardDeviation](const Scalar threshold) -> Scalar
  {
    const Evaluation evaluation = evaluate(threshold);
    ++inversion.probabilityEvaluationCount;
    inversion.history.add(Point({1.0 * inversion.probabilityEvaluationCount, threshold}));
    lastValue = evaluation.tailProbability - target;
    lastHasStandardDeviation = evaluation.hasStandardDeviation;
    lastStandardDeviation = evaluation.standardDeviation;
    return lastValue;
  };

  // The wrapped engine may be stochastic and may not expose a stream
  // that can be reset.  The bracket is still built from point
  // estimates; the iteration below only discards an endpoint when the
  // whole confidence bound is on one side of the target.
  const QuantileRootSolver solver(rootTolerance_, maximumRootIteration_);
  Scalar lowerBound = 0.0;
  Scalar upperBound = 0.0;
  Scalar lowerValue = 0.0;
  Scalar upperValue = 0.0;
  solver.bracket(objective, start, lowerBound, upperBound, lowerValue, upperValue,
                 step, ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-DefaultBracketExpansion"));

  const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta_));
  inversion.converged = false;
  while (inversion.iterationCount < maximumRootIteration_)
  {
    const Scalar scale = std::max(1.0, std::max(std::abs(lowerBound), std::abs(upperBound)));
    if ((upperBound - lowerBound) <= rootTolerance_ * scale)
    {
      inversion.converged = true;
      break;
    }
    const Scalar midpoint = 0.5 * (lowerBound + upperBound);
    if ((midpoint == lowerBound) || (midpoint == upperBound))
    {
      inversion.converged = true;
      break;
    }
    const Scalar midpointValue = objective(midpoint);
    ++inversion.iterationCount;
    if (!std::isfinite(midpointValue))
      throw NotDefinedException(HERE)
          << "The wrapped algorithm returned a non-finite objective at threshold "
          << midpoint;
    if (!useConfidenceBounds)
    {
      // Deterministic fixed experiment: the survival function is a
      // step function of the threshold, so the sign of the objective
      // defines the bracket exactly.
      if (midpointValue >= 0.0)
      {
        lowerBound = midpoint;
        lowerValue = midpointValue;
      }
      else
      {
        upperBound = midpoint;
        upperValue = midpointValue;
      }
    }
    else if (lastHasStandardDeviation)
    {
      const Scalar lowerConfidence = midpointValue - z * lastStandardDeviation;
      const Scalar upperConfidence = midpointValue + z * lastStandardDeviation;
      if (lowerConfidence > 0.0)
      {
        lowerBound = midpoint;
        lowerValue = midpointValue;
      }
      else if (upperConfidence < 0.0)
      {
        upperBound = midpoint;
        upperValue = midpointValue;
      }
      else
      {
        // The confidence bound straddles the target: the sign of the
        // objective is undecided and the bracket cannot be narrowed.
        inversion.converged = false;
        break;
      }
    }
    else if (midpointValue >= 0.0)
    {
      lowerBound = midpoint;
      lowerValue = midpointValue;
    }
    else
    {
      upperBound = midpoint;
      upperValue = midpointValue;
    }
  }

  inversion.estimate = 0.5 * (lowerBound + upperBound);
  inversion.bracket = Interval(lowerBound, upperBound);
  return inversion;
}

/* Delegate a plain Monte Carlo engine to direct extraction */
QuantileSimulationResult ProbabilityInversionQuantile::runDelegated()
{
  const UnsignedInteger nominalSize = getNominalSampleSize();
  const Distribution inputDistribution(output_.getAntecedent().getDistribution());
  QuantileSimulationAlgorithm algorithm(output_,
                                        MonteCarloExperiment(inputDistribution, nominalSize),
                                        alpha_, beta_);
  algorithm.setRequiredCoverage(requiredCoverage_);
  algorithm.run();
  QuantileSimulationResult result(algorithm.getResult());
  result.setDelegatedToDirectExtraction(true);
  result.setWrappedAlgorithmDescription(algorithm_.__repr__());
  result.setCommonRandomNumbers(false);
  result.setRandomStreamCapability(false);
  result.setRootSolverType(QuantileSimulationResultImplementation::BISECTION);
  return result;
}

/* Check the input parameters */
void ProbabilityInversionQuantile::checkInputs() const
{
  if (!hasAlgorithm_)
    throw InvalidArgumentException(HERE)
        << "The wrapped algorithm must be set before running the inversion";
  QuantileSimulationImplementation::checkInputs();
  if (replicateCount_ == 0)
    throw InvalidArgumentException(HERE)
        << "The replicate count must be positive";
}

/* Run the probability-inversion experiment */
QuantileSimulationResult ProbabilityInversionQuantile::runFixedSample()
{
  if (isPlainMonteCarlo())
    return runDelegated();

  // Pilot sample used only to start and scale the bracket.
  const Sample pilotSample(output_.getSample(pilotSampleSize_));
  const Scalar start = inversionPilotQuantile(pilotSample, alpha_);
  const Scalar step = inversionPilotScale(pilotSample);

  Point replicateEstimates(replicateCount_);
  Sample history(0, 2);
  UnsignedInteger totalEvaluationCount = 0;
  UnsignedInteger totalIterationCount = 0;
  Scalar unionLower = std::numeric_limits<Scalar>::max();
  Scalar unionUpper = -std::numeric_limits<Scalar>::max();
  Bool allConverged = true;
  const Bool useConfidenceBounds = !hasFixedExperiment();
  for (UnsignedInteger replicate = 0; replicate < replicateCount_; ++replicate)
  {
    const Inversion inversion = invert(start, step, useConfidenceBounds);
    replicateEstimates[replicate] = inversion.estimate;
    totalEvaluationCount += inversion.probabilityEvaluationCount;
    totalIterationCount += inversion.iterationCount;
    unionLower = std::min(unionLower, inversion.bracket.getLowerBound()[0]);
    unionUpper = std::max(unionUpper, inversion.bracket.getUpperBound()[0]);
    if (!inversion.converged)
      allConverged = false;
    if (replicate == 0)
      history = inversion.history;
    else
    {
      // Keep a single evaluation index across replicates.
      for (UnsignedInteger i = 0; i < inversion.history.getSize(); ++i)
        history.add(Point({1.0 * (totalEvaluationCount - inversion.probabilityEvaluationCount + i + 1),
                           inversion.history(i, 1)}));
    }
  }

  const Interval bracket(Point(1, unionLower), Point(1, unionUpper));
  Scalar estimate = replicateEstimates[0];
  Scalar standardError = 0.0;
  Interval interval(Point(1, estimate), Point(1, estimate));
  if (replicateCount_ >= 2)
    interval = inversionReplicateInterval(replicateEstimates, beta_, estimate, standardError);
  if (!allConverged)
  {
    // The confidence-bound bisection can stall when independent runs
    // of the wrapped engine are used for successive thresholds.  The
    // point estimate is still reported, but without any coverage
    // guarantee.
    estimate = 0.5 * (bracket.getLowerBound()[0] + bracket.getUpperBound()[0]);
    standardError = 0.0;
    interval = Interval(Point(1, estimate), Point(1, estimate));
  }

  // The generic adapter has no stream-injection capability in this
  // branch.  It therefore never claims more than COVERAGE_NONE, even
  // when independent replicates provide a Student interval.
  const QuantileSimulationResultImplementation::CoverageType coverage =
      QuantileSimulationResultImplementation::COVERAGE_NONE;

  const UnsignedInteger engineSampleSize = getNominalSampleSize();
  const Scalar effectiveSampleSize = std::max(1.0, 1.0 * engineSampleSize * std::max<UnsignedInteger>(1, totalEvaluationCount));

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(QuantileSimulationResultImplementation::EMPIRICAL_INVERSE);
  result.setCoverageType(coverage);
  result.setRequiredCoverage(requiredCoverage_);
  result.setEffectiveSampleSize(effectiveSampleSize);
  result.setSampleSize(engineSampleSize);
  result.setBlockLength(0);
  result.setIsFixedSample(true);
  result.setOuterSampling(std::max<UnsignedInteger>(1, totalEvaluationCount));
  result.setBlockSize(std::max<UnsignedInteger>(1, algorithm_.getBlockSize()));
  result.setConvergenceHistory(history);
  result.setDirectionCount(0);
  result.setRootBracket(bracket);
  result.setRootIterationCount(totalIterationCount);
  result.setProbabilityEvaluationCount(totalEvaluationCount);
  result.setRootSolverType(QuantileSimulationResultImplementation::BISECTION);
  result.setCommonRandomNumbers(false);
  result.setRandomStreamCapability(false);
  result.setDelegatedToDirectExtraction(false);
  result.setWrappedAlgorithmDescription(algorithm_.__repr__());
  result.setReplicateCount(replicateCount_);
  result.setReplicateEstimates(replicateEstimates);
  return result;
}

/* String converter */
String ProbabilityInversionQuantile::__repr__() const
{
  OSS oss;
  oss << "class=" << ProbabilityInversionQuantile::GetClassName()
      << " algorithm=" << algorithm_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " useCommonRandomNumbers=" << useCommonRandomNumbers_
      << " rootTolerance=" << rootTolerance_
      << " maximumRootIteration=" << maximumRootIteration_
      << " pilotSampleSize=" << pilotSampleSize_
      << " replicateCount=" << replicateCount_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void ProbabilityInversionQuantile::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("algorithm_", algorithm_);
  adv.saveAttribute("hasAlgorithm_", hasAlgorithm_);
  adv.saveAttribute("thresholdOperator_", thresholdOperator_);
  adv.saveAttribute("upperTail_", upperTail_);
  adv.saveAttribute("useCommonRandomNumbers_", useCommonRandomNumbers_);
  adv.saveAttribute("rootTolerance_", rootTolerance_);
  adv.saveAttribute("maximumRootIteration_", maximumRootIteration_);
  adv.saveAttribute("pilotSampleSize_", pilotSampleSize_);
  adv.saveAttribute("replicateCount_", replicateCount_);
}

/* Method load() reloads the object from the StorageManager */
void ProbabilityInversionQuantile::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("algorithm_", algorithm_);
  adv.loadAttribute("hasAlgorithm_", hasAlgorithm_);
  adv.loadAttribute("thresholdOperator_", thresholdOperator_);
  adv.loadAttribute("upperTail_", upperTail_);
  adv.loadAttribute("useCommonRandomNumbers_", useCommonRandomNumbers_);
  adv.loadAttribute("rootTolerance_", rootTolerance_);
  adv.loadAttribute("maximumRootIteration_", maximumRootIteration_);
  adv.loadAttribute("pilotSampleSize_", pilotSampleSize_);
  adv.loadAttribute("replicateCount_", replicateCount_);
}

END_NAMESPACE_OPENTURNS
