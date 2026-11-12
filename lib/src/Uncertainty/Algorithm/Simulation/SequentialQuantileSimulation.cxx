//                                               -*- C++ -*-
/**
 *  @brief Sequential quantile simulation
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
#include <algorithm>
#include <cmath>

#include "openturns/SequentialQuantileSimulation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SequentialQuantileSimulation)

static const Factory<SequentialQuantileSimulation> Factory_SequentialQuantileSimulation;

namespace
{

/* Index of the empirical inverse in a sorted sample */
UnsignedInteger sequentialEmpiricalInverseIndex(const UnsignedInteger size, const Scalar alpha)
{
  const Scalar scalarIndex = std::ceil(1.0 * size * alpha);
  UnsignedInteger index = static_cast<UnsignedInteger>(scalarIndex);
  if (index > 0)
    --index;
  if (index >= size)
    index = size - 1;
  return index;
}

/* Spacing estimate of the density at an order statistic */
Scalar sequentialSpacingDensity(const Sample & sortedSample, const UnsignedInteger index)
{
  const UnsignedInteger size = sortedSample.getSize();
  if (size < 2)
    return 0.0;
  UnsignedInteger halfWidth = 1;
  while (true)
  {
    const UnsignedInteger lowerIndex = (index >= halfWidth) ? index - halfWidth : 0;
    const UnsignedInteger upperIndex = (index + halfWidth < size) ? index + halfWidth : size - 1;
    if (upperIndex > lowerIndex)
    {
      const Scalar width = sortedSample(upperIndex, 0) - sortedSample(lowerIndex, 0);
      if (width > 0.0)
        return 1.0 * (upperIndex - lowerIndex) / (1.0 * size * width);
    }
    if (halfWidth >= size)
      break;
    halfWidth *= 2;
  }
  return 0.0;
}

} // anonymous namespace

/* Default constructor */
SequentialQuantileSimulation::SequentialQuantileSimulation()
  : QuantileSimulationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
SequentialQuantileSimulation::SequentialQuantileSimulation(const RandomVector & output,
                                                           const Scalar alpha,
                                                           const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  // Nothing to do
}

/* Virtual constructor */
SequentialQuantileSimulation * SequentialQuantileSimulation::clone() const
{
  return new SequentialQuantileSimulation(*this);
}

/* Target half-width accessor */
void SequentialQuantileSimulation::setTargetHalfWidth(const Scalar halfWidth)
{
  if (!std::isfinite(halfWidth) || !(halfWidth > 0.0))
    throw InvalidArgumentException(HERE)
        << "The target half-width must be finite and positive, but halfWidth = " << halfWidth;
  targetHalfWidth_ = halfWidth;
}

Scalar SequentialQuantileSimulation::getTargetHalfWidth() const
{
  return targetHalfWidth_;
}

Bool SequentialQuantileSimulation::hasTargetHalfWidth() const
{
  return targetHalfWidth_ > 0.0;
}

/* Run the sequential experiment */
QuantileSimulationResult SequentialQuantileSimulation::runFixedSample()
{
  const UnsignedInteger budget = sampleSize_;
  const Scalar gridRatio = ResourceMap::GetAsScalar("SequentialQuantileSimulation-GridRatio");
  const UnsignedInteger minimumGridSize = ResourceMap::GetAsUnsignedInteger("SequentialQuantileSimulation-MinimumGridSize");
  if (!(gridRatio > 1.0))
    throw InvalidArgumentException(HERE)
        << "The grid ratio must be larger than 1, but gridRatio = " << gridRatio
        << " (ResourceMap key SequentialQuantileSimulation-GridRatio)";
  if (minimumGridSize == 0)
    throw InvalidArgumentException(HERE)
        << "The minimum grid size must be positive (ResourceMap key SequentialQuantileSimulation-MinimumGridSize)";

  const Scalar failureProbability = 1.0 - beta_;
  const Bool targetIsSet = hasTargetHalfWidth();
  Sample accumulatedSample(0, 1);
  Sample path(0, 4);
  Sample sortedSample(0, 1);
  UnsignedInteger candidateSize = std::min(minimumGridSize, budget);
  UnsignedInteger gridIndex = 0;
  Scalar estimate = 0.0;
  Scalar lowerBound = 0.0;
  Scalar upperBound = 0.0;
  Bool finiteLowerBound = false;
  Bool finiteUpperBound = false;
  String stoppingCriterion;

  // The candidate sizes form a deterministic geometric grid.  The union
  // bound delta / ((k + 1) * (k + 2)) over the grid indices k sums to
  // delta = 1 - beta, so the intervals hold simultaneously at every
  // candidate size and optional stopping preserves the coverage.
  while (true)
  {
    const UnsignedInteger currentSize = accumulatedSample.getSize();
    if (candidateSize > currentSize)
      accumulatedSample.add(output_.getSample(candidateSize - currentSize));
    sortedSample = accumulatedSample.sort();

    const Scalar delta = failureProbability / ((1.0 * gridIndex + 1.0) * (1.0 * gridIndex + 2.0));
    const Scalar epsilon = std::sqrt(std::log(2.0 / delta) / (2.0 * candidateSize));

    estimate = sortedSample(sequentialEmpiricalInverseIndex(candidateSize, alpha_), 0);

    // Invert the distribution-function band [Fhat - epsilon, Fhat + epsilon].
    // The lower quantile bound is the smallest observation whose empirical
    // CDF reaches alpha - epsilon, and the upper bound reaches alpha + epsilon.
    const Scalar lowerProbability = alpha_ - epsilon;
    const Scalar upperProbability = alpha_ + epsilon;
    finiteLowerBound = lowerProbability > 0.0;
    finiteUpperBound = upperProbability < 1.0;
    if (finiteLowerBound)
    {
      UnsignedInteger lowerIndex = static_cast<UnsignedInteger>(std::ceil(1.0 * candidateSize * lowerProbability));
      if (lowerIndex == 0)
        lowerIndex = 1;
      lowerBound = sortedSample(lowerIndex - 1, 0);
    }
    else
      lowerBound = -SpecFunc::MaxScalar;
    if (finiteUpperBound)
    {
      // The upper endpoint is the smallest observation whose empirical
      // distribution function is strictly larger than alpha + epsilon.
      UnsignedInteger upperIndex = static_cast<UnsignedInteger>(std::floor(1.0 * candidateSize * upperProbability));
      if (upperIndex >= candidateSize)
        upperIndex = candidateSize - 1;
      upperBound = sortedSample(upperIndex, 0);
    }
    else
      upperBound = SpecFunc::MaxScalar;

    Point row(4);
    row[0] = 1.0 * candidateSize;
    row[1] = estimate;
    row[2] = lowerBound;
    row[3] = upperBound;
    path.add(row);

    const Scalar halfWidth = (finiteLowerBound && finiteUpperBound)
                             ? 0.5 * (upperBound - lowerBound)
                             : SpecFunc::MaxScalar;
    if (targetIsSet && (halfWidth <= targetHalfWidth_))
    {
      stoppingCriterion = "target half-width";
      break;
    }
    if (candidateSize >= budget)
    {
      stoppingCriterion = "maximum sample size";
      break;
    }

    UnsignedInteger nextSize = budget;
    const Scalar nextSizeAsScalar = gridRatio * candidateSize;
    if (nextSizeAsScalar < 1.0 * budget)
      nextSize = std::max(candidateSize + 1,
                          static_cast<UnsignedInteger>(std::ceil(nextSizeAsScalar)));
    candidateSize = nextSize;
    ++gridIndex;
  }

  const UnsignedInteger stoppingTime = candidateSize;
  const Bool budgetExhausted = targetIsSet && (stoppingCriterion == "maximum sample size");
  if (budgetExhausted)
    LOGWARN(OSS() << "The sequential quantile simulation reached the maximum sample size ("
            << budget << ") without reaching the target half-width (" << targetHalfWidth_
            << "); the reported interval is the anytime-valid interval at the stopping time");

  Scalar standardError = 0.0;
  const UnsignedInteger estimateIndex = sequentialEmpiricalInverseIndex(stoppingTime, alpha_);
  const Scalar density = sequentialSpacingDensity(sortedSample, estimateIndex);
  if (density > 0.0)
    standardError = std::sqrt(alpha_ * (1.0 - alpha_) / (1.0 * stoppingTime)) / density;
  if (!(standardError > 0.0) || !std::isfinite(standardError))
  {
    if (finiteLowerBound && finiteUpperBound)
    {
      const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta_));
      standardError = 0.5 * (upperBound - lowerBound) / z;
    }
    if (!(standardError >= 0.0) || !std::isfinite(standardError))
      standardError = 0.0;
  }

  Interval::BoolCollection finiteLower(1, finiteLowerBound);
  Interval::BoolCollection finiteUpper(1, finiteUpperBound);
  const Interval interval(Point({lowerBound}), Point({upperBound}), finiteLower, finiteUpper);

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(QuantileSimulationResultImplementation::EMPIRICAL_INVERSE);
  result.setCoverageType(QuantileSimulationResultImplementation::COVERAGE_ANYTIME_VALID);
  result.setCoverageAssumption("Dvoretzky-Kiefer-Wolfowitz-Massart inequality with a union bound over the evaluated sample sizes");
  result.setRequiredCoverage(requiredCoverage_);
  result.setSampleSize(stoppingTime);
  result.setEffectiveSampleSize(1.0 * stoppingTime);
  result.setBlockLength(0);
  result.setIsFixedSample(false);
  result.setOuterSampling(1);
  result.setBlockSize(getBlockSize());
  result.setStoppingTime(stoppingTime);
  result.setStoppingCriterion(stoppingCriterion);
  result.setConfidenceSequencePath(path);
  result.setConvergenceHistory(path);
  if (targetIsSet)
    result.setTargetHalfWidth(targetHalfWidth_);
  result.setBudgetExhausted(budgetExhausted);
  return result;
}

/* String converter */
String SequentialQuantileSimulation::__repr__() const
{
  OSS oss;
  oss << "class=" << SequentialQuantileSimulation::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " hasTargetHalfWidth=" << hasTargetHalfWidth()
      << " targetHalfWidth=" << targetHalfWidth_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void SequentialQuantileSimulation::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("targetHalfWidth_", targetHalfWidth_);
}

/* Method load() reloads the object from the StorageManager */
void SequentialQuantileSimulation::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("targetHalfWidth_", targetHalfWidth_);
}

END_NAMESPACE_OPENTURNS
