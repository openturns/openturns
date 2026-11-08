//                                               -*- C++ -*-
/**
 *  @brief Two-stage quantile simulation
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
#include <limits>

#include "openturns/TwoStageQuantileSimulation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/QuantileConfidence.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TwoStageQuantileSimulation)

static const Factory<TwoStageQuantileSimulation> Factory_TwoStageQuantileSimulation;

namespace
{

/* Index of the empirical inverse in a sorted sample */
UnsignedInteger twoStageEmpiricalInverseIndex(const UnsignedInteger size, const Scalar alpha)
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
Scalar twoStageSpacingDensity(const Sample & sortedSample, const UnsignedInteger index)
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

/* Exact binomial order-statistic interval */
Interval twoStageExactInterval(const Sample & sample,
                       const Scalar alpha,
                       const Scalar beta,
                       Scalar & estimate,
                       Scalar & standardError)
{
  const UnsignedInteger size = sample.getSize();
  if (size == 0)
    throw InvalidArgumentException(HERE) << "The confirmation sample must not be empty";

  const Sample sortedSample(sample.sort());
  const QuantileConfidence quantileConfidence(alpha, beta);
  const Indices ranks(quantileConfidence.computeBilateralRank(size));

  const UnsignedInteger k = twoStageEmpiricalInverseIndex(size, alpha);
  estimate = sortedSample(k, 0);

  UnsignedInteger lowerRank = ranks[0];
  UnsignedInteger upperRank = ranks[1];
  if (k < lowerRank)
    lowerRank = k;
  if (k > upperRank)
    upperRank = k;
  const Interval interval(Point({sortedSample(lowerRank, 0)}),
                          Point({sortedSample(upperRank, 0)}));

  const UnsignedInteger lowerSpacingIndex = (k > 0) ? k - 1 : 0;
  const UnsignedInteger upperSpacingIndex = (k + 1 < size) ? k + 1 : size - 1;
  standardError = 0.0;
  if (upperSpacingIndex > lowerSpacingIndex)
  {
    const Scalar width = sortedSample(upperSpacingIndex, 0) - sortedSample(lowerSpacingIndex, 0);
    if (width > 0.0)
    {
      const Scalar density = 1.0 * (upperSpacingIndex - lowerSpacingIndex) / (1.0 * size * width);
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

} // anonymous namespace

/* Default constructor */
TwoStageQuantileSimulation::TwoStageQuantileSimulation()
  : QuantileSimulationImplementation()
{
  // Nothing to do
}

/* Constructor with parameters */
TwoStageQuantileSimulation::TwoStageQuantileSimulation(const RandomVector & output,
                                                       const Scalar alpha,
                                                       const Scalar beta)
  : QuantileSimulationImplementation(output, alpha, beta)
{
  // Nothing to do
}

/* Virtual constructor */
TwoStageQuantileSimulation * TwoStageQuantileSimulation::clone() const
{
  return new TwoStageQuantileSimulation(*this);
}

/* Pilot sample size accessor */
void TwoStageQuantileSimulation::setPilotSize(const UnsignedInteger pilotSize)
{
  if (pilotSize == 0)
    throw InvalidArgumentException(HERE) << "The pilot size must be positive";
  pilotSize_ = pilotSize;
}

UnsignedInteger TwoStageQuantileSimulation::getPilotSize() const
{
  return pilotSize_;
}

/* Target half-width accessor */
void TwoStageQuantileSimulation::setTargetHalfWidth(const Scalar halfWidth)
{
  if (!std::isfinite(halfWidth) || !(halfWidth > 0.0))
    throw InvalidArgumentException(HERE)
        << "The target half-width must be finite and positive, but halfWidth = " << halfWidth;
  targetHalfWidth_ = halfWidth;
}

Scalar TwoStageQuantileSimulation::getTargetHalfWidth() const
{
  return targetHalfWidth_;
}

Bool TwoStageQuantileSimulation::hasTargetHalfWidth() const
{
  return targetHalfWidth_ > 0.0;
}

/* Run the two-stage experiment */
QuantileSimulationResult TwoStageQuantileSimulation::runFixedSample()
{
  const UnsignedInteger budget = sampleSize_;
  const QuantileConfidence quantileConfidence(alpha_, beta_);
  const UnsignedInteger minimumConfirmationSize = quantileConfidence.computeBilateralMinimumSampleSize();
  if (budget <= minimumConfirmationSize)
    throw InvalidArgumentException(HERE)
        << "The evaluation budget (" << budget
        << ") must be larger than the minimum confirmation size (" << minimumConfirmationSize
        << ") required for alpha = " << alpha_ << " and beta = " << beta_;

  // The pilot is paid from the budget and must leave room for a valid exact interval.
  const UnsignedInteger maximumPilotSize = budget - minimumConfirmationSize;
  const UnsignedInteger pilotSize = std::min(pilotSize_, maximumPilotSize);

  const Sample pilotSample(output_.getSample(pilotSize));
  const Sample sortedPilotSample(pilotSample.sort());
  const UnsignedInteger pilotIndex = twoStageEmpiricalInverseIndex(pilotSize, alpha_);
  const Scalar pilotQuantileEstimate = sortedPilotSample(pilotIndex, 0);

  // Size the confirmation sample from the exact bilateral pilot interval.
  // Its half-width h0 estimates z_{1-beta/2} sqrt(alpha(1-alpha))
  // / (f(q_alpha) sqrt(N0)) with far less variance than a single-spacing
  // density estimate, while the rule N1 = N0 (h0 / h)^2 remains the
  // finite-sample form of the asymptotic sizing formula.  The spacing
  // density is only a fallback when the pilot is too small for an exact
  // interval; it is still reported as a diagnostic.
  Scalar pilotHalfWidth = 0.0;
  Scalar pilotDensity = 0.0;
  if (pilotSize >= minimumConfirmationSize)
  {
    const Indices pilotRanks(quantileConfidence.computeBilateralRank(pilotSize));
    const Scalar pilotWidth = sortedPilotSample(pilotRanks[1], 0) - sortedPilotSample(pilotRanks[0], 0);
    if (pilotWidth > 0.0)
    {
      pilotHalfWidth = 0.5 * pilotWidth;
      pilotDensity = 1.0 * (pilotRanks[1] - pilotRanks[0]) / (1.0 * pilotSize * pilotWidth);
    }
  }
  if (!(pilotDensity > 0.0))
    pilotDensity = twoStageSpacingDensity(sortedPilotSample, pilotIndex);

  const UnsignedInteger availableConfirmationSize = budget - pilotSize;
  UnsignedInteger requiredConfirmationSize = availableConfirmationSize;
  Bool budgetExhausted = false;
  if (hasTargetHalfWidth())
  {
    Scalar requiredAsScalar = 0.0;
    Bool hasSizeRule = false;
    if (pilotHalfWidth > 0.0 && std::isfinite(pilotHalfWidth))
    {
      const Scalar widthRatio = pilotHalfWidth / targetHalfWidth_;
      requiredAsScalar = 1.0 * pilotSize * widthRatio * widthRatio;
      hasSizeRule = true;
    }
    else if (pilotDensity > 0.0)
    {
      const Scalar z = DistFunc::qNormal(0.5 * (1.0 + beta_));
      const Scalar numerator = z * z * alpha_ * (1.0 - alpha_);
      const Scalar denominator = pilotDensity * pilotDensity * targetHalfWidth_ * targetHalfWidth_;
      requiredAsScalar = numerator / denominator;
      hasSizeRule = true;
    }
    if (!hasSizeRule || !(requiredAsScalar <= 1.0 * availableConfirmationSize))
    {
      requiredConfirmationSize = availableConfirmationSize;
      if (availableConfirmationSize < std::numeric_limits<UnsignedInteger>::max())
        requiredConfirmationSize = availableConfirmationSize + 1;
      budgetExhausted = true;
    }
    else
      requiredConfirmationSize = std::max(minimumConfirmationSize,
                                          static_cast<UnsignedInteger>(std::ceil(requiredAsScalar)));
  }
  const UnsignedInteger confirmationSize = std::min(requiredConfirmationSize, availableConfirmationSize);
  if (budgetExhausted)
    LOGWARN(OSS() << "The two-stage quantile simulation budget (" << budget
            << ") is insufficient to reach the target half-width (" << targetHalfWidth_
            << "): the confirmation sample is limited to " << confirmationSize
            << " observations");

  const Sample confirmationSample(output_.getSample(confirmationSize));
  Scalar estimate = 0.0;
  Scalar standardError = 0.0;
  const Interval interval(twoStageExactInterval(confirmationSample, alpha_, beta_, estimate, standardError));

  QuantileSimulationResult result(new QuantileSimulationResultImplementation(output_, alpha_, beta_));
  result.setQuantileEstimate(estimate);
  result.setStandardError(standardError);
  result.setConfidenceInterval(interval);
  result.setEstimatorType(QuantileSimulationResultImplementation::EMPIRICAL_INVERSE);
  result.setCoverageType(QuantileSimulationResultImplementation::COVERAGE_EXACT);
  result.setRequiredCoverage(requiredCoverage_);
  result.setSampleSize(pilotSize + confirmationSize);
  result.setEffectiveSampleSize(1.0 * confirmationSize);
  result.setBlockLength(0);
  result.setIsFixedSample(true);
  result.setOuterSampling(1);
  result.setBlockSize(getBlockSize());
  result.setPilotSize(pilotSize);
  result.setPilotQuantileEstimate(pilotQuantileEstimate);
  result.setPilotDensity(pilotDensity);
  result.setConfirmationSize(confirmationSize);
  result.setRequiredConfirmationSize(requiredConfirmationSize);
  if (hasTargetHalfWidth())
    result.setTargetHalfWidth(targetHalfWidth_);
  result.setBudgetExhausted(budgetExhausted);

  Sample history(1, 4);
  history(0, 0) = pilotSize + confirmationSize;
  history(0, 1) = estimate;
  history(0, 2) = interval.getLowerBound()[0];
  history(0, 3) = interval.getUpperBound()[0];
  result.setConvergenceHistory(history);
  return result;
}

/* String converter */
String TwoStageQuantileSimulation::__repr__() const
{
  OSS oss;
  oss << "class=" << TwoStageQuantileSimulation::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " sampleSize=" << sampleSize_
      << " pilotSize=" << pilotSize_
      << " hasTargetHalfWidth=" << hasTargetHalfWidth()
      << " targetHalfWidth=" << targetHalfWidth_
      << " requiredCoverage=" << requiredCoverage_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void TwoStageQuantileSimulation::save(Advocate & adv) const
{
  QuantileSimulationImplementation::save(adv);
  adv.saveAttribute("pilotSize_", pilotSize_);
  adv.saveAttribute("targetHalfWidth_", targetHalfWidth_);
}

/* Method load() reloads the object from the StorageManager */
void TwoStageQuantileSimulation::load(Advocate & adv)
{
  QuantileSimulationImplementation::load(adv);
  adv.loadAttribute("pilotSize_", pilotSize_);
  adv.loadAttribute("targetHalfWidth_", targetHalfWidth_);
}

END_NAMESPACE_OPENTURNS
