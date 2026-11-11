//                                               -*- C++ -*-
/**
 *  @brief Quantile simulation driven by a weighted experiment
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

#include "openturns/QuantileSimulationAlgorithm.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/QuantileConfidence.hxx"
#include "openturns/ImportanceSamplingExperiment.hxx"
#include "openturns/LHSExperiment.hxx"
#include "openturns/SobolIndicesExperiment.hxx"
#include "openturns/MonteCarloExperiment.hxx"
#include "openturns/TensorProductExperiment.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuantileSimulationAlgorithm)

static const Factory<QuantileSimulationAlgorithm> Factory_QuantileSimulationAlgorithm;

namespace
{

/*
 * The helpers below implement standard quantile estimators.  The exact
 * interval follows Wilks, S. S., Order statistics, Bulletin of the
 * American Mathematical Society, 54(1):6-50, 1948: for B ~ Binomial(N,
 * alpha), the coverage of [Y_(r1), Y_(r2)] is P(r1 <= B <= r2) and
 * depends only on N, alpha and the two ranks.  The Woodruff interval
 * follows Woodruff, R. S., Confidence intervals for medians and other
 * position measures, Journal of the American Statistical Association,
 * 47(260):635-646, 1952: it inverts the empirical CDF at the levels
 * alpha - z*sqrt(V) and alpha + z*sqrt(V), where V is the variance of
 * the empirical CDF at the estimate.  The interpolated estimator is the
 * type-5 rule of Hyndman, R. J. and Fan, Y., Sample quantiles in
 * statistical packages, The American Statistician, 50(4):361-365, 1996:
 * it interpolates adjacent order statistics at index N*alpha + 1/2 and
 * matches Sample::computeQuantile on uniform weights; on non-uniform
 * weights it interpolates linearly between cumulative weights.  The
 * effective sample size is the Kish effective sample size, Kish, L.,
 * Survey Sampling, Wiley, 1965.
 */

/** Sorted sample and self-normalized weights, with cumulative weights. */
struct WeightedSample
{
  Point values;
  Point weights;
  Point cumulative;
  Scalar squareSum = 0.0;
  Bool uniform = false;
};

WeightedSample prepareWeightedSample(const Sample & sample,
                                     const Point & weightsIn)
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The sample must not be empty";
  if (sample.getDimension() != 1)
    throw InvalidArgumentException(HERE)
        << "The output sample must be one-dimensional, but its dimension is "
        << sample.getDimension();
  if (weightsIn.getSize() != size)
    throw InvalidArgumentException(HERE)
        << "The weight vector size (" << weightsIn.getSize()
        << ") does not match the sample size (" << size << ")";

  Scalar weightSum = 0.0;
  Bool uniform = true;
  const Scalar firstWeight = weightsIn[0];
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar weight = weightsIn[i];
    if (!(weight >= 0.0) || !std::isfinite(weight))
      throw InvalidArgumentException(HERE)
          << "The weights must be finite and non-negative, but weight[" << i << "] = " << weight;
    if (weight != firstWeight)
      uniform = false;
    weightSum += weight;
  }
  if (!std::isfinite(weightSum) || !(weightSum > 0.0))
    throw InvalidArgumentException(HERE) << "The sum of the weights must be finite and positive";

  // Reject NaN outputs before sorting: comparisons with NaN do not define
  // a strict weak ordering, so the sorted order (and the estimate) would be
  // implementation-dependent.
  for (UnsignedInteger i = 0; i < size; ++i)
    if (std::isnan(sample(i, 0)))
      throw InvalidArgumentException(HERE)
          << "The output sample contains a NaN value at index " << i;

  const Indices order(sample.argsort());
  WeightedSample result;
  result.values = Point(size);
  result.weights = Point(size);
  result.cumulative = Point(size);
  result.uniform = uniform;
  for (UnsignedInteger i = 0; i < size; ++i)
    result.values[i] = sample(order[i], 0);
  Scalar cumulative = 0.0;
  Scalar squareSum = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar weight = weightsIn[order[i]] / weightSum;
    result.weights[i] = weight;
    cumulative += weight;
    result.cumulative[i] = cumulative;
    squareSum += weight * weight;
  }
  // The last cumulative weight is exactly one up to round-off.
  result.cumulative[size - 1] = 1.0;
  result.squareSum = squareSum;
  return result;
}

/** Lower weighted empirical inverse: first cumulative weight >= p. */
Scalar weightedQuantile(const WeightedSample & data, const Scalar p)
{
  const UnsignedInteger size = data.values.getSize();
  // Zero-weight observations carry no probability mass and must not
  // extend the estimate beyond the support of the weighted sample.
  if (p <= 0.0)
  {
    UnsignedInteger k = 0;
    while ((k < size) && !(data.weights[k] > 0.0))
      ++k;
    if (k >= size)
      k = size - 1;
    return data.values[k];
  }
  if (p >= 1.0)
  {
    UnsignedInteger k = size;
    while ((k > 0) && !(data.weights[k - 1] > 0.0))
      --k;
    if (k == 0)
      k = 1;
    return data.values[k - 1];
  }
  UnsignedInteger k = 0;
  while ((k < size) && (data.cumulative[k] < p))
    ++k;
  if (k >= size)
    k = size - 1;
  return data.values[k];
}

/** Reject a resize of an experiment whose size is derived from its marginals. */
void checkResizableExperiment(const WeightedExperiment & experiment,
                              const UnsignedInteger requestedSize)
{
  if ((requestedSize == 0)
      || (experiment.getSize() == requestedSize)
      || (dynamic_cast<const TensorProductExperiment *>(experiment.getImplementation().get()) == nullptr))
    return;
  throw InvalidArgumentException(HERE)
      << "The size of a TensorProductExperiment cannot be changed from "
      << experiment.getSize() << " to " << requestedSize
      << " because it is the product of its marginal designs. "
      << "Set the size of each marginal experiment instead.";
}

/** Weighted empirical CDF at a given value. */
Scalar weightedCDF(const WeightedSample & data, const Scalar value)
{
  Scalar cdf = 0.0;
  const UnsignedInteger size = data.values.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (data.values[i] <= value)
      cdf += data.weights[i];
    else
      break;
  }
  return cdf;
}

/** Asymptotic variance of the weighted empirical CDF at a given value. */
Scalar weightedVariance(const WeightedSample & data, const Scalar value)
{
  const Scalar cdf = weightedCDF(data, value);
  Scalar lowSquareSum = 0.0;
  Scalar highSquareSum = 0.0;
  const UnsignedInteger size = data.values.getSize();
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    if (data.values[i] <= value)
      lowSquareSum += data.weights[i] * data.weights[i];
    else
      highSquareSum += data.weights[i] * data.weights[i];
  }
  return (1.0 - cdf) * (1.0 - cdf) * lowSquareSum + cdf * cdf * highSquareSum;
}

/** Interpolated quantile.  Uniform weights use the type-5 convention. */
Scalar interpolatedQuantile(const WeightedSample & data, const Scalar alpha)
{
  const UnsignedInteger size = data.values.getSize();
  if (size == 1)
    return data.values[0];
  if (data.uniform)
  {
    const Scalar scalarIndex = alpha * size - 0.5;
    if (scalarIndex >= size - 1)
      return data.values[size - 1];
    if (scalarIndex <= 0.0)
      return data.values[0];
    const UnsignedInteger index = static_cast<UnsignedInteger>(std::floor(scalarIndex));
    const Scalar beta = scalarIndex - index;
    return (1.0 - beta) * data.values[index] + beta * data.values[index + 1];
  }
  // Zero-weight observations carry no probability mass: they must not
  // contribute to the interpolation, otherwise a single weighted point
  // surrounded by zero-weight points would be moved away from its value.
  UnsignedInteger first = 0;
  while ((first < size) && !(data.weights[first] > 0.0))
    ++first;
  if (first == size)
    return data.values[size - 1];
  if (alpha <= data.cumulative[first])
    return data.values[first];
  UnsignedInteger lower = first;
  for (UnsignedInteger i = first + 1; i < size; ++i)
  {
    if (!(data.weights[i] > 0.0))
      continue;
    if (data.cumulative[i] < alpha)
    {
      lower = i;
      continue;
    }
    const Scalar lowerWeight = data.cumulative[lower];
    const Scalar upperWeight = data.cumulative[i];
    const Scalar fraction = (alpha - lowerWeight) / (upperWeight - lowerWeight);
    return (1.0 - fraction) * data.values[lower] + fraction * data.values[i];
  }
  return data.values[lower];
}

/** Whether the experiment reuses a Latin hypercube stratum permutation across draws. */
Bool reusesLHSShuffle(const WeightedExperiment & experiment)
{
  const LHSExperiment * lhs = dynamic_cast<const LHSExperiment *>(experiment.getImplementation().get());
  if (lhs != nullptr)
    return !lhs->getAlwaysShuffle();
  // A Sobol wrapper expands the underlying design deterministically.  A
  // Latin hypercube with a fixed random shift then produces the same points
  // in every replicate, even when the stratum permutation is redrawn.
  const SobolIndicesExperiment * sobol = dynamic_cast<const SobolIndicesExperiment *>(experiment.getImplementation().get());
  if (sobol != nullptr)
  {
    const WeightedExperiment inner(sobol->getWeightedExperiment());
    const LHSExperiment * innerLHS = dynamic_cast<const LHSExperiment *>(inner.getImplementation().get());
    if ((innerLHS != nullptr) && !innerLHS->getRandomShift())
      return true;
    return reusesLHSShuffle(inner);
  }
  const TensorProductExperiment * tensor = dynamic_cast<const TensorProductExperiment *>(experiment.getImplementation().get());
  if (tensor != nullptr)
  {
    const TensorProductExperiment::WeightedExperimentCollection collection(tensor->getWeightedExperimentCollection());
    for (UnsignedInteger i = 0; i < collection.getSize(); ++i)
      if (reusesLHSShuffle(collection[i]))
        return true;
  }
  return false;
}

/** Woodruff confidence interval for the weighted quantile. */
Interval woodruffInterval(const WeightedSample & data,
                          const Scalar alpha,
                          const Scalar beta,
                          const Scalar estimate,
                          Scalar & standardError)
{
  const Scalar variance = std::max(0.0, weightedVariance(data, estimate));
  const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta));
  const Scalar halfWidth = z * std::sqrt(variance);
  Scalar lower = weightedQuantile(data, alpha - halfWidth);
  Scalar upper = weightedQuantile(data, alpha + halfWidth);
  lower = std::min(lower, estimate);
  upper = std::max(upper, estimate);
  standardError = (upper - lower) / (2.0 * z);
  if (!(standardError >= 0.0) || !std::isfinite(standardError))
    standardError = 0.0;
  return Interval(lower, upper);
}

/** Exact binomial order-statistic interval. */
Interval exactInterval(const Sample & sample,
                       const Scalar alpha,
                       const Scalar beta,
                       Scalar & estimate,
                       Scalar & standardError)
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The sample must not be empty";

  const Sample sortedSample(sample.sort());
  Point values(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    values[i] = sortedSample(i, 0);

  // The ranks are computed from the sample size, alpha and beta only.
  const QuantileConfidence quantileConfidence(alpha, beta);
  const Indices ranks(quantileConfidence.computeBilateralRank(size));

  const Scalar scalarIndex = std::ceil(1.0 * size * alpha);
  UnsignedInteger k = static_cast<UnsignedInteger>(scalarIndex);
  if (k > 0)
    --k;
  if (k >= size)
    k = size - 1;
  estimate = values[k];

  UnsignedInteger lowerRank = ranks[0];
  UnsignedInteger upperRank = ranks[1];
  if (k < lowerRank)
    lowerRank = k;
  if (k > upperRank)
    upperRank = k;
  const Interval interval(Point({values[lowerRank]}), Point({values[upperRank]}));

  // Spacing estimate of the density at the quantile.
  const UnsignedInteger r1 = (k > 0) ? k - 1 : 0;
  const UnsignedInteger r2 = (k + 1 < size) ? k + 1 : size - 1;
  standardError = 0.0;
  if (r2 > r1)
  {
    const Scalar width = values[r2] - values[r1];
    if (width > 0.0)
    {
      const Scalar density = 1.0 * (r2 - r1) / (1.0 * size * width);
      standardError = std::sqrt(alpha * (1.0 - alpha) / (1.0 * size)) / density;
    }
  }
  if (!(standardError > 0.0) || !std::isfinite(standardError))
  {
    const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta));
    standardError = (interval.getUpperBound()[0] - interval.getLowerBound()[0]) / (2.0 * z);
    if (!(standardError >= 0.0) || !std::isfinite(standardError))
      standardError = 0.0;
  }
  return interval;
}

/** Student t-interval from independent replicate estimates. */
Interval replicateInterval(const Point & estimates,
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
  return Interval(mean - halfWidth, mean + halfWidth);
}

/** Build a result from the common statistical fields. */
QuantileSimulationResult buildResult(const RandomVector & output,
                                     const Scalar alpha,
                                     const Scalar beta,
                                     const Scalar estimate,
                                     const Scalar standardError,
                                     const Interval & interval,
                                     const QuantileSimulationResultImplementation::EstimatorType estimatorType,
                                     const QuantileSimulationResultImplementation::CoverageType coverageType,
                                     const QuantileSimulationResultImplementation::CoverageType requiredCoverage,
                                     const Scalar effectiveSampleSize,
                                     const UnsignedInteger sampleSize,
                                     const UnsignedInteger outerSampling,
                                     const UnsignedInteger blockSize,
                                     const Sample & history)
{
  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output, alpha, beta));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(estimatorType);
  result.setCoverageType(coverageType);
  result.setRequiredCoverage(requiredCoverage);
  result.setEffectiveSampleSize(effectiveSampleSize);
  result.setSampleSize(sampleSize);
  result.setBlockLength(0);
  result.setIsFixedSample(true);
  result.setOuterSampling(outerSampling);
  result.setBlockSize(blockSize);
  result.setConvergenceHistory(history);
  return result;
}

} // anonymous namespace

/* Default constructor */
QuantileSimulationAlgorithm::QuantileSimulationAlgorithm()
  : QuantileSimulationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
QuantileSimulationAlgorithm::QuantileSimulationAlgorithm(const RandomVector & output,
                                                         const Scalar alpha,
                                                         const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  // Nothing to do
}

/* Constructor with parameters and experiment */
QuantileSimulationAlgorithm::QuantileSimulationAlgorithm(const RandomVector & output,
                                                         const WeightedExperiment & experiment,
                                                         const Scalar alpha,
                                                         const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  setExperiment(experiment);
}

/* Virtual constructor */
QuantileSimulationAlgorithm * QuantileSimulationAlgorithm::clone() const
{
  return new QuantileSimulationAlgorithm(*this);
}

/* Experiment accessor */
void QuantileSimulationAlgorithm::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  isExperimentProvided_ = true;
  sampleSize_ = experiment_.getSize();
}

WeightedExperiment QuantileSimulationAlgorithm::getExperiment() const
{
  return experiment_;
}

Bool QuantileSimulationAlgorithm::hasExperiment() const
{
  return isExperimentProvided_;
}

/* Sample size accessor */
void QuantileSimulationAlgorithm::setSampleSize(const UnsignedInteger sampleSize)
{
  if (isExperimentProvided_)
    checkResizableExperiment(experiment_, sampleSize);
  QuantileSimulationImplementation::setSampleSize(sampleSize);
  if (isExperimentProvided_ && (experiment_.getSize() != sampleSize))
    experiment_.setSize(sampleSize);
}

/* Replicate count accessor */
void QuantileSimulationAlgorithm::setReplicateCount(const UnsignedInteger replicateCount)
{
  if (replicateCount < 2)
    throw InvalidArgumentException(HERE)
        << "The replicate count must be at least 2, but replicateCount = " << replicateCount;
  replicateCount_ = replicateCount;
}

UnsignedInteger QuantileSimulationAlgorithm::getReplicateCount() const
{
  return replicateCount_;
}

/* Keep sample accessor */
void QuantileSimulationAlgorithm::setKeepSample(const Bool keepSample)
{
  if (!keepSample)
  {
    inputSample_ = Sample();
    outputSample_ = Sample();
    weights_ = Point();
  }
  keepSample_ = keepSample;
}

Bool QuantileSimulationAlgorithm::getKeepSample() const
{
  return keepSample_;
}

/* Input sample accessor */
Sample QuantileSimulationAlgorithm::getInputSample() const
{
  return inputSample_;
}

/* Output sample accessor */
Sample QuantileSimulationAlgorithm::getOutputSample() const
{
  return outputSample_;
}

/* Weights accessor */
Point QuantileSimulationAlgorithm::getWeights() const
{
  return weights_;
}

/* Check the input parameters */
void QuantileSimulationAlgorithm::checkInputs() const
{
  QuantileSimulationImplementation::checkInputs();
  if (requiredCoverage_ == QuantileSimulationResultImplementation::COVERAGE_ANYTIME_VALID)
    throw InvalidArgumentException(HERE)
        << "The weighted-experiment estimator cannot provide an anytime-valid coverage";
}

/* Run a fixed-sample experiment */
QuantileSimulationResult QuantileSimulationAlgorithm::runFixedSample()
{
  if (sampleSize_ == 0)
    throw InvalidArgumentException(HERE) << "The sample size must be set before running the algorithm";

  inputSample_ = Sample();
  outputSample_ = Sample();
  weights_ = Point();

  WeightedExperiment experiment = experiment_;
  if (!isExperimentProvided_)
  {
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
    experiment = MonteCarloExperiment(distribution, sampleSize_);
  }
  else
  {
    if (experiment.getSize() != sampleSize_)
    {
      checkResizableExperiment(experiment, sampleSize_);
      experiment.setSize(sampleSize_);
    }
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
    // A stochastic experiment must sample from the input distribution of
    // the output.  Otherwise the estimate describes the experiment
    // distribution and the coverage label would be meaningless.  Fixed
    // designs are diagnostic and may carry arbitrary supplied values;
    // TensorProductExperiment reports a block-independent distribution
    // built from its marginals, whose structural equality with the input
    // distribution is not a reliable test.
    if (experiment.isRandom()
        && (dynamic_cast<const TensorProductExperiment *>(experiment.getImplementation().get()) == nullptr))
    {
      Distribution inputDistribution;
      Bool hasInputDistribution = true;
      try
      {
        inputDistribution = output_.isComposite()
                            ? output_.getAntecedent().getDistribution()
                            : output_.getDistribution();
      }
      catch (const Exception &)
      {
        hasInputDistribution = false;
      }
      if (hasInputDistribution)
      {
        Bool sameDistribution = true;
        try
        {
          sameDistribution = (experiment.getDistribution() == inputDistribution);
        }
        catch (const Exception &)
        {
          // Some distributions do not implement structural equality:
          // keep the dimension check as the fallback.
        }
        if (!sameDistribution)
          throw InvalidArgumentException(HERE)
              << "The experiment distribution does not match the input distribution of the output random vector";
      }
    }
  }

  const Bool uniformWeights = experiment.hasUniformWeights();
  const Bool randomExperiment = experiment.isRandom();
  const Bool iidExperiment = experiment.isIID();
  // ImportanceSamplingExperiment draws i.i.d. points from its proposal and
  // attaches the likelihood ratio as a weight, so the Woodruff variance
  // estimator is valid even though the weights are not uniform.  For any
  // other weighted design the variance estimator would omit covariance
  // terms, so the coverage is not claimed.
  const Bool importanceSamplingDraws =
      dynamic_cast<const ImportanceSamplingExperiment *>(experiment.getImplementation().get()) != nullptr;

  QuantileSimulationResultImplementation::CoverageType coverageType;
  if (iidExperiment && uniformWeights && (estimatorType_ == QuantileSimulationResultImplementation::EMPIRICAL_INVERSE))
    coverageType = QuantileSimulationResultImplementation::COVERAGE_EXACT;
  else if (!randomExperiment || (!uniformWeights && !importanceSamplingDraws))
    coverageType = QuantileSimulationResultImplementation::COVERAGE_NONE;
  else
    coverageType = QuantileSimulationResultImplementation::COVERAGE_ASYMPTOTIC;

  // Exact i.i.d. path: one draw, binomial order-statistic ranks.
  if (coverageType == QuantileSimulationResultImplementation::COVERAGE_EXACT)
  {
    Point weights;
    const Sample inputSample(experiment.generateWithWeights(weights));
    const Sample outputSample(output_.getFrozenSample(inputSample));
    if (keepSample_)
    {
      inputSample_ = inputSample;
      outputSample_ = outputSample;
      weights_ = weights;
    }
    Scalar estimate = 0.0;
    Scalar standardError = 0.0;
    const Interval interval(exactInterval(outputSample, alpha_, beta_, estimate, standardError));
    Sample history(1, 4);
    history(0, 0) = sampleSize_;
    history(0, 1) = estimate;
    history(0, 2) = interval.getLowerBound()[0];
    history(0, 3) = interval.getUpperBound()[0];
    return buildResult(output_, alpha_, beta_, estimate, standardError, interval,
                       estimatorType_, coverageType, requiredCoverage_,
                       1.0 * sampleSize_, sampleSize_, 1,
                       getBlockSize(), history);
  }

  // Randomized stratified path: independent replicates and a Student t-interval.
  if (uniformWeights && randomExperiment && !iidExperiment)
  {
    // A Latin hypercube experiment reuses its stratum permutation from one
    // draw to the next when alwaysShuffle is false.  The replicate estimates
    // are then correlated (and identical when randomShift is false too), so
    // the Student interval is not valid.  The same holds when such an
    // experiment is nested in a tensor product.  Reject the configuration
    // instead of reporting a coverage that the design cannot support.
    if (reusesLHSShuffle(experiment))
      throw InvalidArgumentException(HERE)
          << "The replicate interval requires independent draws, but the Latin hypercube experiment "
          << "reuses its shuffle (alwaysShuffle=false) or keeps a fixed random shift, possibly inside "
          << "a Sobol or tensor-product wrapper. Set alwaysShuffle=true and randomShift=true, "
          << "or use another experiment.";
    const UnsignedInteger replicateCount = replicateCount_;
    if (replicateCount < 2)
      throw InvalidArgumentException(HERE)
          << "At least two replicates are required for a replicate interval, but replicateCount = " << replicateCount;
    Point estimates(replicateCount);
    Sample history(replicateCount, 4);
    Scalar runningMean = 0.0;
    Scalar runningSumSquare = 0.0;
    for (UnsignedInteger r = 0; r < replicateCount; ++r)
    {
      Point weights;
      const Sample inputSample(experiment.generateWithWeights(weights));
      const Sample outputSample(output_.getFrozenSample(inputSample));
      if (keepSample_ && (r == 0))
      {
        inputSample_ = inputSample;
        outputSample_ = outputSample;
        weights_ = weights;
      }
      const WeightedSample data(prepareWeightedSample(outputSample, weights));
      const Scalar replicateEstimate = (estimatorType_ == QuantileSimulationResultImplementation::EMPIRICAL_INVERSE)
                                       ? weightedQuantile(data, alpha_)
                                       : interpolatedQuantile(data, alpha_);
      estimates[r] = replicateEstimate;
      const Scalar count = 1.0 * (r + 1);
      const Scalar delta = replicateEstimate - runningMean;
      runningMean += delta / count;
      runningSumSquare += delta * (replicateEstimate - runningMean);
      history(r, 0) = 1.0 * (r + 1) * sampleSize_;
      history(r, 1) = runningMean;
      if (r == 0)
      {
        history(r, 2) = replicateEstimate;
        history(r, 3) = replicateEstimate;
      }
      else
      {
        const Scalar variance = runningSumSquare / (1.0 * r);
        const Scalar standardError = std::sqrt(std::max(0.0, variance) / count);
        const Scalar t = DistFunc::qStudent(1.0 * r, 0.5 * (1.0 + beta_));
        history(r, 2) = runningMean - t * standardError;
        history(r, 3) = runningMean + t * standardError;
      }
    }
    Scalar estimate = 0.0;
    Scalar standardError = 0.0;
    const Interval interval(replicateInterval(estimates, beta_, estimate, standardError));
    return buildResult(output_, alpha_, beta_, estimate, standardError, interval,
                       estimatorType_, coverageType, requiredCoverage_,
                       1.0 * replicateCount * sampleSize_,
                       replicateCount * sampleSize_, replicateCount,
                       getBlockSize(), history);
  }

  // Weighted or deterministic path: one draw and the Woodruff diagnostic.
  {
    Point weights;
    const Sample inputSample(experiment.generateWithWeights(weights));
    const Sample outputSample(output_.getFrozenSample(inputSample));
    if (keepSample_)
    {
      inputSample_ = inputSample;
      outputSample_ = outputSample;
      weights_ = weights;
    }
    const WeightedSample data(prepareWeightedSample(outputSample, weights));
    const Scalar estimate = (estimatorType_ == QuantileSimulationResultImplementation::EMPIRICAL_INVERSE)
                            ? weightedQuantile(data, alpha_)
                            : interpolatedQuantile(data, alpha_);
    Scalar standardError = 0.0;
    const Interval interval(woodruffInterval(data, alpha_, beta_, estimate, standardError));
    Sample history(1, 4);
    history(0, 0) = sampleSize_;
    history(0, 1) = estimate;
    history(0, 2) = interval.getLowerBound()[0];
    history(0, 3) = interval.getUpperBound()[0];
    const Scalar effectiveSampleSize = 1.0 / data.squareSum;
    return buildResult(output_, alpha_, beta_, estimate, standardError, interval,
                       estimatorType_, coverageType, requiredCoverage_,
                       effectiveSampleSize, sampleSize_, 1, getBlockSize(), history);
  }
}

/* String converter */
String QuantileSimulationAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << QuantileSimulationAlgorithm::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " estimator=" << estimatorType_
      << " requiredCoverage=" << requiredCoverage_
      << " experimentProvided=" << isExperimentProvided_;
  if (isExperimentProvided_)
    oss << " experiment=" << experiment_;
  oss << " replicateCount=" << replicateCount_
      << " keepSample=" << keepSample_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void QuantileSimulationAlgorithm::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("isExperimentProvided_", isExperimentProvided_);
  adv.saveAttribute("replicateCount_", replicateCount_);
  adv.saveAttribute("keepSample_", keepSample_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
  adv.saveAttribute("weights_", weights_);
}

/* Method load() reloads the object from the StorageManager */
void QuantileSimulationAlgorithm::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("isExperimentProvided_", isExperimentProvided_);
  adv.loadAttribute("replicateCount_", replicateCount_);
  adv.loadAttribute("keepSample_", keepSample_);
  adv.loadAttribute("inputSample_", inputSample_);
  adv.loadAttribute("outputSample_", outputSample_);
  adv.loadAttribute("weights_", weights_);
}

END_NAMESPACE_OPENTURNS
