//                                               -*- C++ -*-
/**
 *  @brief Cross-entropy quantile estimator
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
#include <chrono>
#include <limits>
#include <utility>

#include "openturns/CrossEntropyQuantile.hxx"
#include "openturns/QuantileConditionalSampler.hxx"
#include "openturns/StandardSpaceCrossEntropyImportanceSampling.hxx"
#include "openturns/CrossEntropyResult.hxx"
#include "openturns/ThresholdEvent.hxx"
#include "openturns/CompositeRandomVector.hxx"
#include "openturns/ComposedFunction.hxx"
#include "openturns/Normal.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Less.hxx"
#include "openturns/Greater.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CrossEntropyQuantile)

static const Factory<CrossEntropyQuantile> Factory_CrossEntropyQuantile;

/* Default constructor */
CrossEntropyQuantile::CrossEntropyQuantile()
  : QuantileSimulationImplementation()
{
  sampleSize_ = getMaximumOuterSampling() * getBlockSize();
}

/* Constructor with parameters */
CrossEntropyQuantile::CrossEntropyQuantile(const RandomVector & output,
                                           const Scalar alpha,
                                           const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  sampleSize_ = getMaximumOuterSampling() * getBlockSize();
}

/* Virtual constructor */
CrossEntropyQuantile * CrossEntropyQuantile::clone() const
{
  return new CrossEntropyQuantile(*this);
}

/* Intermediate quantile level accessor */
void CrossEntropyQuantile::setIntermediateQuantileLevel(const Scalar level)
{
  if (!(level > 0.0) || !(level < 1.0))
    throw InvalidArgumentException(HERE)
        << "The intermediate quantile level must be in (0, 1), but level = " << level;
  intermediateQuantileLevel_ = level;
}

Scalar CrossEntropyQuantile::getIntermediateQuantileLevel() const
{
  return intermediateQuantileLevel_;
}

/* Pilot sample size accessor */
void CrossEntropyQuantile::setPilotSize(const UnsignedInteger pilotSize)
{
  if (pilotSize < 2)
    throw InvalidArgumentException(HERE)
        << "The pilot size must be at least 2, but pilotSize = " << pilotSize;
  pilotSize_ = pilotSize;
}

UnsignedInteger CrossEntropyQuantile::getPilotSize() const
{
  return pilotSize_;
}

/* Minimum proposal standard deviation accessor */
void CrossEntropyQuantile::setMinimumProposalStandardDeviation(const Scalar value)
{
  if (!(value >= 1.0) || !std::isfinite(value))
    throw InvalidArgumentException(HERE)
        << "The minimum proposal standard deviation must be finite and at least 1, but value = "
        << value;
  minimumProposalStandardDeviation_ = value;
}

Scalar CrossEntropyQuantile::getMinimumProposalStandardDeviation() const
{
  return minimumProposalStandardDeviation_;
}

/* Check the input parameters */
void CrossEntropyQuantile::checkInputs() const
{
  QuantileSimulationImplementation::checkInputs();
  if (estimatorType_ != QuantileSimulationResultImplementation::EMPIRICAL_INVERSE)
    throw InvalidArgumentException(HERE)
        << "The cross-entropy estimator only supports the empirical inverse estimator, "
        << "but the interpolated estimator was requested. "
        << "Use MonteCarloQuantileSimulation or BlockQuantileSimulation for the interpolated estimator.";
}

/* Run the cross-entropy sequence */
QuantileSimulationResult CrossEntropyQuantile::runFixedSample()
{
  if (sampleSize_ < 2)
    throw InvalidArgumentException(HERE)
        << "The cross-entropy estimator requires at least two samples, but sampleSize = "
        << sampleSize_;
  if (pilotSize_ < 2)
    throw InvalidArgumentException(HERE)
        << "The pilot size must be at least 2, but pilotSize = " << pilotSize_;

  // The outer run limits apply to the whole estimator, including the
  // pilot stage and the final importance-sampling stage.  The engine only
  // sees its own share of the time budget.
  const std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
  const auto elapsedTime = [&startTime]()
  {
    return std::chrono::duration<Scalar>(std::chrono::steady_clock::now() - startTime).count();
  };
  const auto checkRunLimits = [&](const char * stage)
  {
    const Scalar elapsed = elapsedTime();
    const Scalar maximumDuration = getMaximumTimeDuration();
    if ((maximumDuration > 0.0) && (elapsed > maximumDuration))
      throw TimeoutException(HERE)
          << "Duration (" << elapsed << "s) exceeds maximum duration ("
          << maximumDuration << " s) during " << stage;
    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InterruptionException(HERE) << "User stopped simulation during " << stage;
  };

  const Bool lowerTail = alpha_ <= 0.5;
  const ComparisonOperator op = lowerTail ? ComparisonOperator(Less()) : ComparisonOperator(Greater());

  // A pilot sample locates the target tail.  The pilot only selects the
  // event threshold of the cross-entropy run: the final estimator is a
  // self-normalized importance-sampling quantile, which remains
  // consistent for any auxiliary distribution with full support.
  checkRunLimits("the pilot sampling");
  const Sample pilotOutput(output_.getSample(pilotSize_));
  checkRunLimits("the pilot sampling");
  if (pilotOutput.getDimension() != 1)
    throw InvalidArgumentException(HERE)
        << "The output random vector must be scalar, but its dimension is "
        << pilotOutput.getDimension();
  const Scalar pilotThreshold = pilotOutput.computeQuantile(alpha_)[0];
  if (!std::isfinite(pilotThreshold))
    throw NotDefinedException(HERE)
        << "The pilot sample did not produce a finite threshold";

  const CompositeRandomVector composite(QuantileConditionalSampler::buildCompositeOutput(output_));
  const Distribution inputDistribution(composite.getAntecedent().getDistribution());
  const Function limitState(ComposedFunction(composite.getFunction(), inputDistribution.getInverseIsoProbabilisticTransformation()));
  const ThresholdEvent event(composite, op, pilotThreshold);
  StandardSpaceCrossEntropyImportanceSampling engine(event, intermediateQuantileLevel_);
  const UnsignedInteger blockSize = std::max<UnsignedInteger>(1, getBlockSize());
  engine.setBlockSize(blockSize);
  engine.setMaximumOuterSampling((sampleSize_ + blockSize - 1) / blockSize);
  engine.setStopCallback(stopCallback_.first, stopCallback_.second);
  const Scalar maximumDuration = getMaximumTimeDuration();
  if (maximumDuration > 0.0)
  {
    const Scalar remainingDuration = maximumDuration - elapsedTime();
    if (!(remainingDuration > 0.0))
      throw TimeoutException(HERE)
          << "Duration (" << elapsedTime() << "s) exceeds maximum duration ("
          << maximumDuration << " s) before the cross-entropy sampling";
    engine.setMaximumTimeDuration(remainingDuration);
  }
  engine.run();
  checkRunLimits("the cross-entropy sampling");

  const CrossEntropyResult crossEntropyResult(engine.getResult());
  const Distribution crossEntropyDistribution(crossEntropyResult.getAuxiliaryDistribution());
  const UnsignedInteger dimension = crossEntropyDistribution.getDimension();
  const Point defensiveMean(crossEntropyDistribution.getMean());
  const Point crossEntropySigma(crossEntropyDistribution.getStandardDeviation());
  if ((defensiveMean.getSize() != dimension) || (crossEntropySigma.getSize() != dimension))
    throw InternalException(HERE)
        << "The cross-entropy auxiliary distribution does not expose a mean and a standard deviation of dimension "
        << dimension;

  // The cross-entropy distribution concentrates on the target tail and its
  // standard deviations are typically smaller than one in the standard
  // space.  The raw importance weights then have an infinite variance.  The
  // defensive proposal keeps the cross-entropy mean but inflates every
  // standard deviation to a floor of at least one, which restores a finite
  // variance.  The final weighted estimate is computed from draws of this
  // proposal.
  Point defensiveSigma(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
    defensiveSigma[i] = std::max(crossEntropySigma[i], minimumProposalStandardDeviation_);
  const Normal defensiveDistribution(defensiveMean, defensiveSigma);
  const Distribution auxiliaryDistribution(defensiveDistribution);
  // Draw and evaluate the final importance sample by chunks so that the
  // outer stop callback and time limit are checked during this stage too.
  Sample inputSample(0, defensiveDistribution.getDimension());
  Sample outputSample(0, 1);
  for (UnsignedInteger start = 0; start < sampleSize_; start += blockSize)
  {
    const UnsignedInteger chunkSize = std::min(blockSize, sampleSize_ - start);
    const Sample chunkInput(defensiveDistribution.getSample(chunkSize));
    inputSample.add(chunkInput);
    outputSample.add(limitState(chunkInput));
    checkRunLimits("the final sampling");
  }
  const UnsignedInteger sampleSize = inputSample.getSize();
  if (sampleSize < 2)
    throw NotDefinedException(HERE)
        << "The cross-entropy run did not produce enough auxiliary samples";
  if (outputSample.getDimension() != 1)
    throw InternalException(HERE)
        << "The cross-entropy output dimension is " << outputSample.getDimension()
        << " instead of 1";

  // Self-normalized importance weights, stabilized in the log domain.
  const Normal initialDistribution(inputSample.getDimension());
  const Sample logInitialPDF(initialDistribution.computeLogPDF(inputSample));
  const Sample logAuxiliaryPDF(auxiliaryDistribution.computeLogPDF(inputSample));
  Point logWeights(sampleSize);
  Scalar maxLogWeight = -std::numeric_limits<Scalar>::infinity();
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    logWeights[i] = logInitialPDF(i, 0) - logAuxiliaryPDF(i, 0);
    if (logWeights[i] > maxLogWeight)
      maxLogWeight = logWeights[i];
  }
  if (!std::isfinite(maxLogWeight))
    throw NotDefinedException(HERE)
        << "All the importance weights are zero or non-finite";

  Point weights(sampleSize);
  Scalar sumWeights = 0.0;
  Scalar sumSquareWeights = 0.0;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    weights[i] = std::exp(logWeights[i] - maxLogWeight);
    sumWeights += weights[i];
    sumSquareWeights += weights[i] * weights[i];
  }
  if (!(sumWeights > 0.0) || !(sumSquareWeights > 0.0))
    throw NotDefinedException(HERE)
        << "The importance weights have a zero sum";

  // Sort by output value; ties are broken by the weight.
  Collection<std::pair<Scalar, Scalar> > sortedValues(sampleSize);
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
    sortedValues[i] = std::make_pair(outputSample(i, 0), weights[i]);
  std::sort(sortedValues.begin(), sortedValues.end());

  Point cumulativeWeights(sampleSize);
  Scalar cumulative = 0.0;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    cumulative += sortedValues[i].second;
    cumulativeWeights[i] = cumulative / sumWeights;
  }

  const auto weightedInverse = [&](const Scalar level) -> Scalar
  {
    if (level <= 0.0)
      return sortedValues[0].first;
    if (level >= 1.0)
      return sortedValues[sampleSize - 1].first;
    for (UnsignedInteger i = 0; i < sampleSize; ++i)
      if (cumulativeWeights[i] >= level)
        return sortedValues[i].first;
    return sortedValues[sampleSize - 1].first;
  };

  UnsignedInteger estimateIndex = sampleSize - 1;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    if (cumulativeWeights[i] >= alpha_)
    {
      estimateIndex = i;
      break;
    }
  }
  const Scalar estimate = sortedValues[estimateIndex].first;

  // The weighted empirical CDF is flat on ties: use the last index with
  // the same value to evaluate the variance at the estimate.
  UnsignedInteger lastTieIndex = estimateIndex;
  while ((lastTieIndex + 1 < sampleSize)
         && (sortedValues[lastTieIndex + 1].first == estimate))
    ++lastTieIndex;
  const Scalar empiricalCDF = cumulativeWeights[lastTieIndex];

  Scalar varianceNumerator = 0.0;
  for (UnsignedInteger i = 0; i < sampleSize; ++i)
  {
    const Scalar weight = sortedValues[i].second;
    const Scalar indicator = (i <= lastTieIndex) ? 1.0 : 0.0;
    const Scalar delta = indicator - empiricalCDF;
    varianceNumerator += weight * weight * delta * delta;
  }
  const Scalar variance = varianceNumerator / (sumWeights * sumWeights);
  const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta_));
  const Scalar halfWidth = z * std::sqrt(std::max(0.0, variance));
  const Scalar lowerLevel = std::min(1.0, std::max(0.0, alpha_ - halfWidth));
  const Scalar upperLevel = std::min(1.0, std::max(0.0, alpha_ + halfWidth));
  const Scalar lowerBound = weightedInverse(lowerLevel);
  const Scalar upperBound = weightedInverse(upperLevel);
  const Scalar standardError = (z > 0.0) ? 0.5 * (upperBound - lowerBound) / z : 0.0;
  const Scalar effectiveSampleSize = sumWeights * sumWeights / sumSquareWeights;

  const UnsignedInteger totalEvaluations = pilotSize_
      + crossEntropyResult.getOuterSampling() * crossEntropyResult.getBlockSize()
      + sampleSize;

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(Interval(lowerBound, upperBound));
  result.setEstimatorType(estimatorType_);
  result.setCoverageType(QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC);
  result.setRequiredCoverage(requiredCoverage_);
  result.setEffectiveSampleSize(effectiveSampleSize);
  result.setSampleSize(totalEvaluations);
  result.setIsFixedSample(false);
  result.setTailType(lowerTail ? QuantileSimulationResultImplementation::LOWER_TAIL
                     : QuantileSimulationResultImplementation::UPPER_TAIL);
  result.setThresholdSequence(engine.getThresholdPerStep());
  result.setStepCount(engine.getStepsNumber());
  result.setParticleCount(sampleSize);
  result.setAuxiliaryDistribution(auxiliaryDistribution);
  result.setIntermediateQuantileLevel(intermediateQuantileLevel_);
  result.setOuterSampling(crossEntropyResult.getOuterSampling());
  result.setBlockSize(crossEntropyResult.getBlockSize());
  return result;
}

/* String converter */
String CrossEntropyQuantile::__repr__() const
{
  OSS oss;
  oss << "class=" << CrossEntropyQuantile::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " intermediateQuantileLevel=" << intermediateQuantileLevel_
      << " pilotSize=" << pilotSize_
      << " minimumProposalStandardDeviation=" << minimumProposalStandardDeviation_
      << " estimator=" << estimatorType_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void CrossEntropyQuantile::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("intermediateQuantileLevel_", intermediateQuantileLevel_);
  adv.saveAttribute("pilotSize_", pilotSize_);
  adv.saveAttribute("minimumProposalStandardDeviation_", minimumProposalStandardDeviation_);
}

/* Method load() reloads the object from the StorageManager */
void CrossEntropyQuantile::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("intermediateQuantileLevel_", intermediateQuantileLevel_);
  adv.loadAttribute("pilotSize_", pilotSize_);
  adv.loadAttribute("minimumProposalStandardDeviation_", minimumProposalStandardDeviation_);
}

END_NAMESPACE_OPENTURNS
