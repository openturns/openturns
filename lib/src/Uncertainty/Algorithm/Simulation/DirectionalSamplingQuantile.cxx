//                                               -*- C++ -*-
/**
 *  @brief Directional-simulation quantile estimator
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

#include "openturns/DirectionalSamplingQuantile.hxx"
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
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DirectionalSamplingQuantile)

static const Factory<DirectionalSamplingQuantile> Factory_DirectionalSamplingQuantile;

namespace
{
/** Empirical inverse at level alpha, using the same convention as the
    fixed-sample quantile estimators. */
Scalar directionalPilotQuantile(const Sample & sample, const Scalar alpha)
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
Scalar directionalPilotScale(const Sample & sample)
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
DirectionalSamplingQuantile::DirectionalSamplingQuantile()
  : QuantileSimulationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
DirectionalSamplingQuantile::DirectionalSamplingQuantile(const RandomVector & output,
                                                         const Scalar alpha,
                                                         const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  // Nothing to do
}

/* Virtual constructor */
DirectionalSamplingQuantile * DirectionalSamplingQuantile::clone() const
{
  return new DirectionalSamplingQuantile(*this);
}

/* Root strategy accessor */
void DirectionalSamplingQuantile::setRootStrategy(const RootStrategy & rootStrategy)
{
  rootStrategy_ = rootStrategy;
}

RootStrategy DirectionalSamplingQuantile::getRootStrategy() const
{
  return rootStrategy_;
}

/* Sampling strategy accessor */
void DirectionalSamplingQuantile::setSamplingStrategy(const SamplingStrategy & samplingStrategy)
{
  samplingStrategy_ = samplingStrategy;
}

SamplingStrategy DirectionalSamplingQuantile::getSamplingStrategy() const
{
  return samplingStrategy_;
}

/* Root-finding tolerance accessor */
void DirectionalSamplingQuantile::setRootTolerance(const Scalar tolerance)
{
  if (!std::isfinite(tolerance) || !(tolerance > 0.0))
    throw InvalidArgumentException(HERE)
        << "The root-finding tolerance must be finite and positive, but tolerance = "
        << tolerance;
  rootTolerance_ = tolerance;
}

Scalar DirectionalSamplingQuantile::getRootTolerance() const
{
  return rootTolerance_;
}

/* Maximum root-finding iteration accessor */
void DirectionalSamplingQuantile::setMaximumRootIteration(const UnsignedInteger maximumIteration)
{
  if (maximumIteration == 0)
    throw InvalidArgumentException(HERE)
        << "The maximum root-finding iteration count must be positive";
  maximumRootIteration_ = maximumIteration;
}

UnsignedInteger DirectionalSamplingQuantile::getMaximumRootIteration() const
{
  return maximumRootIteration_;
}

/* Pilot sample size accessor */
void DirectionalSamplingQuantile::setPilotSampleSize(const UnsignedInteger pilotSampleSize)
{
  if (pilotSampleSize == 0)
    throw InvalidArgumentException(HERE)
        << "The pilot sample size must be positive";
  pilotSampleSize_ = pilotSampleSize;
}

UnsignedInteger DirectionalSamplingQuantile::getPilotSampleSize() const
{
  return pilotSampleSize_;
}

/* Probability estimate and conditional contributions at a threshold.
   The direction sample is fixed, so this is a deterministic function of
   the threshold for a given run. */
Scalar DirectionalSamplingQuantile::computeProbability(const Scalar threshold,
                                                       Point & contributions)
{
  const UnsignedInteger size = directions_.getSize();
  const UnsignedInteger dimension = standardDistribution_.getDimension();
  contributions = Point(size);
  const Bool originInEvent = originValue_ > threshold;
  rootStrategy_.setOriginValue(originValue_);
  Matrix linear(dimension, 1);
  Scalar sum = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Point direction(directions_[i]);
    for (UnsignedInteger j = 0; j < dimension; ++j)
      linear(j, 0) = direction[j];
    const LinearFunction ray(Point(1, 0.0), originPoint_, linear);
    const ComposedFunction functionAlongRay(standardFunction_, ray);
    const RootStrategy::ScalarCollection roots(rootStrategy_.solve(functionAlongRay, threshold));
    Scalar rayEstimate = 0.0;
    Scalar sign = 1.0;
    for (UnsignedInteger k = 0; k < roots.getSize(); ++k)
    {
      rayEstimate += sign * standardDistribution_.computeRadialDistributionCDF(roots[k], true);
      sign = -sign;
    }
    Scalar contribution = originInEvent ? 1.0 - rayEstimate : rayEstimate;
    contribution = std::min(1.0, std::max(0.0, contribution));
    contributions[i] = contribution;
    sum += contribution;
  }
  return sum / size;
}

/* Run the directional root-finding experiment */
QuantileSimulationResult DirectionalSamplingQuantile::runFixedSample()
{
  if (sampleSize_ < 2)
    throw InvalidArgumentException(HERE)
        << "At least two directions are required for a delta-method interval, but sampleSize = "
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
  originPoint_ = Point(dimension);
  originValue_ = standardFunction_(originPoint_)[0];

  // Draw the direction sample once.  Reusing it at every candidate
  // threshold is the common-random-number construction that makes the
  // outer root well defined.
  samplingStrategy_.setDimension(dimension);
  directions_ = Sample(0, dimension);
  while (directions_.getSize() < sampleSize_)
    directions_.add(samplingStrategy_.generate());
  if (directions_.getSize() > sampleSize_)
  {
    Sample truncatedSample(sampleSize_, dimension);
    for (UnsignedInteger i = 0; i < sampleSize_; ++i)
      for (UnsignedInteger j = 0; j < dimension; ++j)
        truncatedSample(i, j) = directions_(i, j);
    directions_ = truncatedSample;
  }

  // Pilot sample used only to start and scale the bracket.
  const Sample pilotSample(output_.getSample(pilotSampleSize_));
  const Scalar start = directionalPilotQuantile(pilotSample, alpha_);
  const Scalar step = directionalPilotScale(pilotSample);

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
  // direction sample: no extra random draw is involved.
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
String DirectionalSamplingQuantile::__repr__() const
{
  OSS oss;
  oss << "class=" << DirectionalSamplingQuantile::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " rootTolerance=" << rootTolerance_
      << " maximumRootIteration=" << maximumRootIteration_
      << " pilotSampleSize=" << pilotSampleSize_
      << " rootStrategy=" << rootStrategy_
      << " samplingStrategy=" << samplingStrategy_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void DirectionalSamplingQuantile::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("rootStrategy_", rootStrategy_);
  adv.saveAttribute("samplingStrategy_", samplingStrategy_);
  adv.saveAttribute("rootTolerance_", rootTolerance_);
  adv.saveAttribute("maximumRootIteration_", maximumRootIteration_);
  adv.saveAttribute("pilotSampleSize_", pilotSampleSize_);
}

/* Method load() reloads the object from the StorageManager */
void DirectionalSamplingQuantile::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("rootStrategy_", rootStrategy_);
  adv.loadAttribute("samplingStrategy_", samplingStrategy_);
  adv.loadAttribute("rootTolerance_", rootTolerance_);
  adv.loadAttribute("maximumRootIteration_", maximumRootIteration_);
  adv.loadAttribute("pilotSampleSize_", pilotSampleSize_);
}

END_NAMESPACE_OPENTURNS
