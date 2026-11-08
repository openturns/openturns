//                                               -*- C++ -*-
/**
 *  @brief Block quantile simulation
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

#include "openturns/BlockQuantileSimulation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(BlockQuantileSimulation)

static const Factory<BlockQuantileSimulation> Factory_BlockQuantileSimulation;

/*
 * The block estimator is a batch-means estimator in the spirit of the
 * batching and sectioning constructions of:
 *
 * H. Dong and M. K. Nakayama.
 * Constructing confidence intervals for a quantile using batching and
 * sectioning when applying Latin hypercube sampling.
 * In Proceedings of the Winter Simulation Conference 2014, pages
 * 640-651, 2014.
 *
 * H. Dong and M. K. Nakayama.
 * Quantile estimation with Latin hypercube sampling.
 * Operations Research, 65(6):1678-1695, 2017.
 *
 * The sample is split into B independent blocks of length b.  One block
 * quantile is computed per block, and the block quantiles are combined
 * by a mean, a median or a trimmed mean.  The block length b is a
 * statistical parameter, distinct from SimulationAlgorithm::blockSize,
 * which only controls the evaluation batches.
 */

namespace
{

Scalar computeMean(const Point & values)
{
  Scalar mean = 0.0;
  const UnsignedInteger size = values.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
    mean += values[i];
  if (size > 0)
    mean /= 1.0 * size;
  return mean;
}

Scalar computeStandardDeviation(const Point & values)
{
  const UnsignedInteger size = values.getSize();
  if (size < 2)
    return 0.0;
  const Scalar mean = computeMean(values);
  Scalar sumSquare = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar delta = values[i] - mean;
    sumSquare += delta * delta;
  }
  return std::sqrt(std::max(0.0, sumSquare / (1.0 * (size - 1))));
}

Scalar percentile(const Point & sortedValues, const Scalar probability)
{
  const UnsignedInteger size = sortedValues.getSize();
  if (size == 0)
    return 0.0;
  if (size == 1)
    return sortedValues[0];
  const Scalar position = probability * (1.0 * (size - 1));
  const UnsignedInteger index = static_cast<UnsignedInteger>(std::floor(position));
  const Scalar fraction = position - 1.0 * index;
  if (index >= size - 1)
    return sortedValues[size - 1];
  return (1.0 - fraction) * sortedValues[index] + fraction * sortedValues[index + 1];
}

Scalar computeAsymptoticBlockLength(const UnsignedInteger sampleSize)
{
  // The mean of the block quantiles has variance of order 1/N and bias of
  // order 1/b.  The exponent 2/3 makes the bias o(N^(-1/2)), hence
  // negligible relative to the standard error.  The memory-optimal sqrt(N)
  // length only balances the two terms and does not support a coverage
  // statement.
  return std::ceil(std::pow(1.0 * sampleSize, 2.0 / 3.0));
}

String computeCombinationDescription(const QuantileSimulationResultImplementation::CombinationType type,
                                     const Scalar trimmingFraction)
{
  switch (type)
  {
    case QuantileSimulationResultImplementation::MEAN:
      return "Mean";
    case QuantileSimulationResultImplementation::MEDIAN:
      return "Median";
    case QuantileSimulationResultImplementation::TRIMMED_MEAN:
      return OSS() << "Trimmed mean (gamma=" << trimmingFraction << ")";
    default:
      break;
  }
  return "Unknown";
}

} // anonymous namespace

/* Default constructor */
BlockQuantileSimulation::BlockQuantileSimulation()
  : QuantileSimulationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
BlockQuantileSimulation::BlockQuantileSimulation(const RandomVector & output,
                                                 const Scalar alpha,
                                                 const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  // Nothing to do
}

/* Constructor with parameters and experiment */
BlockQuantileSimulation::BlockQuantileSimulation(const RandomVector & output,
                                                 const WeightedExperiment & experiment,
                                                 const Scalar alpha,
                                                 const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  setExperiment(experiment);
}

/* Virtual constructor */
BlockQuantileSimulation * BlockQuantileSimulation::clone() const
{
  return new BlockQuantileSimulation(*this);
}

/* Default block-length policy from the ResourceMap */
BlockQuantileSimulation::BlockLengthPolicy BlockQuantileSimulation::getDefaultBlockLengthPolicy()
{
  const String policy = ResourceMap::GetAsString("BlockQuantileSimulation-DefaultBlockLengthPolicy");
  if (policy == "FIXED")
    return FIXED;
  if (policy == "SQRT_N")
    return SQRT_N;
  if (policy == "ASYMPTOTIC")
    return ASYMPTOTIC;
  if (policy == "BIAS_TARGET")
    return BIAS_TARGET;
  if (policy == "MEMORY_BOUND")
    return MEMORY_BOUND;
  if (policy == "ADAPTIVE")
    return ADAPTIVE;
  throw InvalidArgumentException(HERE)
      << "Unknown block-length policy " << policy
      << " in the ResourceMap key BlockQuantileSimulation-DefaultBlockLengthPolicy";
}

/* Check the input parameters */
void BlockQuantileSimulation::checkInputs() const
{
  QuantileSimulationImplementation::checkInputs();
  if (!(trimmingFraction_ >= 0.0 && trimmingFraction_ < 0.5))
    throw InvalidArgumentException(HERE)
        << "The trimming fraction must be in [0, 0.5), but gamma = " << trimmingFraction_
        << " (ResourceMap key QuantileSimulation-DefaultTrimmingFraction)";
  if (minimumBlockCount_ < 2)
    throw InvalidArgumentException(HERE)
        << "The minimum block count must be at least 2, but minimumBlockCount = " << minimumBlockCount_
        << " (ResourceMap key QuantileSimulation-MinimumBlockCount)";
  if (!(biasTolerance_ > 0.0) || !std::isfinite(biasTolerance_))
    throw InvalidArgumentException(HERE)
        << "The bias tolerance must be finite and positive, but tolerance = " << biasTolerance_
        << " (ResourceMap key BlockQuantileSimulation-DefaultBiasTolerance)";
  if ((bootstrapSize_ > 0) && (bootstrapSize_ < 2))
    throw InvalidArgumentException(HERE)
        << "The bootstrap size must be 0 (disabled) or at least 2, but bootstrapSize = " << bootstrapSize_
        << " (ResourceMap key BlockQuantileSimulation-DefaultBootstrapSize)";
}

/* Experiment accessor */
void BlockQuantileSimulation::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  isExperimentProvided_ = true;
  sampleSize_ = experiment_.getSize();
}

WeightedExperiment BlockQuantileSimulation::getExperiment() const
{
  return experiment_;
}

Bool BlockQuantileSimulation::hasExperiment() const
{
  return isExperimentProvided_;
}

/* Sample size accessor */
void BlockQuantileSimulation::setSampleSize(const UnsignedInteger sampleSize)
{
  QuantileSimulationImplementation::setSampleSize(sampleSize);
  if (isExperimentProvided_)
    experiment_.setSize(sampleSize);
}

/* Statistical block length accessor */
void BlockQuantileSimulation::setBlockLength(const UnsignedInteger blockLength)
{
  if (blockLength == 0)
    throw InvalidArgumentException(HERE) << "The block length must be positive";
  blockLength_ = blockLength;
  // An explicit length selects the fixed-length policy.
  blockLengthPolicy_ = FIXED;
}

UnsignedInteger BlockQuantileSimulation::getBlockLength() const
{
  return blockLength_;
}

/* Block-length policy accessor */
void BlockQuantileSimulation::setBlockLengthPolicy(const BlockLengthPolicy policy)
{
  if ((policy != FIXED) && (policy != SQRT_N) && (policy != ASYMPTOTIC)
      && (policy != BIAS_TARGET) && (policy != MEMORY_BOUND) && (policy != ADAPTIVE))
    throw InvalidArgumentException(HERE) << "Unknown block-length policy";
  blockLengthPolicy_ = policy;
}

BlockQuantileSimulation::BlockLengthPolicy BlockQuantileSimulation::getBlockLengthPolicy() const
{
  return blockLengthPolicy_;
}

/* Combination rule accessor */
void BlockQuantileSimulation::setCombinationType(const QuantileSimulationResultImplementation::CombinationType type)
{
  if ((type != QuantileSimulationResultImplementation::MEAN)
      && (type != QuantileSimulationResultImplementation::MEDIAN)
      && (type != QuantileSimulationResultImplementation::TRIMMED_MEAN))
    throw InvalidArgumentException(HERE) << "Unknown combination type";
  combinationType_ = type;
}

QuantileSimulationResultImplementation::CombinationType BlockQuantileSimulation::getCombinationType() const
{
  return combinationType_;
}

/* Trimming fraction accessor */
void BlockQuantileSimulation::setTrimmingFraction(const Scalar gamma)
{
  if (!(gamma >= 0.0 && gamma < 0.5))
    throw InvalidArgumentException(HERE)
        << "The trimming fraction must be in [0, 0.5), but gamma = " << gamma;
  trimmingFraction_ = gamma;
}

Scalar BlockQuantileSimulation::getTrimmingFraction() const
{
  return trimmingFraction_;
}

/* Minimum block count accessor */
void BlockQuantileSimulation::setMinimumBlockCount(const UnsignedInteger minimumBlockCount)
{
  if (minimumBlockCount < 2)
    throw InvalidArgumentException(HERE)
        << "The minimum block count must be at least 2, but minimumBlockCount = " << minimumBlockCount;
  minimumBlockCount_ = minimumBlockCount;
}

UnsignedInteger BlockQuantileSimulation::getMinimumBlockCount() const
{
  return minimumBlockCount_;
}

/* Memory budget accessor */
void BlockQuantileSimulation::setMemoryBudget(const UnsignedInteger memoryBudget)
{
  if (memoryBudget == 0)
    throw InvalidArgumentException(HERE) << "The memory budget must be positive";
  memoryBudget_ = memoryBudget;
}

UnsignedInteger BlockQuantileSimulation::getMemoryBudget() const
{
  return memoryBudget_;
}

/* Bias tolerance accessor */
void BlockQuantileSimulation::setBiasTolerance(const Scalar tolerance)
{
  if (!(tolerance > 0.0) || !std::isfinite(tolerance))
    throw InvalidArgumentException(HERE)
        << "The bias tolerance must be finite and positive, but tolerance = " << tolerance;
  biasTolerance_ = tolerance;
}

Scalar BlockQuantileSimulation::getBiasTolerance() const
{
  return biasTolerance_;
}

/* Bootstrap size accessor */
void BlockQuantileSimulation::setBootstrapSize(const UnsignedInteger bootstrapSize)
{
  if ((bootstrapSize > 0) && (bootstrapSize < 2))
    throw InvalidArgumentException(HERE)
        << "The bootstrap size must be 0 (disabled) or at least 2, but bootstrapSize = " << bootstrapSize;
  bootstrapSize_ = bootstrapSize;
}

UnsignedInteger BlockQuantileSimulation::getBootstrapSize() const
{
  return bootstrapSize_;
}

/* Resolve the experiment, building a Monte Carlo experiment when needed */
WeightedExperiment BlockQuantileSimulation::resolveExperiment() const
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

/* Draw a block from the experiment */
Sample BlockQuantileSimulation::drawBlock(WeightedExperiment & experiment,
                                          const UnsignedInteger blockLength) const
{
  experiment.setSize(blockLength);
  Point weights;
  const Sample inputSample(experiment.generateWithWeights(weights));
  if (inputSample.getSize() != blockLength)
    throw InternalException(HERE)
        << "The experiment returned " << inputSample.getSize()
        << " points instead of the requested block length " << blockLength;
  const Sample outputSample(output_.getFrozenSample(inputSample));
  if (outputSample.getDimension() != 1)
    throw InvalidArgumentException(HERE)
        << "The block estimator requires a scalar output, but the output dimension is "
        << outputSample.getDimension();
  return outputSample;
}

/* Block quantile from one block */
Scalar BlockQuantileSimulation::blockQuantile(const Sample & block) const
{
  const UnsignedInteger size = block.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The block must not be empty";
  Point values(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    values[i] = block(i, 0);
  std::sort(values.begin(), values.end());

  if (estimatorType_ == QuantileSimulationResultImplementation::EMPIRICAL_INVERSE)
  {
    UnsignedInteger index = static_cast<UnsignedInteger>(std::ceil(1.0 * size * alpha_));
    if (index > 0)
      --index;
    if (index >= size)
      index = size - 1;
    return values[index];
  }

  // Type-5 interpolation, consistent with Sample::computeQuantile on
  // uniform weights.
  if (size == 1)
    return values[0];
  const Scalar scalarIndex = alpha_ * (1.0 * size) - 0.5;
  if (scalarIndex <= 0.0)
    return values[0];
  if (scalarIndex >= 1.0 * (size - 1))
    return values[size - 1];
  const UnsignedInteger index = static_cast<UnsignedInteger>(std::floor(scalarIndex));
  const Scalar fraction = scalarIndex - 1.0 * index;
  return (1.0 - fraction) * values[index] + fraction * values[index + 1];
}

/* Combine block quantiles */
Scalar BlockQuantileSimulation::combine(const Point & blockQuantiles) const
{
  const UnsignedInteger size = blockQuantiles.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The block quantile sample must not be empty";

  if (combinationType_ == QuantileSimulationResultImplementation::MEAN)
    return computeMean(blockQuantiles);

  Point sortedValues(blockQuantiles);
  std::sort(sortedValues.begin(), sortedValues.end());

  if (combinationType_ == QuantileSimulationResultImplementation::MEDIAN)
  {
    if (size % 2 == 1)
      return sortedValues[size / 2];
    return 0.5 * (sortedValues[size / 2 - 1] + sortedValues[size / 2]);
  }

  const UnsignedInteger trimmed = static_cast<UnsignedInteger>(std::floor(trimmingFraction_ * (1.0 * size)));
  const UnsignedInteger retained = size - 2 * trimmed;
  if (retained == 0)
    throw InvalidArgumentException(HERE)
        << "The trimming fraction " << trimmingFraction_
        << " leaves no block quantile for combination";
  Scalar mean = 0.0;
  for (UnsignedInteger i = trimmed; i < size - trimmed; ++i)
    mean += sortedValues[i];
  return mean / (1.0 * retained);
}

/* Estimate the bias constant from a pilot sample */
Scalar BlockQuantileSimulation::estimateBiasConstant(const UnsignedInteger sampleSize,
                                                     WeightedExperiment & experiment,
                                                     UnsignedInteger & pilotBlockLength,
                                                     Scalar & pilotDeviation,
                                                     UnsignedInteger & pilotEvaluationCount) const
{
  pilotBlockLength = 0;
  pilotDeviation = 0.0;
  pilotEvaluationCount = 0;
  const Scalar nan = std::numeric_limits<Scalar>::quiet_NaN();

  // A small pilot run is enough to estimate the O(1/b) bias constant.
  // It is capped so that very large sample sizes do not pay a large
  // pilot overhead.
  const UnsignedInteger minimumPilotSize = ResourceMap::GetAsUnsignedInteger("BlockQuantileSimulation-PilotMinimumSize");
  const UnsignedInteger maximumPilotSize = ResourceMap::GetAsUnsignedInteger("BlockQuantileSimulation-PilotMaximumSize");
  const Scalar pilotSizeFraction = ResourceMap::GetAsScalar("BlockQuantileSimulation-PilotSizeFraction");
  if (minimumPilotSize == 0)
    throw InvalidArgumentException(HERE)
        << "The ResourceMap key BlockQuantileSimulation-PilotMinimumSize must be positive";
  if (maximumPilotSize < minimumPilotSize)
    throw InvalidArgumentException(HERE)
        << "The ResourceMap key BlockQuantileSimulation-PilotMaximumSize ("
        << maximumPilotSize << ") must not be smaller than BlockQuantileSimulation-PilotMinimumSize ("
        << minimumPilotSize << ")";
  if (!(pilotSizeFraction >= 0.0) || !std::isfinite(pilotSizeFraction) || (pilotSizeFraction > 1.0))
    throw InvalidArgumentException(HERE)
        << "The ResourceMap key BlockQuantileSimulation-PilotSizeFraction must be in [0, 1], but its value is "
        << pilotSizeFraction;
  const UnsignedInteger scaledPilotSize = static_cast<UnsignedInteger>(std::floor(pilotSizeFraction * (1.0 * sampleSize)));
  const UnsignedInteger pilotSize = std::min<UnsignedInteger>(sampleSize,
      std::max<UnsignedInteger>(minimumPilotSize, std::min<UnsignedInteger>(scaledPilotSize, maximumPilotSize)));
  const UnsignedInteger firstHalfSize = pilotSize / 2;
  const UnsignedInteger secondHalfSize = pilotSize - firstHalfSize;
  // Each half must contain at least two blocks of the smallest admissible
  // pilot length 2, hence at least four observations.
  if ((firstHalfSize < 4) || (secondHalfSize < 4))
    return nan;

  // The smallest pilot block length is 2 so that b2 = 2 b1 and both pilot
  // runs contain at least two blocks.
  const UnsignedInteger firstBlockLength = std::max<UnsignedInteger>(2,
      static_cast<UnsignedInteger>(std::sqrt(1.0 * firstHalfSize)));
  const UnsignedInteger secondBlockLength = 2 * firstBlockLength;
  const UnsignedInteger firstBlockCount = firstHalfSize / firstBlockLength;
  const UnsignedInteger secondBlockCount = secondHalfSize / secondBlockLength;
  if ((firstBlockCount < 2) || (secondBlockCount < 2))
    return nan;

  // The two pilot runs use independent streams: the first half of the
  // pilot budget is used at b1, the second half at b2 = 2 b1.  These
  // evaluations are reported in the result in addition to the main run.
  pilotEvaluationCount = firstBlockCount * firstBlockLength + secondBlockCount * secondBlockLength;
  Point firstQuantiles(firstBlockCount);
  for (UnsignedInteger i = 0; i < firstBlockCount; ++i)
  {
    const Sample block(drawBlock(experiment, firstBlockLength));
    firstQuantiles[i] = blockQuantile(block);
  }
  Point secondQuantiles(secondBlockCount);
  for (UnsignedInteger i = 0; i < secondBlockCount; ++i)
  {
    const Sample block(drawBlock(experiment, secondBlockLength));
    secondQuantiles[i] = blockQuantile(block);
  }

  pilotBlockLength = firstBlockLength;
  pilotDeviation = computeStandardDeviation(firstQuantiles);
  if (!(pilotDeviation > 0.0) || !std::isfinite(pilotDeviation))
    return nan;

  const Scalar firstMean = computeMean(firstQuantiles);
  const Scalar secondMean = computeMean(secondQuantiles);
  const Scalar denominator = 1.0 / (1.0 * firstBlockLength) - 1.0 / (1.0 * secondBlockLength);
  if (!(denominator > 0.0))
    return nan;
  const Scalar biasConstant = (firstMean - secondMean) / denominator;
  if (!std::isfinite(biasConstant))
    return nan;
  return biasConstant;
}

/* Compute the block length for a given policy */
UnsignedInteger BlockQuantileSimulation::computeBlockLength(const UnsignedInteger sampleSize,
                                                            WeightedExperiment & experiment,
                                                            Scalar & biasEstimate,
                                                            UnsignedInteger & pilotEvaluationCount) const
{
  biasEstimate = std::numeric_limits<Scalar>::quiet_NaN();
  pilotEvaluationCount = 0;
  const Scalar sqrtN = std::sqrt(1.0 * sampleSize);
  const Scalar asymptoticLength = computeAsymptoticBlockLength(sampleSize);
  UnsignedInteger blockLength = 0;

  switch (blockLengthPolicy_)
  {
    case FIXED:
      if (blockLength_ == 0)
        throw InvalidArgumentException(HERE)
            << "The FIXED block-length policy requires a positive block length";
      blockLength = blockLength_;
      break;
    case SQRT_N:
      blockLength = std::max<UnsignedInteger>(1, static_cast<UnsignedInteger>(std::ceil(sqrtN)));
      break;
    case ASYMPTOTIC:
      blockLength = std::max<UnsignedInteger>(1, static_cast<UnsignedInteger>(asymptoticLength));
      break;
    case MEMORY_BOUND:
    {
      if (memoryBudget_ == 0)
        throw InvalidArgumentException(HERE)
            << "The MEMORY_BOUND block-length policy requires a positive memory budget";
      const Scalar budget = 1.0 * memoryBudget_;
      const Scalar discriminant = budget * budget - 4.0 * (1.0 * sampleSize);
      if (!(discriminant >= 0.0))
        throw InvalidArgumentException(HERE)
            << "The memory budget (" << memoryBudget_ << " values) is too small for a sample size of "
            << sampleSize << ": at least 2*sqrt(sampleSize) = " << 2.0 * sqrtN
            << " values are required";
      Scalar candidate = 0.5 * (budget + std::sqrt(discriminant));
      blockLength = static_cast<UnsignedInteger>(std::floor(candidate));
      while ((blockLength > 1) && (1.0 * blockLength + (1.0 * sampleSize) / (1.0 * blockLength) > budget))
        --blockLength;
      // Keep at least the minimum number of blocks requested by the user.
      const UnsignedInteger blockCountFloor = std::max<UnsignedInteger>(2, minimumBlockCount_);
      const UnsignedInteger maximumBlockLength = std::max<UnsignedInteger>(1, sampleSize / blockCountFloor);
      if (blockLength > maximumBlockLength)
        blockLength = maximumBlockLength;
      break;
    }
    case BIAS_TARGET:
    case ADAPTIVE:
    {
      const Scalar tolerance = (blockLengthPolicy_ == BIAS_TARGET)
                               ? biasTolerance_
                               : ResourceMap::GetAsScalar("BlockQuantileSimulation-DefaultAdaptiveTolerance");
      if (!(tolerance > 0.0) || !std::isfinite(tolerance))
        throw InvalidArgumentException(HERE)
            << "The bias tolerance must be finite and positive, but tolerance = " << tolerance;
      UnsignedInteger pilotBlockLength = 0;
      Scalar pilotDeviation = 0.0;
      const Scalar biasConstant = estimateBiasConstant(sampleSize, experiment, pilotBlockLength, pilotDeviation, pilotEvaluationCount);
      if (std::isfinite(biasConstant) && (pilotDeviation > 0.0) && (pilotBlockLength > 0))
      {
        // sqrt(pi/2) is the asymptotic ratio between the standard error of
        // the median and that of the mean for normal block quantiles.
        const Scalar efficiencyFactor = (combinationType_ == QuantileSimulationResultImplementation::MEDIAN)
                                        ? std::sqrt(0.5 * M_PI) : 1.0;
        // The standard error of the combined estimator is
        // approximately independent of b to first order.
        const Scalar standardError = efficiencyFactor * pilotDeviation
                                     * std::sqrt((1.0 * pilotBlockLength) / (1.0 * sampleSize));
        if (standardError > 0.0)
        {
          const Scalar target = std::ceil(std::abs(biasConstant) / (tolerance * standardError));
          blockLength = std::max<UnsignedInteger>(1, static_cast<UnsignedInteger>(target));
        }
      }
      if (blockLength == 0)
        blockLength = std::max<UnsignedInteger>(1, static_cast<UnsignedInteger>(asymptoticLength));
      // Below sqrt(N) the memory grows and the interval is not valid.
      const UnsignedInteger minimumBlockLength = std::max<UnsignedInteger>(1, static_cast<UnsignedInteger>(std::ceil(sqrtN)));
      if (blockLength < minimumBlockLength)
        blockLength = minimumBlockLength;
      // Keep at least two blocks, as required by the t-interval.
      const UnsignedInteger blockCountFloor = std::max<UnsignedInteger>(2, minimumBlockCount_);
      const UnsignedInteger maximumBlockLength = std::max<UnsignedInteger>(1, sampleSize / blockCountFloor);
      if (blockLength > maximumBlockLength)
        blockLength = maximumBlockLength;
      if (std::isfinite(biasConstant))
        biasEstimate = biasConstant / (1.0 * blockLength);
      break;
    }
    default:
      throw InvalidArgumentException(HERE) << "Unknown block-length policy";
  }

  if (blockLength == 0)
    blockLength = 1;
  if (blockLength > sampleSize)
    blockLength = sampleSize;
  if (sampleSize / blockLength < 2)
    throw InvalidArgumentException(HERE)
        << "The block length " << blockLength << " leaves fewer than two blocks for a sample size of "
        << sampleSize << ". Reduce the block length or increase the sample size.";
  return blockLength;
}

/* Description of the block-length policy */
String BlockQuantileSimulation::getBlockLengthPolicyDescription() const
{
  switch (blockLengthPolicy_)
  {
    case FIXED:
      return OSS() << "Fixed (blockLength=" << blockLength_ << ")";
    case SQRT_N:
      return OSS() << "Square root (blockLength=" << lastBlockLength_ << ")";
    case ASYMPTOTIC:
      return OSS() << "Asymptotic (blockLength=" << lastBlockLength_ << ")";
    case BIAS_TARGET:
      return OSS() << "Bias target (blockLength=" << lastBlockLength_
                   << ", tolerance=" << biasTolerance_ << ")";
    case MEMORY_BOUND:
      return OSS() << "Memory bound (blockLength=" << lastBlockLength_
                   << ", budget=" << memoryBudget_ << ")";
    case ADAPTIVE:
      return OSS() << "Adaptive (blockLength=" << lastBlockLength_
                   << ", tolerance=" << ResourceMap::GetAsScalar("BlockQuantileSimulation-DefaultAdaptiveTolerance") << ")";
    default:
      break;
  }
  return "Unknown";
}

/* Bootstrap percentile interval */
Interval BlockQuantileSimulation::bootstrapInterval(const Point & blockQuantiles,
                                                    Scalar & estimate,
                                                    Scalar & standardError) const
{
  const UnsignedInteger blockCount = blockQuantiles.getSize();
  if (blockCount < 2)
    throw InvalidArgumentException(HERE)
        << "The bootstrap interval requires at least two block quantiles, but blockCount = " << blockCount;
  if (bootstrapSize_ < 2)
    throw InvalidArgumentException(HERE)
        << "The bootstrap size must be at least 2, but bootstrapSize = " << bootstrapSize_;

  Point bootstrapEstimates(bootstrapSize_);
  Point resampled(blockCount);
  for (UnsignedInteger i = 0; i < bootstrapSize_; ++i)
  {
    for (UnsignedInteger j = 0; j < blockCount; ++j)
      resampled[j] = blockQuantiles[RandomGenerator::IntegerGenerate(blockCount)];
    bootstrapEstimates[i] = combine(resampled);
  }

  const Scalar originalEstimate = combine(blockQuantiles);
  standardError = computeStandardDeviation(bootstrapEstimates);

  Point sortedEstimates(bootstrapEstimates);
  std::sort(sortedEstimates.begin(), sortedEstimates.end());
  Scalar lower = percentile(sortedEstimates, 0.5 * (1.0 - beta_));
  Scalar upper = percentile(sortedEstimates, 0.5 * (1.0 + beta_));
  lower = std::min(lower, originalEstimate);
  upper = std::max(upper, originalEstimate);
  estimate = originalEstimate;
  return Interval(lower, upper);
}

/* Run a fixed-sample experiment */
QuantileSimulationResult BlockQuantileSimulation::runFixedSample()
{
  if (sampleSize_ == 0)
    throw InvalidArgumentException(HERE) << "The sample size must be set before running the algorithm";

  const UnsignedInteger requestedSize = sampleSize_;
  WeightedExperiment experiment(resolveExperiment());
  if (!experiment.isIID())
    throw InvalidArgumentException(HERE)
        << "The block estimator requires an i.i.d. experiment, but "
        << experiment.getClassName() << " is not i.i.d. "
        << "Use a MonteCarloExperiment, or another estimator for stratified designs.";
  if (!experiment.hasUniformWeights())
    throw InvalidArgumentException(HERE)
        << "The block estimator requires uniform weights, but "
        << experiment.getClassName() << " has non-uniform weights";

  UnsignedInteger inputDimension = 0;
  try
  {
    inputDimension = output_.isComposite()
                     ? output_.getAntecedent().getDimension()
                     : output_.getDimension();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE) << "The input dimension of the output random vector cannot be retrieved";
  }
  if (experiment.getDistribution().getDimension() != inputDimension)
    throw InvalidArgumentException(HERE)
        << "The experiment distribution dimension (" << experiment.getDistribution().getDimension()
        << ") does not match the input dimension (" << inputDimension << ")";

  Scalar biasEstimate = std::numeric_limits<Scalar>::quiet_NaN();
  UnsignedInteger pilotEvaluationCount = 0;
  const UnsignedInteger blockLength = computeBlockLength(requestedSize, experiment, biasEstimate, pilotEvaluationCount);
  lastBlockLength_ = blockLength;
  const UnsignedInteger blockCount = requestedSize / blockLength;
  const UnsignedInteger totalSize = blockCount * blockLength;

  Point blockQuantiles(blockCount);
  Sample history(blockCount, 4);
  Scalar runningMean = 0.0;
  Scalar runningSumSquare = 0.0;
  for (UnsignedInteger i = 0; i < blockCount; ++i)
  {
    const Sample block(drawBlock(experiment, blockLength));
    blockQuantiles[i] = blockQuantile(block);
    const Scalar count = 1.0 * (i + 1);
    const Scalar delta = blockQuantiles[i] - runningMean;
    runningMean += delta / count;
    runningSumSquare += delta * (blockQuantiles[i] - runningMean);
    history(i, 0) = 1.0 * (i + 1) * blockLength;
    history(i, 1) = runningMean;
    if (i == 0)
    {
      history(i, 2) = blockQuantiles[i];
      history(i, 3) = blockQuantiles[i];
    }
    else
    {
      const Scalar variance = runningSumSquare / (1.0 * i);
      const Scalar standardError = std::sqrt(std::max(0.0, variance) / count);
      const Scalar t = DistFunc::qStudent(1.0 * i, 0.5 * (1.0 + beta_));
      history(i, 2) = runningMean - t * standardError;
      history(i, 3) = runningMean + t * standardError;
    }
  }

  Scalar estimate = combine(blockQuantiles);
  Scalar standardError = 0.0;
  Interval interval;
  if (bootstrapSize_ > 0)
  {
    interval = bootstrapInterval(blockQuantiles, estimate, standardError);
  }
  else
  {
    standardError = computeStandardDeviation(blockQuantiles) / std::sqrt(1.0 * blockCount);
    if (combinationType_ == QuantileSimulationResultImplementation::MEDIAN)
      standardError *= std::sqrt(0.5 * M_PI);
    else if (combinationType_ == QuantileSimulationResultImplementation::TRIMMED_MEAN)
    {
      const UnsignedInteger trimmed = static_cast<UnsignedInteger>(std::floor(trimmingFraction_ * (1.0 * blockCount)));
      Point sortedValues(blockQuantiles);
      std::sort(sortedValues.begin(), sortedValues.end());
      // Tukey, J. W. and McLaughlin, D. H., Less vulnerable confidence and
      // significance procedures for location based on a single sample:
      // trimming/winsorization 1, Sankhya A, 25(3):331-352, 1963: the
      // standard error of the trimmed mean is the standard deviation of the
      // winsorized block quantiles divided by the retained fraction and the
      // square root of the full block count.
      Point winsorized(sortedValues);
      for (UnsignedInteger i = 0; i < trimmed; ++i)
      {
        winsorized[i] = sortedValues[trimmed];
        winsorized[blockCount - 1 - i] = sortedValues[blockCount - 1 - trimmed];
      }
      const Scalar retainedFraction = 1.0 * (blockCount - 2 * trimmed) / (1.0 * blockCount);
      standardError = computeStandardDeviation(winsorized) / (retainedFraction * std::sqrt(1.0 * blockCount));
    }
    const Scalar t = DistFunc::qStudent(1.0 * (blockCount - 1), 0.5 * (1.0 + beta_));
    interval = Interval(estimate - t * standardError, estimate + t * standardError);
  }

  // Coverage label.  The block interval is asymptotic in B and requires
  // b to dominate sqrt(N).  SQRT_N is always reported without coverage.
  QuantileSimulationResultImplementation::CoverageType coverageType
    = QuantileSimulationResultImplementation::COVERAGE_NONE;
  const Scalar sqrtSize = std::sqrt(1.0 * totalSize);
  const Scalar asymptoticLength = computeAsymptoticBlockLength(totalSize);
  if ((blockLengthPolicy_ != SQRT_N)
      && (1.0 * blockLength > sqrtSize)
      && ((blockLengthPolicy_ != ASYMPTOTIC) || (1.0 * blockLength >= asymptoticLength)))
    coverageType = QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC;

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(estimatorType_);
  result.setCoverageType(coverageType);
  result.setRequiredCoverage(requiredCoverage_);
  result.setEffectiveSampleSize(1.0 * totalSize);
  result.setSampleSize(totalSize + pilotEvaluationCount);
  result.setBlockLength(blockLength);
  result.setBlockCount(blockCount);
  result.setBlockQuantileSample(blockQuantiles);
  result.setCombinationType(combinationType_);
  result.setTrimmingFraction(trimmingFraction_);
  result.setBlockLengthPolicyDescription(getBlockLengthPolicyDescription());
  result.setPeakMemory(blockLength + blockCount);
  if (std::isfinite(biasEstimate))
    result.setBiasEstimate(biasEstimate);
  result.setIsFixedSample(true);
  result.setOuterSampling(blockCount);
  result.setBlockSize(getBlockSize());
  result.setConvergenceHistory(history);
  return result;
}

/* String converter */
String BlockQuantileSimulation::__repr__() const
{
  OSS oss;
  oss << "class=" << BlockQuantileSimulation::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " estimator=" << estimatorType_
      << " requiredCoverage=" << requiredCoverage_
      << " blockLengthPolicy=" << blockLengthPolicy_ << " (" << getBlockLengthPolicyDescription() << ")"
      << " combination=" << computeCombinationDescription(combinationType_, trimmingFraction_)
      << " minimumBlockCount=" << minimumBlockCount_
      << " memoryBudget=" << memoryBudget_
      << " biasTolerance=" << biasTolerance_
      << " bootstrapSize=" << bootstrapSize_
      << " experimentProvided=" << isExperimentProvided_;
  if (isExperimentProvided_)
    oss << " experiment=" << experiment_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void BlockQuantileSimulation::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("isExperimentProvided_", isExperimentProvided_);
  adv.saveAttribute("blockLength_", blockLength_);
  adv.saveAttribute("lastBlockLength_", lastBlockLength_);
  adv.saveAttribute("blockLengthPolicy_", static_cast<UnsignedInteger>(blockLengthPolicy_));
  adv.saveAttribute("combinationType_", static_cast<UnsignedInteger>(combinationType_));
  adv.saveAttribute("trimmingFraction_", trimmingFraction_);
  adv.saveAttribute("minimumBlockCount_", minimumBlockCount_);
  adv.saveAttribute("memoryBudget_", memoryBudget_);
  adv.saveAttribute("biasTolerance_", biasTolerance_);
  adv.saveAttribute("bootstrapSize_", bootstrapSize_);
}

/* Method load() reloads the object through the StorageManager */
void BlockQuantileSimulation::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("isExperimentProvided_", isExperimentProvided_);
  adv.loadAttribute("blockLength_", blockLength_);
  adv.loadAttribute("lastBlockLength_", lastBlockLength_);
  UnsignedInteger blockLengthPolicy = static_cast<UnsignedInteger>(blockLengthPolicy_);
  adv.loadAttribute("blockLengthPolicy_", blockLengthPolicy);
  blockLengthPolicy_ = static_cast<BlockLengthPolicy>(blockLengthPolicy);
  UnsignedInteger combinationType = static_cast<UnsignedInteger>(combinationType_);
  adv.loadAttribute("combinationType_", combinationType);
  combinationType_ = static_cast<QuantileSimulationResultImplementation::CombinationType>(combinationType);
  adv.loadAttribute("trimmingFraction_", trimmingFraction_);
  adv.loadAttribute("minimumBlockCount_", minimumBlockCount_);
  adv.loadAttribute("memoryBudget_", memoryBudget_);
  adv.loadAttribute("biasTolerance_", biasTolerance_);
  adv.loadAttribute("bootstrapSize_", bootstrapSize_);
}

END_NAMESPACE_OPENTURNS
