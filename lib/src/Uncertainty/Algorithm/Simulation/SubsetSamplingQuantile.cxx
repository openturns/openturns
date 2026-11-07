//                                               -*- C++ -*-
/**
 *  @brief Subset-simulation quantile estimator
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

#include "openturns/SubsetSamplingQuantile.hxx"
#include "openturns/QuantileConditionalSampler.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Less.hxx"
#include "openturns/Greater.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SubsetSamplingQuantile)

static const Factory<SubsetSamplingQuantile> Factory_SubsetSamplingQuantile;

namespace
{

Scalar subsetQuantileMean(const Point & values)
{
  const UnsignedInteger size = values.getSize();
  if (size == 0)
    return 0.0;
  Scalar mean = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
    mean += values[i];
  return mean / (1.0 * size);
}

Scalar subsetQuantileStandardDeviation(const Point & values)
{
  const UnsignedInteger size = values.getSize();
  if (size < 2)
    return 0.0;
  const Scalar mean = subsetQuantileMean(values);
  Scalar sumSquare = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar delta = values[i] - mean;
    sumSquare += delta * delta;
  }
  return std::sqrt(std::max(0.0, sumSquare / (1.0 * (size - 1))));
}

Scalar subsetQuantileEmpiricalInverse(const Sample & sample, const Scalar level)
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The sample must not be empty";
  Point values(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    values[i] = sample(i, 0);
  std::sort(values.begin(), values.end());
  if (level <= 0.0)
    return values[0];
  if (level >= 1.0)
    return values[size - 1];
  UnsignedInteger index = static_cast<UnsignedInteger>(std::ceil(level * (1.0 * size)));
  if (index == 0)
    index = 1;
  if (index > size)
    index = size;
  return values[index - 1];
}

UnsignedInteger subsetQuantileLevelCount(const Scalar probability, const Scalar conditionalProbability)
{
  if (probability >= conditionalProbability)
    return 1;
  const Scalar ratio = std::log(probability) / std::log(conditionalProbability);
  const Scalar rounded = std::ceil(ratio);
  if (!(rounded >= 1.0))
    return 1;
  return static_cast<UnsignedInteger>(rounded);
}

} // anonymous namespace

/* Default constructor */
SubsetSamplingQuantile::SubsetSamplingQuantile()
  : QuantileSimulationImplementation()
{
  sampleSize_ = getMaximumOuterSampling() * getBlockSize();
}

/* Constructor with parameters */
SubsetSamplingQuantile::SubsetSamplingQuantile(const RandomVector & output,
                                               const Scalar alpha,
                                               const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  sampleSize_ = getMaximumOuterSampling() * getBlockSize();
}

/* Virtual constructor */
SubsetSamplingQuantile * SubsetSamplingQuantile::clone() const
{
  return new SubsetSamplingQuantile(*this);
}

/* Conditional probability accessor */
void SubsetSamplingQuantile::setConditionalProbability(const Scalar conditionalProbability)
{
  if (!(conditionalProbability > 0.0) || !(conditionalProbability < 1.0))
    throw InvalidArgumentException(HERE)
        << "The conditional probability must be in (0, 1), but conditionalProbability = "
        << conditionalProbability;
  conditionalProbability_ = conditionalProbability;
}

Scalar SubsetSamplingQuantile::getConditionalProbability() const
{
  return conditionalProbability_;
}

/* Proposal range accessor */
void SubsetSamplingQuantile::setProposalRange(const Scalar proposalRange)
{
  if (!(proposalRange > 0.0) || !std::isfinite(proposalRange))
    throw InvalidArgumentException(HERE)
        << "The proposal range must be finite and positive, but proposalRange = " << proposalRange;
  proposalRange_ = proposalRange;
}

Scalar SubsetSamplingQuantile::getProposalRange() const
{
  return proposalRange_;
}

/* Burn-in accessor */
void SubsetSamplingQuantile::setBurnIn(const UnsignedInteger burnIn)
{
  if (burnIn == 0)
    throw InvalidArgumentException(HERE) << "The burn-in must be positive";
  burnIn_ = burnIn;
}

UnsignedInteger SubsetSamplingQuantile::getBurnIn() const
{
  return burnIn_;
}

/* Replicate count accessor */
void SubsetSamplingQuantile::setReplicateCount(const UnsignedInteger replicateCount)
{
  if (replicateCount < 2)
    throw InvalidArgumentException(HERE)
        << "The replicate count must be at least 2, but replicateCount = " << replicateCount;
  replicateCount_ = replicateCount;
}

UnsignedInteger SubsetSamplingQuantile::getReplicateCount() const
{
  return replicateCount_;
}

/* Particle count accessor */
void SubsetSamplingQuantile::setParticleCount(const UnsignedInteger particleCount)
{
  setSampleSize(particleCount);
}

UnsignedInteger SubsetSamplingQuantile::getParticleCount() const
{
  return resolveParticleCount();
}

/* Sample size accessor */
void SubsetSamplingQuantile::setSampleSize(const UnsignedInteger sampleSize)
{
  QuantileSimulationImplementation::setSampleSize(sampleSize);
  isSampleSizeProvided_ = true;
}

UnsignedInteger SubsetSamplingQuantile::getSampleSize() const
{
  return resolveParticleCount();
}

/* Population size used by the level loop */
UnsignedInteger SubsetSamplingQuantile::resolveParticleCount() const
{
  if (isSampleSizeProvided_)
    return sampleSize_;
  return getMaximumOuterSampling() * getBlockSize();
}

/* Run a fixed-level subset experiment */
QuantileSimulationResult SubsetSamplingQuantile::runFixedSample()
{
  const UnsignedInteger particleCount = resolveParticleCount();
  if (particleCount < 2)
    throw InvalidArgumentException(HERE)
        << "The subset quantile estimator requires at least two particles, but particleCount = "
        << particleCount;
  if (1.0 * particleCount * conditionalProbability_ < 1.0)
    throw InvalidArgumentException(HERE)
        << "The particle count (" << particleCount
        << ") is too small for the conditional probability (" << conditionalProbability_
        << "): at least " << std::ceil(1.0 / conditionalProbability_) << " particles are required";

  const Bool lowerTail = alpha_ <= 0.5;
  const Scalar tailProbability = lowerTail ? alpha_ : 1.0 - alpha_;
  const ComparisonOperator op = lowerTail ? ComparisonOperator(Less()) : ComparisonOperator(Greater());
  const Scalar conditionalProbability = conditionalProbability_;

  // The number of levels is fixed before seeing the data.  With
  // m levels, the final conditional sample is conditioned on the
  // (m-1)-th threshold and the target conditional tail probability is
  // p / p0^(m-1).
  const UnsignedInteger levelCount = subsetQuantileLevelCount(tailProbability, conditionalProbability);
  const UnsignedInteger conditionalLevelCount = levelCount - 1;
  const Scalar targetConditionalProbability = tailProbability
      / std::pow(conditionalProbability, 1.0 * conditionalLevelCount);
  const Scalar conditionalLevel = lowerTail
                                  ? targetConditionalProbability
                                  : 1.0 - targetConditionalProbability;
  if (!(targetConditionalProbability > 0.0) || !(targetConditionalProbability <= 1.0))
    throw InternalException(HERE)
        << "Invalid conditional tail probability: " << targetConditionalProbability;

  const UnsignedInteger replicateCount = replicateCount_;
  Point replicateEstimates(replicateCount);
  Point firstThresholds;
  Point firstLevelProbabilities;
  UnsignedInteger totalTransitions = 0;
  UnsignedInteger acceptedTransitions = 0;
  UnsignedInteger totalEvaluations = 0;

  Sample history(replicateCount, 4);
  Scalar runningMean = 0.0;
  Scalar runningSumSquare = 0.0;

  for (UnsignedInteger replicate = 0; replicate < replicateCount; ++replicate)
  {
    QuantileConditionalSampler sampler(output_, proposalRange_, burnIn_);
    sampler.resetDiagnostics();
    Sample standardSample(sampler.generatePopulation(particleCount));
    Sample outputSample(sampler.evaluate(standardSample));

    Point thresholds(conditionalLevelCount);
    Point levelProbabilities(conditionalLevelCount);
    for (UnsignedInteger level = 0; level < conditionalLevelCount; ++level)
    {
      const Scalar ratio = lowerTail ? conditionalProbability : 1.0 - conditionalProbability;
      const Scalar threshold = outputSample.computeQuantile(ratio)[0];
      thresholds[level] = threshold;
      levelProbabilities[level] = std::pow(conditionalProbability, 1.0 * (level + 1));

      Indices survivors;
      for (UnsignedInteger i = 0; i < particleCount; ++i)
        if (op(outputSample(i, 0), threshold))
          survivors.add(i);
      if (survivors.getSize() == 0)
        throw NotDefinedException(HERE)
            << "No particle satisfies the level threshold " << threshold
            << " at level " << (level + 1) << ". Increase the particle count or the conditional probability.";

      UnsignedInteger seedIndex = 0;
      for (UnsignedInteger i = 0; i < particleCount; ++i)
      {
        if (op(outputSample(i, 0), threshold))
          continue;
        const Point & seed = standardSample[survivors[seedIndex % survivors.getSize()]];
        const Point replacement(sampler.sample(seed, op, threshold));
        standardSample[i] = replacement;
        outputSample(i, 0) = sampler.evaluate(replacement);
        ++seedIndex;
      }
    }

    replicateEstimates[replicate] = subsetQuantileEmpiricalInverse(outputSample, conditionalLevel);
    if (replicate == 0)
    {
      firstThresholds = thresholds;
      firstLevelProbabilities = levelProbabilities;
    }
    totalTransitions += sampler.getTransitionCount();
    acceptedTransitions += sampler.getAcceptedTransitionCount();
    totalEvaluations += sampler.getEvaluationCount();

    const Scalar count = 1.0 * (replicate + 1);
    const Scalar delta = replicateEstimates[replicate] - runningMean;
    runningMean += delta / count;
    runningSumSquare += delta * (replicateEstimates[replicate] - runningMean);
    history(replicate, 0) = count;
    history(replicate, 1) = runningMean;
    if (replicate == 0)
    {
      history(replicate, 2) = replicateEstimates[replicate];
      history(replicate, 3) = replicateEstimates[replicate];
    }
    else
    {
      const Scalar variance = runningSumSquare / (1.0 * replicate);
      const Scalar standardError = std::sqrt(std::max(0.0, variance) / count);
      const Scalar t = DistFunc::qStudent(1.0 * replicate, 0.5 * (1.0 + beta_));
      history(replicate, 2) = runningMean - t * standardError;
      history(replicate, 3) = runningMean + t * standardError;
    }
  }

  const Scalar estimate = subsetQuantileMean(replicateEstimates);
  const Scalar standardError = subsetQuantileStandardDeviation(replicateEstimates) / std::sqrt(1.0 * replicateCount);
  const Scalar t = DistFunc::qStudent(1.0 * (replicateCount - 1), 0.5 * (1.0 + beta_));
  const Interval interval(estimate - t * standardError, estimate + t * standardError);
  const Scalar acceptanceRate = (totalTransitions > 0)
                                ? 1.0 * acceptedTransitions / (1.0 * totalTransitions) : 0.0;

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(estimatorType_);
  result.setCoverageType(QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC);
  result.setRequiredCoverage(requiredCoverage_);
  result.setEffectiveSampleSize(1.0 * replicateCount);
  result.setSampleSize(totalEvaluations);
  result.setIsFixedSample(false);
  result.setTailType(lowerTail ? QuantileSimulationResultImplementation::LOWER_TAIL
                     : QuantileSimulationResultImplementation::UPPER_TAIL);
  result.setThresholdSequence(firstThresholds);
  result.setLevelProbabilitySequence(firstLevelProbabilities);
  result.setStepCount(conditionalLevelCount);
  result.setAcceptanceRate(acceptanceRate);
  result.setParticleCount(particleCount);
  result.setReplicateCount(replicateCount);
  result.setReplicateEstimates(replicateEstimates);
  result.setConditionalProbability(conditionalProbability);
  result.setOuterSampling(replicateCount);
  result.setBlockSize(getBlockSize());
  result.setConvergenceHistory(history);
  return result;
}

/* String converter */
String SubsetSamplingQuantile::__repr__() const
{
  OSS oss;
  oss << "class=" << SubsetSamplingQuantile::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " particleCount=" << resolveParticleCount()
      << " conditionalProbability=" << conditionalProbability_
      << " proposalRange=" << proposalRange_
      << " burnIn=" << burnIn_
      << " replicateCount=" << replicateCount_
      << " estimator=" << estimatorType_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SubsetSamplingQuantile::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("conditionalProbability_", conditionalProbability_);
  adv.saveAttribute("proposalRange_", proposalRange_);
  adv.saveAttribute("burnIn_", burnIn_);
  adv.saveAttribute("replicateCount_", replicateCount_);
  adv.saveAttribute("isSampleSizeProvided_", isSampleSizeProvided_);
}

/* Method load() reloads the object from the StorageManager */
void SubsetSamplingQuantile::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("conditionalProbability_", conditionalProbability_);
  adv.loadAttribute("proposalRange_", proposalRange_);
  adv.loadAttribute("burnIn_", burnIn_);
  adv.loadAttribute("replicateCount_", replicateCount_);
  adv.loadAttribute("isSampleSizeProvided_", isSampleSizeProvided_);
}

END_NAMESPACE_OPENTURNS
