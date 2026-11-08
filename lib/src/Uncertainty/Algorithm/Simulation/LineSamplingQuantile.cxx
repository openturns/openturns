//                                               -*- C++ -*-
/**
 *  @brief Line-sampling quantile estimator
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

#include "openturns/LineSamplingQuantile.hxx"
#include "openturns/QuantileRootSolver.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/StandardEvent.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/IdentityFunction.hxx"
#include "openturns/LinearFunction.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/Greater.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/AbdoRackwitz.hxx"
#include "openturns/FORM.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(LineSamplingQuantile)

static const Factory<LineSamplingQuantile> Factory_LineSamplingQuantile;

namespace
{
/** Empirical inverse at level alpha, using the same convention as the
    fixed-sample quantile estimators. */
Scalar linePilotQuantile(const Sample & sample, const Scalar alpha)
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
Scalar linePilotScale(const Sample & sample)
{
  const Scalar standardDeviation = sample.computeStandardDeviation()[0];
  const Scalar interquartileRange = sample.computeQuantile(0.75)[0] - sample.computeQuantile(0.25)[0];
  Scalar scale = std::max(standardDeviation, interquartileRange);
  if (!std::isfinite(scale) || !(scale > 0.0))
    scale = 1.0;
  return scale;
}

} // anonymous namespace

/* Default constructor */
LineSamplingQuantile::LineSamplingQuantile()
  : QuantileSimulationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
LineSamplingQuantile::LineSamplingQuantile(const RandomVector & output,
                                           const Scalar alpha,
                                           const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  // Nothing to do
}

/* Virtual constructor */
LineSamplingQuantile * LineSamplingQuantile::clone() const
{
  return new LineSamplingQuantile(*this);
}

/* Important direction accessor */
void LineSamplingQuantile::setInitialAlpha(const Point & initialAlpha)
{
  const Scalar norm = initialAlpha.norm();
  if (!std::isfinite(norm) || !(norm > 0.0))
    throw InvalidArgumentException(HERE)
        << "The important direction must be finite and non-zero";
  initialAlpha_ = initialAlpha;
  hasInitialAlpha_ = true;
}

Point LineSamplingQuantile::getInitialAlpha() const
{
  return initialAlpha_;
}

/* Root strategy accessor */
void LineSamplingQuantile::setRootStrategy(const RootStrategy & rootStrategy)
{
  rootStrategy_ = rootStrategy;
}

RootStrategy LineSamplingQuantile::getRootStrategy() const
{
  return rootStrategy_;
}

/* Opposite-direction search accessor */
void LineSamplingQuantile::setSearchOppositeDirection(const Bool searchOppositeDirection)
{
  searchOppositeDirection_ = searchOppositeDirection;
}

Bool LineSamplingQuantile::getSearchOppositeDirection() const
{
  return searchOppositeDirection_;
}

/* Root-finding tolerance accessor */
void LineSamplingQuantile::setRootTolerance(const Scalar tolerance)
{
  if (!std::isfinite(tolerance) || !(tolerance > 0.0))
    throw InvalidArgumentException(HERE)
        << "The root-finding tolerance must be finite and positive, but tolerance = "
        << tolerance;
  rootTolerance_ = tolerance;
}

Scalar LineSamplingQuantile::getRootTolerance() const
{
  return rootTolerance_;
}

/* Maximum root-finding iteration accessor */
void LineSamplingQuantile::setMaximumRootIteration(const UnsignedInteger maximumIteration)
{
  if (maximumIteration == 0)
    throw InvalidArgumentException(HERE)
        << "The maximum root-finding iteration count must be positive";
  maximumRootIteration_ = maximumIteration;
}

UnsignedInteger LineSamplingQuantile::getMaximumRootIteration() const
{
  return maximumRootIteration_;
}

/* Pilot sample size accessor */
void LineSamplingQuantile::setPilotSampleSize(const UnsignedInteger pilotSampleSize)
{
  if (pilotSampleSize == 0)
    throw InvalidArgumentException(HERE)
        << "The pilot sample size must be positive";
  pilotSampleSize_ = pilotSampleSize;
}

UnsignedInteger LineSamplingQuantile::getPilotSampleSize() const
{
  return pilotSampleSize_;
}

/* Probability estimate and conditional contributions at a threshold.
   The lines are fixed, so this is a deterministic function of the
   threshold for a given run. */
Scalar LineSamplingQuantile::computeProbability(const Scalar threshold,
                                                Point & contributions)
{
  const UnsignedInteger size = orthogonalPoints_.getSize();
  const UnsignedInteger dimension = standardDistribution_.getDimension();
  contributions = Point(size);
  Matrix linearForward(dimension, 1);
  Matrix linearBackward(dimension, 1);
  for (UnsignedInteger j = 0; j < dimension; ++j)
  {
    linearForward(j, 0) = importantDirection_[j];
    linearBackward(j, 0) = -importantDirection_[j];
  }
  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point origin(orthogonalPoints_[i]);
    const Bool originInEvent = originValues_[i] > threshold;
    rootStrategy_.setOriginValue(originValues_[i]);

    const LinearFunction forwardRay(Point(1, 0.0), origin, linearForward);
    const ComposedFunction forwardFunction(standardFunction_, forwardRay);
    const RootStrategy::ScalarCollection forwardRoots(rootStrategy_.solve(forwardFunction, threshold));
    Scalar forwardEstimate = 0.0;
    Scalar sign = 1.0;
    for (UnsignedInteger k = 0; k < forwardRoots.getSize(); ++k)
    {
      forwardEstimate += sign * DistFunc::pNormal(-forwardRoots[k]);
      sign = -sign;
    }

    Scalar contribution = 0.0;
    if (searchOppositeDirection_)
    {
      const LinearFunction backwardRay(Point(1, 0.0), origin, linearBackward);
      const ComposedFunction backwardFunction(standardFunction_, backwardRay);
      const RootStrategy::ScalarCollection backwardRoots(rootStrategy_.solve(backwardFunction, threshold));
      Scalar backwardEstimate = 0.0;
      sign = 1.0;
      for (UnsignedInteger k = 0; k < backwardRoots.getSize(); ++k)
      {
        backwardEstimate += sign * DistFunc::pNormal(-backwardRoots[k]);
        sign = -sign;
      }
      contribution = originInEvent ? 1.0 - (forwardEstimate + backwardEstimate)
                                   : forwardEstimate + backwardEstimate;
    }
    else
    {
      contribution = originInEvent ? 1.0 - forwardEstimate : forwardEstimate;
    }
    contribution = std::min(1.0, std::max(0.0, contribution));
    contributions[i] = contribution;
    sum += contribution;
  }
  return sum / size;
}

/* Run the line-sampling root-finding experiment */
QuantileSimulationResult LineSamplingQuantile::runFixedSample()
{
  if (sampleSize_ < 2)
    throw InvalidArgumentException(HERE)
        << "At least two lines are required for a delta-method interval, but sampleSize = "
        << sampleSize_;

  // Threshold events require a composite random vector.
  RandomVector compositeOutput(output_);
  if (!compositeOutput.isComposite())
    compositeOutput = CompositeRandomVector(IdentityFunction(1), output_);

  // Build the standard-space limit state once.  The threshold value
  // used here is irrelevant: only the composed function and the
  // standard distribution are retained.
  const ThresholdEvent referenceEvent(compositeOutput, Greater(), 0.0);
  const StandardEvent standardEvent(referenceEvent);
  standardFunction_ = standardEvent.getImplementation()->getFunction();
  standardDistribution_ = standardEvent.getImplementation()->getAntecedent().getDistribution();
  const UnsignedInteger dimension = standardDistribution_.getDimension();
  const Distribution inputDistribution(compositeOutput.getAntecedent().getDistribution());

  // Pilot sample used for the initial bracket and, when the user did
  // not provide a direction, for the important direction.
  const Sample pilotInputSample(inputDistribution.getSample(pilotSampleSize_));
  const Sample pilotOutputSample(compositeOutput.getFunction()(pilotInputSample));
  const Scalar start = linePilotQuantile(pilotOutputSample, alpha_);
  const Scalar step = linePilotScale(pilotOutputSample);

  // Fix the important direction once.  It is never updated across
  // thresholds, otherwise the common-random-number structure would be
  // broken.
  Point direction;
  if (hasInitialAlpha_)
  {
    direction = initialAlpha_;
  }
  else
  {
    try
    {
      AbdoRackwitz nearestPointAlgorithm;
      nearestPointAlgorithm.setStartingPoint(inputDistribution.getMean());
      const ThresholdEvent pilotEvent(compositeOutput, Greater(), start);
      FORM form(nearestPointAlgorithm, pilotEvent);
      form.run();
      const Point designPoint(form.getResult().getStandardSpaceDesignPoint());
      if (designPoint.norm() > 0.0)
        direction = designPoint;
    }
    catch (const Exception &)
    {
      LOGINFO("LineSamplingQuantile: FORM failed at the pilot threshold, using an extreme pilot point instead");
    }
    if (!(direction.getSize() > 0))
    {
      // Fallback: the pilot standard point with the largest norm
      // among the tail observations, otherwise the largest norm.
      const Sample pilotStandardSample(inputDistribution.getInverseIsoProbabilisticTransformation()(pilotInputSample));
      Scalar bestNorm = -1.0;
      for (UnsignedInteger i = 0; i < pilotStandardSample.getSize(); ++i)
        if (pilotOutputSample(i, 0) > start)
        {
          const Point pilotPoint(pilotStandardSample[i]);
          const Scalar currentNorm = pilotPoint.norm();
          if (currentNorm > bestNorm)
          {
            bestNorm = currentNorm;
            direction = pilotPoint;
          }
        }
      if (!(direction.getSize() > 0))
      {
        for (UnsignedInteger i = 0; i < pilotStandardSample.getSize(); ++i)
        {
          const Point pilotPoint(pilotStandardSample[i]);
          const Scalar currentNorm = pilotPoint.norm();
          if (currentNorm > bestNorm)
          {
            bestNorm = currentNorm;
            direction = pilotPoint;
          }
        }
      }
    }
  }
  if (direction.getDimension() != dimension)
    throw InvalidArgumentException(HERE)
        << "The important direction dimension (" << direction.getDimension()
        << ") does not match the standard space dimension (" << dimension << ")";
  const Scalar directionNorm = direction.norm();
  if (!std::isfinite(directionNorm) || !(directionNorm > 0.0))
    throw InvalidArgumentException(HERE) << "The important direction must be finite and non-zero";
  importantDirection_ = direction.normalize();

  // Draw the orthogonal line offsets once.  They are reused at every
  // candidate threshold.
  const Sample standardSample(standardDistribution_.getSample(sampleSize_));
  orthogonalPoints_ = Sample(sampleSize_, dimension);
  originValues_ = Point(sampleSize_);
  for (UnsignedInteger i = 0; i < sampleSize_; ++i)
  {
    Point origin(standardSample[i]);
    origin -= importantDirection_ * origin.dot(importantDirection_);
    orthogonalPoints_[i] = origin;
    originValues_[i] = standardFunction_(origin)[0];
  }

  const Scalar target = 1.0 - alpha_;
  UnsignedInteger probabilityEvaluationCount = 0;
  Point lastContributions;
  Sample history(0, 2);
  const QuantileRootSolver::FunctionType objective =
      [this, target, &probabilityEvaluationCount, &lastContributions, &history](const Scalar threshold) -> Scalar
  {
    const Scalar probability = computeProbability(threshold, lastContributions);
    ++probabilityEvaluationCount;
    history.add(Point({1.0 * probabilityEvaluationCount, threshold}));
    return probability - target;
  };

  const QuantileRootSolver solver(rootTolerance_, maximumRootIteration_);
  Scalar lowerBound = 0.0;
  Scalar upperBound = 0.0;
  Scalar lowerValue = 0.0;
  Scalar upperValue = 0.0;
  solver.bracket(objective, start, lowerBound, upperBound, lowerValue, upperValue,
                 step, ResourceMap::GetAsUnsignedInteger("QuantileRootSolver-DefaultBracketExpansion"));
  Scalar residual = 0.0;
  Interval bracket;
  UnsignedInteger iterationCount = 0;
  Bool converged = false;
  const Scalar estimate = solver.solveBisection(objective, lowerBound, upperBound,
                                                lowerValue, upperValue, residual,
                                                bracket, iterationCount, converged);

  // Contributions at the returned root, used for the delta-method
  // interval.  Derivative by a central difference on the same fixed
  // line sample: no extra random draw is involved.
  const Point rootContributions(lastContributions);
  const Scalar scale = std::max(1.0, std::abs(estimate));
  const Scalar relativeStep = std::max(10.0 * rootTolerance_ * scale, 1.0e-8);
  const Scalar probabilityUp = objective(estimate + relativeStep);
  const Scalar probabilityDown = objective(estimate - relativeStep);
  const Scalar derivative = (probabilityUp - probabilityDown) / (2.0 * relativeStep);

  const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta_));
  Scalar standardError = 0.0;
  Interval interval(Point(1, estimate), Point(1, estimate));
  Bool derivativeValid = std::isfinite(derivative) && (derivative < 0.0);
  if (converged && derivativeValid)
  {
    Scalar mean = 0.0;
    for (UnsignedInteger i = 0; i < rootContributions.getSize(); ++i)
      mean += rootContributions[i];
    mean /= rootContributions.getSize();
    Scalar variance = 0.0;
    for (UnsignedInteger i = 0; i < rootContributions.getSize(); ++i)
    {
      const Scalar delta = rootContributions[i] - mean;
      variance += delta * delta;
    }
    variance /= (rootContributions.getSize() - 1);
    const Scalar probabilityStandardError = std::sqrt(std::max(0.0, variance) / rootContributions.getSize());
    standardError = probabilityStandardError / std::abs(derivative);
    if (!std::isfinite(standardError) || !(standardError >= 0.0))
    {
      standardError = 0.0;
      derivativeValid = false;
    }
    // The delta-method variance can vanish on a linear limit state.
    // The root-finding resolution then bounds the usable standard
    // error, so the interval never pretends to be narrower than the
    // root it contains.
    if (!(standardError >= rootTolerance_ * scale / z))
      standardError = rootTolerance_ * scale / z;
    const Scalar halfWidth = z * standardError;
    interval = Interval(Point(1, estimate - halfWidth), Point(1, estimate + halfWidth));
  }

  const QuantileSimulationResultImplementation::CoverageType coverage =
      (converged && derivativeValid)
      ? QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC
      : QuantileSimulationResultImplementation::COVERAGE_NONE;

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(QuantileSimulationResultImplementation::EMPIRICAL_INVERSE);
  result.setCoverageType(coverage);
  result.setRequiredCoverage(requiredCoverage_);
  result.setEffectiveSampleSize(1.0 * sampleSize_);
  result.setSampleSize(sampleSize_);
  result.setBlockLength(0);
  result.setIsFixedSample(true);
  result.setOuterSampling(1);
  result.setBlockSize(getBlockSize());
  result.setConvergenceHistory(history);
  result.setDirectionCount(sampleSize_);
  result.setConditionalProbabilitySample(rootContributions);
  result.setImportantDirection(importantDirection_);
  result.setRootBracket(bracket);
  result.setRootIterationCount(iterationCount);
  result.setProbabilityEvaluationCount(probabilityEvaluationCount);
  result.setRootSolverType(QuantileSimulationResultImplementation::BISECTION);
  result.setCommonRandomNumbers(true);
  result.setRandomStreamCapability(false);
  result.setDelegatedToDirectExtraction(false);
  return result;
}

/* String converter */
String LineSamplingQuantile::__repr__() const
{
  OSS oss;
  oss << "class=" << LineSamplingQuantile::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " initialAlpha=" << initialAlpha_
      << " hasInitialAlpha=" << hasInitialAlpha_
      << " rootTolerance=" << rootTolerance_
      << " maximumRootIteration=" << maximumRootIteration_
      << " pilotSampleSize=" << pilotSampleSize_
      << " searchOppositeDirection=" << searchOppositeDirection_
      << " rootStrategy=" << rootStrategy_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LineSamplingQuantile::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("initialAlpha_", initialAlpha_);
  adv.saveAttribute("hasInitialAlpha_", hasInitialAlpha_);
  adv.saveAttribute("rootStrategy_", rootStrategy_);
  adv.saveAttribute("searchOppositeDirection_", searchOppositeDirection_);
  adv.saveAttribute("rootTolerance_", rootTolerance_);
  adv.saveAttribute("maximumRootIteration_", maximumRootIteration_);
  adv.saveAttribute("pilotSampleSize_", pilotSampleSize_);
}

/* Method load() reloads the object from the StorageManager */
void LineSamplingQuantile::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("initialAlpha_", initialAlpha_);
  adv.loadAttribute("hasInitialAlpha_", hasInitialAlpha_);
  adv.loadAttribute("rootStrategy_", rootStrategy_);
  adv.loadAttribute("searchOppositeDirection_", searchOppositeDirection_);
  adv.loadAttribute("rootTolerance_", rootTolerance_);
  adv.loadAttribute("maximumRootIteration_", maximumRootIteration_);
  adv.loadAttribute("pilotSampleSize_", pilotSampleSize_);
}

END_NAMESPACE_OPENTURNS
