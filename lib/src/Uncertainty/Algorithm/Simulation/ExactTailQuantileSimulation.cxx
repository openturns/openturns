//                                               -*- C++ -*-
/**
 *  @brief Exact-tail quantile simulation
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
#include <queue>
#include <vector>
#include <functional>

#include "openturns/ExactTailQuantileSimulation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/QuantileConfidence.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ExactTailQuantileSimulation)

static const Factory<ExactTailQuantileSimulation> Factory_ExactTailQuantileSimulation;

/*
 * The exact-tail estimator follows the classical distribution-free
 * order-statistic construction of:
 *
 * S. S. Wilks.
 * Order statistics.
 * Bulletin of the American Mathematical Society, 54(1):6-50, 1948.
 *
 * For B ~ Binomial(N, alpha), the interval [Y_(r1+1), Y_(r2+1)] has
 * coverage P(r1 + 1 <= B <= r2).  The ranks r1 and r2 are computed before
 * sampling and depend only on N, alpha and beta.  For an upper-tail
 * quantile only the N - r1 largest observations are needed; for a
 * lower-tail quantile only the r2 + 1 smallest observations are
 * needed.  The smaller of the two retained counts is used.
 */

/* Default constructor */
ExactTailQuantileSimulation::ExactTailQuantileSimulation()
  : QuantileSimulationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
ExactTailQuantileSimulation::ExactTailQuantileSimulation(const RandomVector & output,
                                                         const Scalar alpha,
                                                         const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  // Nothing to do
}

/* Constructor with parameters and experiment */
ExactTailQuantileSimulation::ExactTailQuantileSimulation(const RandomVector & output,
                                                         const WeightedExperiment & experiment,
                                                         const Scalar alpha,
                                                         const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  setExperiment(experiment);
}

/* Virtual constructor */
ExactTailQuantileSimulation * ExactTailQuantileSimulation::clone() const
{
  return new ExactTailQuantileSimulation(*this);
}

/* Experiment accessor */
void ExactTailQuantileSimulation::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  isExperimentProvided_ = true;
  sampleSize_ = experiment_.getSize();
}

WeightedExperiment ExactTailQuantileSimulation::getExperiment() const
{
  return experiment_;
}

Bool ExactTailQuantileSimulation::hasExperiment() const
{
  return isExperimentProvided_;
}

/* Sample size accessor */
void ExactTailQuantileSimulation::setSampleSize(const UnsignedInteger sampleSize)
{
  QuantileSimulationImplementation::setSampleSize(sampleSize);
  if (isExperimentProvided_)
    experiment_.setSize(sampleSize);
}

/* Resolve the experiment, building a Monte Carlo experiment when needed */
WeightedExperiment ExactTailQuantileSimulation::resolveExperiment() const
{
  if (isExperimentProvided_)
    return experiment_;

  Distribution distribution;
  try
  {
    if (output_.isComposite())
      distribution = output_.getAntecedent().getDistribution();
    else
      distribution = output_.getDistribution();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE)
        << "No experiment was provided and the distribution of the output random vector cannot be retrieved. "
        << "Provide a WeightedExperiment explicitly.";
  }
  return MonteCarloExperiment(distribution, sampleSize_);
}

/* Check the input parameters */
void ExactTailQuantileSimulation::checkInputs() const
{
  QuantileSimulationImplementation::checkInputs();
  if (estimatorType_ != QuantileSimulationResultImplementation::EMPIRICAL_INVERSE)
    throw InvalidArgumentException(HERE)
        << "The exact-tail estimator only supports the empirical inverse estimator. "
        << "Use MonteCarloQuantileSimulation or BlockQuantileSimulation for the interpolated estimator.";
  if (ResourceMap::GetAsUnsignedInteger("ExactTailQuantileSimulation-MinimumChunkSize") == 0)
    throw InvalidArgumentException(HERE)
        << "The ResourceMap key ExactTailQuantileSimulation-MinimumChunkSize must be positive";
  if (requiredCoverage_ == QuantileSimulationResultImplementation::COVERAGE_ANYTIME_VALID)
    throw InvalidArgumentException(HERE)
        << "The exact-tail estimator cannot provide an anytime-valid coverage";
}

/* Run a fixed-sample experiment */
QuantileSimulationResult ExactTailQuantileSimulation::runFixedSample()
{
  if (sampleSize_ == 0)
    throw InvalidArgumentException(HERE) << "The sample size must be set before running the algorithm";

  WeightedExperiment experiment(resolveExperiment());
  if (!experiment.isIID())
    throw InvalidArgumentException(HERE)
        << "The exact-tail estimator requires an i.i.d. experiment, but "
        << experiment.getClassName() << " is not i.i.d. "
        << "Use a MonteCarloExperiment, or another estimator for stratified designs.";
  if (!experiment.hasUniformWeights())
    throw InvalidArgumentException(HERE)
        << "The exact-tail estimator requires uniform weights, but "
        << experiment.getClassName() << " has non-uniform weights";

  const UnsignedInteger inputDimension = checkExperiment(experiment);

  // Exact ranks are computed before sampling; they depend only on the
  // sample size and on the levels alpha and beta.
  Indices ranks;
  try
  {
    const QuantileConfidence quantileConfidence(alpha_, beta_);
    ranks = quantileConfidence.computeBilateralRank(sampleSize_);
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE)
        << "The exact-tail estimator cannot compute the exact binomial ranks for sampleSize = "
        << sampleSize_ << ", alpha = " << alpha_ << ", beta = " << beta_
        << ". Increase the sample size, decrease the confidence level, or use another quantile algorithm.";
  }

  UnsignedInteger lowerRank = ranks[0];
  UnsignedInteger upperRank = ranks[1];
  UnsignedInteger estimateIndex = static_cast<UnsignedInteger>(std::ceil(1.0 * sampleSize_ * alpha_));
  if (estimateIndex > 0)
    --estimateIndex;
  if (estimateIndex >= sampleSize_)
    estimateIndex = sampleSize_ - 1;

  // The reported interval always contains the point estimate.
  lowerRank = std::min(lowerRank, estimateIndex);
  upperRank = std::max(upperRank, estimateIndex);

  // An upper-tail quantile needs the N - lowerRank largest values; a
  // lower-tail quantile needs the upperRank + 1 smallest values.
  const UnsignedInteger upperRetainedCount = sampleSize_ - lowerRank;
  const UnsignedInteger lowerRetainedCount = upperRank + 1;
  QuantileSimulationResultImplementation::TailType tailType;
  UnsignedInteger retainedCount = 0;
  UnsignedInteger offset = 0;
  if (lowerRetainedCount <= upperRetainedCount)
  {
    tailType = QuantileSimulationResultImplementation::LOWER_TAIL;
    // Keep one observation above the estimate when it exists, so that the
    // spacing estimate below always uses the (k-1, k+1) window of the
    // full-sample estimator.
    retainedCount = std::min(sampleSize_,
        std::max(lowerRetainedCount, estimateIndex + 2));
    offset = 0;
  }
  else
  {
    tailType = QuantileSimulationResultImplementation::UPPER_TAIL;
    // Keep one observation below the estimate when it exists, for the same
    // reason.  Without this, a lower confidence rank clamped to the
    // estimate would leave only the (k, k+1) spacing.
    offset = (estimateIndex > 0) ? std::min(lowerRank, estimateIndex - 1) : lowerRank;
    retainedCount = sampleSize_ - offset;
  }

  // A min-heap of size K keeps the K largest observations; a max-heap
  // keeps the K smallest observations.  std::priority_queue requires an
  // STL sequence container and Collection does not provide the required
  // front/back/push_back/pop_back interface.
  std::priority_queue<Scalar, std::vector<Scalar>, std::greater<Scalar> > upperHeap;
  std::priority_queue<Scalar> lowerHeap;
  const UnsignedInteger chunkSize = std::max<UnsignedInteger>(getBlockSize(),
      ResourceMap::GetAsUnsignedInteger("ExactTailQuantileSimulation-MinimumChunkSize"));
  UnsignedInteger remaining = sampleSize_;
  while (remaining > 0)
  {
    const UnsignedInteger currentSize = std::min<UnsignedInteger>(remaining, chunkSize);
    experiment.setSize(currentSize);
    Point weights;
    const Sample inputSample(experiment.generateWithWeights(weights));
    if (inputSample.getSize() != currentSize)
      throw InternalException(HERE)
          << "The experiment returned " << inputSample.getSize()
          << " points instead of the requested chunk size " << currentSize;
    const Sample outputSample(output_.getFrozenSample(inputSample));
    if (outputSample.getDimension() != 1)
      throw InvalidArgumentException(HERE)
          << "The exact-tail estimator requires a scalar output, but the output dimension is "
          << outputSample.getDimension();
    for (UnsignedInteger i = 0; i < currentSize; ++i)
    {
      const Scalar value = outputSample(i, 0);
      if (std::isnan(value))
        throw InvalidArgumentException(HERE)
            << "The exact-tail estimator cannot process a NaN output value";
      if (tailType == QuantileSimulationResultImplementation::UPPER_TAIL)
      {
        if (upperHeap.size() < retainedCount)
          upperHeap.push(value);
        else if (value > upperHeap.top())
        {
          upperHeap.pop();
          upperHeap.push(value);
        }
      }
      else
      {
        if (lowerHeap.size() < retainedCount)
          lowerHeap.push(value);
        else if (value < lowerHeap.top())
        {
          lowerHeap.pop();
          lowerHeap.push(value);
        }
      }
    }
    remaining -= currentSize;
  }

  // Extract the retained values in increasing order.
  Point retainedValues(retainedCount);
  if (tailType == QuantileSimulationResultImplementation::UPPER_TAIL)
  {
    for (UnsignedInteger i = 0; i < retainedCount; ++i)
    {
      retainedValues[i] = upperHeap.top();
      upperHeap.pop();
    }
  }
  else
  {
    for (UnsignedInteger i = retainedCount; i > 0; --i)
    {
      retainedValues[i - 1] = lowerHeap.top();
      lowerHeap.pop();
    }
  }

  const Scalar estimate = retainedValues[estimateIndex - offset];
  const Scalar lowerBound = retainedValues[lowerRank - offset];
  const Scalar upperBound = retainedValues[upperRank - offset];
  const Interval interval(lowerBound, upperBound);

  // Spacing estimate of the density at the quantile, as in the
  // fixed-sample empirical-inverse estimator.
  Scalar standardError = 0.0;
  const UnsignedInteger localIndex = estimateIndex - offset;
  const UnsignedInteger localLower = (localIndex > 0) ? localIndex - 1 : 0;
  const UnsignedInteger localUpper = (localIndex + 1 < retainedCount) ? localIndex + 1 : retainedCount - 1;
  if (localUpper > localLower)
  {
    const Scalar width = retainedValues[localUpper] - retainedValues[localLower];
    if (width > 0.0)
    {
      const Scalar density = (1.0 * (localUpper - localLower)) / (1.0 * sampleSize_ * width);
      standardError = std::sqrt(alpha_ * (1.0 - alpha_) / (1.0 * sampleSize_)) / density;
    }
  }
  if (!(standardError > 0.0) || !std::isfinite(standardError))
  {
    const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta_));
    standardError = (upperBound - lowerBound) / (2.0 * z);
    if (!(standardError >= 0.0) || !std::isfinite(standardError))
      standardError = 0.0;
  }

  Sample history(1, 4);
  history(0, 0) = sampleSize_;
  history(0, 1) = estimate;
  history(0, 2) = lowerBound;
  history(0, 3) = upperBound;

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(QuantileSimulationResultImplementation::EMPIRICAL_INVERSE);
  result.setCoverageType(QuantileSimulationResultImplementation::COVERAGE_EXACT);
  result.setRequiredCoverage(requiredCoverage_);
  result.setEffectiveSampleSize(1.0 * sampleSize_);
  result.setSampleSize(sampleSize_);
  result.setBlockLength(0);
  result.setBlockCount(0);
  result.setTailType(tailType);
  result.setRetainedCount(retainedCount);
  result.setLowerRank(lowerRank);
  result.setUpperRank(upperRank);
  // Peak storage: the retained tail plus the input and output samples of
  // the chunk being processed.
  result.setPeakMemory(retainedCount + (inputDimension + 1) * std::min(sampleSize_, chunkSize));
  result.setIsFixedSample(true);
  result.setOuterSampling(1);
  result.setBlockSize(getBlockSize());
  result.setConvergenceHistory(history);
  return result;
}

/* String converter */
String ExactTailQuantileSimulation::__repr__() const
{
  OSS oss;
  oss << "class=" << ExactTailQuantileSimulation::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " estimator=" << estimatorType_
      << " requiredCoverage=" << requiredCoverage_
      << " experimentProvided=" << isExperimentProvided_;
  if (isExperimentProvided_)
    oss << " experiment=" << experiment_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void ExactTailQuantileSimulation::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("isExperimentProvided_", isExperimentProvided_);
}

/* Method load() reloads the object through the StorageManager */
void ExactTailQuantileSimulation::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("isExperimentProvided_", isExperimentProvided_);
}

END_NAMESPACE_OPENTURNS
