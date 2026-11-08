//                                               -*- C++ -*-
/**
 *  @brief Base class for quantile simulation results
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
#include <limits>

#include "openturns/QuantileSimulationResultImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(QuantileSimulationResultImplementation)

static const Factory<QuantileSimulationResultImplementation> Factory_QuantileSimulationResultImplementation;

/* Default constructor */
QuantileSimulationResultImplementation::QuantileSimulationResultImplementation()
  : SimulationResult()
  , output_(RandomVector(Uniform()))
  , confidenceInterval_()
{
  // Nothing to do
}

/* Standard constructor */
QuantileSimulationResultImplementation::QuantileSimulationResultImplementation(const RandomVector & output,
                                                                               const Scalar alpha,
                                                                               const Scalar beta)
  : SimulationResult()
  , confidenceInterval_()
{
  setOutput(output);
  setQuantileLevel(alpha);
  setConfidenceLevel(beta);
}

/* Virtual constructor */
QuantileSimulationResultImplementation * QuantileSimulationResultImplementation::clone() const
{
  return new QuantileSimulationResultImplementation(*this);
}

/* Output random vector accessor */
void QuantileSimulationResultImplementation::setOutput(const RandomVector & output)
{
  UnsignedInteger dimension = 0;
  try
  {
    dimension = output.getDimension();
  }
  catch (const Exception &)
  {
    throw InvalidArgumentException(HERE) << "The output random vector must be scalar, but its dimension cannot be retrieved";
  }
  if (dimension != 1)
    throw InvalidArgumentException(HERE)
        << "The quantile simulation result only supports a scalar output random vector, but the output dimension is "
        << dimension << ". Select a marginal with output.getMarginal(i), or define a scalar function h and "
        << "use CompositeRandomVector(h, output).";
  output_ = output;
}

RandomVector QuantileSimulationResultImplementation::getOutput() const
{
  return output_;
}

/* Quantile level accessor */
void QuantileSimulationResultImplementation::setQuantileLevel(const Scalar alpha)
{
  if (!(alpha > 0.0 && alpha < 1.0))
    throw InvalidArgumentException(HERE)
        << "The quantile level must be in (0, 1), but alpha = " << alpha;
  alpha_ = alpha;
}

Scalar QuantileSimulationResultImplementation::getQuantileLevel() const
{
  return alpha_;
}

/* Confidence level accessor */
void QuantileSimulationResultImplementation::setConfidenceLevel(const Scalar beta)
{
  if (!(beta > 0.0 && beta < 1.0))
    throw InvalidArgumentException(HERE)
        << "The confidence level must be in (0, 1), but beta = " << beta;
  beta_ = beta;
}

Scalar QuantileSimulationResultImplementation::getConfidenceLevel() const
{
  return beta_;
}

/* Master seed accessor */
void QuantileSimulationResultImplementation::setMasterSeed(const UnsignedInteger seed)
{
  masterSeed_ = seed;
  hasMasterSeed_ = true;
}

UnsignedInteger QuantileSimulationResultImplementation::getMasterSeed() const
{
  return masterSeed_;
}

Bool QuantileSimulationResultImplementation::hasMasterSeed() const
{
  return hasMasterSeed_;
}

/* Quantile estimate accessor */
void QuantileSimulationResultImplementation::setQuantileEstimate(const Scalar estimate)
{
  if (!std::isfinite(estimate))
    throw InvalidArgumentException(HERE)
        << "The quantile estimate must be finite, but estimate = " << estimate;
  quantileEstimate_ = estimate;
}

Scalar QuantileSimulationResultImplementation::getQuantileEstimate() const
{
  return quantileEstimate_;
}

/* Standard error accessor */
void QuantileSimulationResultImplementation::setStandardError(const Scalar standardError)
{
  if (!std::isfinite(standardError) || !(standardError >= 0.0))
    throw InvalidArgumentException(HERE)
        << "The standard error must be finite and non-negative, but standardError = " << standardError;
  standardError_ = standardError;
}

Scalar QuantileSimulationResultImplementation::getStandardError() const
{
  return standardError_;
}

/* Confidence interval accessor */
void QuantileSimulationResultImplementation::setConfidenceInterval(const Interval & interval)
{
  if (interval.getDimension() != 1)
    throw InvalidArgumentException(HERE)
        << "The confidence interval must be one-dimensional, but its dimension is "
        << interval.getDimension();
  confidenceInterval_ = interval;
}

Interval QuantileSimulationResultImplementation::getConfidenceInterval() const
{
  return confidenceInterval_;
}

/* Estimator type accessor */
void QuantileSimulationResultImplementation::setEstimatorType(const EstimatorType type)
{
  if ((type != EMPIRICAL_INVERSE) && (type != INTERPOLATED))
    throw InvalidArgumentException(HERE) << "Unknown estimator type";
  estimatorType_ = type;
}

QuantileSimulationResultImplementation::EstimatorType QuantileSimulationResultImplementation::getEstimatorType() const
{
  return estimatorType_;
}

/* Achieved coverage accessor */
void QuantileSimulationResultImplementation::setCoverageType(const CoverageType type)
{
  if ((type != COVERAGE_EXACT) && (type != COVERAGE_ASYMPTOTIC)
      && (type != COVERAGE_ANYTIME_VALID) && (type != COVERAGE_NONE))
    throw InvalidArgumentException(HERE) << "Unknown coverage type";
  coverageType_ = type;
}

QuantileSimulationResultImplementation::CoverageType QuantileSimulationResultImplementation::getCoverageType() const
{
  return coverageType_;
}

/* Required coverage accessor */
void QuantileSimulationResultImplementation::setRequiredCoverage(const CoverageType type)
{
  if ((type != COVERAGE_EXACT) && (type != COVERAGE_ASYMPTOTIC)
      && (type != COVERAGE_ANYTIME_VALID) && (type != COVERAGE_NONE))
    throw InvalidArgumentException(HERE) << "Unknown coverage type";
  requiredCoverage_ = type;
}

QuantileSimulationResultImplementation::CoverageType QuantileSimulationResultImplementation::getRequiredCoverage() const
{
  return requiredCoverage_;
}

/* Effective sample size accessor */
void QuantileSimulationResultImplementation::setEffectiveSampleSize(const Scalar ess)
{
  if (!std::isfinite(ess) || !(ess > 0.0))
    throw InvalidArgumentException(HERE)
        << "The effective sample size must be finite and positive, but ess = " << ess;
  effectiveSampleSize_ = ess;
}

Scalar QuantileSimulationResultImplementation::getEffectiveSampleSize() const
{
  return effectiveSampleSize_;
}

/* Sample size accessor */
void QuantileSimulationResultImplementation::setSampleSize(const UnsignedInteger sampleSize)
{
  if (sampleSize == 0)
    throw InvalidArgumentException(HERE) << "The sample size must be positive";
  sampleSize_ = sampleSize;
}

UnsignedInteger QuantileSimulationResultImplementation::getSampleSize() const
{
  return sampleSize_;
}

/* Statistical block length accessor */
void QuantileSimulationResultImplementation::setBlockLength(const UnsignedInteger blockLength)
{
  blockLength_ = blockLength;
}

UnsignedInteger QuantileSimulationResultImplementation::getBlockLength() const
{
  return blockLength_;
}

/* Block count accessor */
void QuantileSimulationResultImplementation::setBlockCount(const UnsignedInteger blockCount)
{
  blockCount_ = blockCount;
}

UnsignedInteger QuantileSimulationResultImplementation::getBlockCount() const
{
  return blockCount_;
}

/* Block quantile sample accessor */
void QuantileSimulationResultImplementation::setBlockQuantileSample(const Point & blockQuantiles)
{
  for (UnsignedInteger i = 0; i < blockQuantiles.getSize(); ++i)
    if (!std::isfinite(blockQuantiles[i]))
      throw InvalidArgumentException(HERE)
          << "The block quantiles must be finite, but blockQuantiles[" << i << "] = " << blockQuantiles[i];
  blockQuantileSample_ = blockQuantiles;
}

Point QuantileSimulationResultImplementation::getBlockQuantileSample() const
{
  return blockQuantileSample_;
}

/* Combination rule accessor */
void QuantileSimulationResultImplementation::setCombinationType(const CombinationType type)
{
  if ((type != MEAN) && (type != MEDIAN) && (type != TRIMMED_MEAN))
    throw InvalidArgumentException(HERE) << "Unknown combination type";
  combinationType_ = type;
}

QuantileSimulationResultImplementation::CombinationType QuantileSimulationResultImplementation::getCombinationType() const
{
  return combinationType_;
}

String QuantileSimulationResultImplementation::getCombinationDescription() const
{
  switch (combinationType_)
  {
    case MEAN:
      return "Mean";
    case MEDIAN:
      return "Median";
    case TRIMMED_MEAN:
      return OSS() << "Trimmed mean (gamma=" << trimmingFraction_ << ")";
    default:
      break;
  }
  return "Unknown";
}

/* Trimming fraction accessor */
void QuantileSimulationResultImplementation::setTrimmingFraction(const Scalar gamma)
{
  if (!(gamma >= 0.0 && gamma < 0.5))
    throw InvalidArgumentException(HERE)
        << "The trimming fraction must be in [0, 0.5), but gamma = " << gamma;
  trimmingFraction_ = gamma;
}

Scalar QuantileSimulationResultImplementation::getTrimmingFraction() const
{
  return trimmingFraction_;
}

/* Block-length policy description accessor */
void QuantileSimulationResultImplementation::setBlockLengthPolicyDescription(const String & description)
{
  blockLengthPolicyDescription_ = description;
}

String QuantileSimulationResultImplementation::getBlockLengthPolicyDescription() const
{
  return blockLengthPolicyDescription_;
}

/* Peak memory accessor */
void QuantileSimulationResultImplementation::setPeakMemory(const UnsignedInteger peakMemory)
{
  peakMemory_ = peakMemory;
}

UnsignedInteger QuantileSimulationResultImplementation::getPeakMemory() const
{
  return peakMemory_;
}

UnsignedInteger QuantileSimulationResultImplementation::getPeakMemoryInBytes() const
{
  return peakMemory_ * sizeof(Scalar);
}

/* Bias estimate accessor */
void QuantileSimulationResultImplementation::setBiasEstimate(const Scalar bias)
{
  if (!std::isfinite(bias))
    throw InvalidArgumentException(HERE)
        << "The bias estimate must be finite, but bias = " << bias;
  biasEstimate_ = bias;
  hasBiasEstimate_ = true;
}

Scalar QuantileSimulationResultImplementation::getBiasEstimate() const
{
  return biasEstimate_;
}

Bool QuantileSimulationResultImplementation::hasBiasEstimate() const
{
  return hasBiasEstimate_;
}

Scalar QuantileSimulationResultImplementation::getBiasToStandardErrorRatio() const
{
  if (!hasBiasEstimate_ || !(standardError_ > 0.0))
    return 0.0;
  return std::abs(biasEstimate_) / standardError_;
}

/* Exact-tail diagnostics accessors */
void QuantileSimulationResultImplementation::setTailType(const TailType type)
{
  if ((type != NO_TAIL) && (type != LOWER_TAIL) && (type != UPPER_TAIL))
    throw InvalidArgumentException(HERE) << "Unknown tail type";
  tailType_ = type;
}

QuantileSimulationResultImplementation::TailType QuantileSimulationResultImplementation::getTailType() const
{
  return tailType_;
}

String QuantileSimulationResultImplementation::getTailDescription() const
{
  switch (tailType_)
  {
    case NO_TAIL:
      return "None";
    case LOWER_TAIL:
      return "Lower tail";
    case UPPER_TAIL:
      return "Upper tail";
    default:
      break;
  }
  return "Unknown";
}

void QuantileSimulationResultImplementation::setRetainedCount(const UnsignedInteger retainedCount)
{
  retainedCount_ = retainedCount;
}

UnsignedInteger QuantileSimulationResultImplementation::getRetainedCount() const
{
  return retainedCount_;
}

void QuantileSimulationResultImplementation::setLowerRank(const UnsignedInteger lowerRank)
{
  lowerRank_ = lowerRank;
}

UnsignedInteger QuantileSimulationResultImplementation::getLowerRank() const
{
  return lowerRank_;
}

void QuantileSimulationResultImplementation::setUpperRank(const UnsignedInteger upperRank)
{
  upperRank_ = upperRank;
}

UnsignedInteger QuantileSimulationResultImplementation::getUpperRank() const
{
  return upperRank_;
}

void QuantileSimulationResultImplementation::setReplicateCount(const UnsignedInteger replicateCount)
{
  if (replicateCount == 0)
    throw InvalidArgumentException(HERE) << "The replicate count must be positive";
  replicateCount_ = replicateCount;
}

UnsignedInteger QuantileSimulationResultImplementation::getReplicateCount() const
{
  return replicateCount_;
}

void QuantileSimulationResultImplementation::setReplicateEstimates(const Point & estimates)
{
  for (UnsignedInteger i = 0; i < estimates.getSize(); ++i)
    if (!std::isfinite(estimates[i]))
      throw InvalidArgumentException(HERE)
          << "The replicate estimates must be finite, but estimates[" << i << "] = " << estimates[i];
  replicateEstimates_ = estimates;
}

Point QuantileSimulationResultImplementation::getReplicateEstimates() const
{
  return replicateEstimates_;
}

/* Two-stage design diagnostics accessors */
void QuantileSimulationResultImplementation::setPilotSize(const UnsignedInteger pilotSize)
{
  if (pilotSize == 0)
    throw InvalidArgumentException(HERE) << "The pilot size must be positive";
  pilotSize_ = pilotSize;
}

UnsignedInteger QuantileSimulationResultImplementation::getPilotSize() const
{
  return pilotSize_;
}

void QuantileSimulationResultImplementation::setPilotQuantileEstimate(const Scalar estimate)
{
  if (!std::isfinite(estimate))
    throw InvalidArgumentException(HERE)
        << "The pilot quantile estimate must be finite, but estimate = " << estimate;
  pilotQuantileEstimate_ = estimate;
  hasPilotQuantileEstimate_ = true;
}

Scalar QuantileSimulationResultImplementation::getPilotQuantileEstimate() const
{
  return pilotQuantileEstimate_;
}

Bool QuantileSimulationResultImplementation::hasPilotQuantileEstimate() const
{
  return hasPilotQuantileEstimate_;
}

void QuantileSimulationResultImplementation::setPilotDensity(const Scalar density)
{
  if (!std::isfinite(density) || !(density >= 0.0))
    throw InvalidArgumentException(HERE)
        << "The pilot density estimate must be finite and non-negative, but density = " << density;
  pilotDensity_ = density;
  hasPilotDensity_ = true;
}

Scalar QuantileSimulationResultImplementation::getPilotDensity() const
{
  return pilotDensity_;
}

Bool QuantileSimulationResultImplementation::hasPilotDensity() const
{
  return hasPilotDensity_;
}

void QuantileSimulationResultImplementation::setConfirmationSize(const UnsignedInteger confirmationSize)
{
  if (confirmationSize == 0)
    throw InvalidArgumentException(HERE) << "The confirmation size must be positive";
  confirmationSize_ = confirmationSize;
}

UnsignedInteger QuantileSimulationResultImplementation::getConfirmationSize() const
{
  return confirmationSize_;
}

void QuantileSimulationResultImplementation::setRequiredConfirmationSize(const UnsignedInteger requiredSize)
{
  if (requiredSize == 0)
    throw InvalidArgumentException(HERE) << "The required confirmation size must be positive";
  requiredConfirmationSize_ = requiredSize;
}

UnsignedInteger QuantileSimulationResultImplementation::getRequiredConfirmationSize() const
{
  return requiredConfirmationSize_;
}

void QuantileSimulationResultImplementation::setTargetHalfWidth(const Scalar halfWidth)
{
  if (!std::isfinite(halfWidth) || !(halfWidth > 0.0))
    throw InvalidArgumentException(HERE)
        << "The target half-width must be finite and positive, but halfWidth = " << halfWidth;
  targetHalfWidth_ = halfWidth;
  hasTargetHalfWidth_ = true;
}

Scalar QuantileSimulationResultImplementation::getTargetHalfWidth() const
{
  return targetHalfWidth_;
}

Bool QuantileSimulationResultImplementation::hasTargetHalfWidth() const
{
  return hasTargetHalfWidth_;
}

void QuantileSimulationResultImplementation::setBudgetExhausted(const Bool exhausted)
{
  budgetExhausted_ = exhausted;
}

Bool QuantileSimulationResultImplementation::getBudgetExhausted() const
{
  return budgetExhausted_;
}

/* Sequential design diagnostics accessors */
void QuantileSimulationResultImplementation::setStoppingTime(const UnsignedInteger stoppingTime)
{
  if (stoppingTime == 0)
    throw InvalidArgumentException(HERE) << "The stopping time must be positive";
  stoppingTime_ = stoppingTime;
}

UnsignedInteger QuantileSimulationResultImplementation::getStoppingTime() const
{
  return stoppingTime_;
}

void QuantileSimulationResultImplementation::setStoppingCriterion(const String & criterion)
{
  if (criterion.empty())
    throw InvalidArgumentException(HERE) << "The stopping criterion must not be empty";
  stoppingCriterion_ = criterion;
}

String QuantileSimulationResultImplementation::getStoppingCriterion() const
{
  return stoppingCriterion_;
}

void QuantileSimulationResultImplementation::setConfidenceSequencePath(const Sample & path)
{
  if ((path.getSize() > 0) && (path.getDimension() != 4))
    throw InvalidArgumentException(HERE)
        << "The confidence sequence path must have four columns (sample size, estimate, lower bound, upper bound), but its dimension is "
        << path.getDimension();
  for (UnsignedInteger i = 0; i < path.getSize(); ++i)
    for (UnsignedInteger j = 0; j < path.getDimension(); ++j)
      if (!std::isfinite(path(i, j)))
        throw InvalidArgumentException(HERE)
            << "The confidence sequence path must be finite, but path(" << i << ", " << j
            << ") = " << path(i, j);
  confidenceSequencePath_ = path;
}

Sample QuantileSimulationResultImplementation::getConfidenceSequencePath() const
{
  return confidenceSequencePath_;
}

void QuantileSimulationResultImplementation::setCoverageAssumption(const String & assumption)
{
  if (assumption.empty())
    throw InvalidArgumentException(HERE) << "The coverage assumption must not be empty";
  coverageAssumption_ = assumption;
}

String QuantileSimulationResultImplementation::getCoverageAssumption() const
{
  return coverageAssumption_;
}

/* Fixed sample flag accessor */
void QuantileSimulationResultImplementation::setIsFixedSample(const Bool fixedSample)
{
  isFixedSample_ = fixedSample;
}

Bool QuantileSimulationResultImplementation::isFixedSample() const
{
  return isFixedSample_;
}

/* Convergence history accessor */
void QuantileSimulationResultImplementation::setConvergenceHistory(const Sample & history)
{
  if ((history.getSize() > 0) && (history.getDimension() < 2))
    throw InvalidArgumentException(HERE)
        << "The convergence history must have at least two columns (sample size, estimate), but its dimension is "
        << history.getDimension();
  convergenceHistory_ = history;
}

Sample QuantileSimulationResultImplementation::getConvergenceHistory() const
{
  return convergenceHistory_;
}

/* Estimator description */
String QuantileSimulationResultImplementation::getEstimatorDescription() const
{
  switch (estimatorType_)
  {
    case EMPIRICAL_INVERSE:
      return "Empirical inverse";
    case INTERPOLATED:
      return "Interpolated";
    default:
      break;
  }
  return "Unknown";
}

/* Coverage description */
String QuantileSimulationResultImplementation::getCoverageDescription() const
{
  switch (coverageType_)
  {
    case COVERAGE_EXACT:
      return "Exact";
    case COVERAGE_ASYMPTOTIC:
      return "Asymptotic";
    case COVERAGE_ANYTIME_VALID:
      return "Anytime-valid";
    case COVERAGE_NONE:
      return "None";
    default:
      break;
  }
  return "Unknown";
}

/* Draw the quantile convergence */
Graph QuantileSimulationResultImplementation::drawQuantileConvergence() const
{
  Graph graph("Quantile convergence", "sample size", "quantile");
  const UnsignedInteger size = convergenceHistory_.getSize();
  if (size == 0)
    return graph;

  Sample estimateSample(size, 2);
  Sample lowerSample(0, 2);
  Sample upperSample(0, 2);
  const Bool hasBounds = convergenceHistory_.getDimension() >= 4;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar sampleSize = convergenceHistory_(i, 0);
    estimateSample(i, 0) = sampleSize;
    estimateSample(i, 1) = convergenceHistory_(i, 1);
    if (hasBounds)
    {
      lowerSample.add(Point({sampleSize, convergenceHistory_(i, 2)}));
      upperSample.add(Point({sampleSize, convergenceHistory_(i, 3)}));
    }
  }

  Curve estimateCurve(estimateSample, "quantile estimate");
  estimateCurve.setLineWidth(2);
  graph.add(estimateCurve);
  if (hasBounds)
  {
    Curve lowerCurve(lowerSample, "confidence bounds");
    graph.add(lowerCurve);
    Curve upperCurve(upperSample);
    upperCurve.setColor(graph.getDrawable(1).getColor());
    graph.add(upperCurve);
  }
  graph.setLegendPosition("topright");
  return graph;
}

/* String converter */
String QuantileSimulationResultImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << QuantileSimulationResultImplementation::GetClassName()
      << " output=" << output_
      << " alpha=" << alpha_
      << " beta=" << beta_
      << " quantileEstimate=" << quantileEstimate_
      << " standardError=" << standardError_
      << " confidenceInterval=" << confidenceInterval_
      << " estimator=" << getEstimatorDescription()
      << " coverage=" << getCoverageDescription()
      << " sampleSize=" << sampleSize_
      << " effectiveSampleSize=" << effectiveSampleSize_
      << " blockLength=" << blockLength_
      << " blockCount=" << blockCount_
      << " combination=" << getCombinationDescription()
      << " blockLengthPolicy=" << blockLengthPolicyDescription_
      << " peakMemory=" << peakMemory_
      << " tail=" << getTailDescription()
      << " retainedCount=" << retainedCount_
      << " lowerRank=" << lowerRank_
      << " upperRank=" << upperRank_
      << " replicateCount=" << replicateCount_
      << " pilotSize=" << pilotSize_
      << " confirmationSize=" << confirmationSize_
      << " requiredConfirmationSize=" << requiredConfirmationSize_
      << " hasTargetHalfWidth=" << hasTargetHalfWidth_
      << " budgetExhausted=" << budgetExhausted_
      << " stoppingTime=" << stoppingTime_
      << " stoppingCriterion=" << stoppingCriterion_
      << " isFixedSample=" << isFixedSample_;
  return oss;
}

String QuantileSimulationResultImplementation::__str__(const String & /*offset*/) const
{
  OSS oss;
  oss << getEstimatorDescription() << " quantile estimate=" << quantileEstimate_
      << " at level " << alpha_
      << " in " << confidenceInterval_
      << " (" << getCoverageDescription() << " coverage)";
  return oss;
}

/* Method save() stores the object through the StorageManager */
void QuantileSimulationResultImplementation::save(Advocate & adv) const
{
  SimulationResult::save(adv);
  adv.saveAttribute("output_", output_);
  adv.saveAttribute("quantileEstimate_", quantileEstimate_);
  adv.saveAttribute("standardError_", standardError_);
  adv.saveAttribute("confidenceInterval_", confidenceInterval_);
  adv.saveAttribute("alpha_", alpha_);
  adv.saveAttribute("beta_", beta_);
  adv.saveAttribute("masterSeed_", masterSeed_);
  adv.saveAttribute("hasMasterSeed_", hasMasterSeed_);
  adv.saveAttribute("estimatorType_", static_cast<UnsignedInteger>(estimatorType_));
  adv.saveAttribute("coverageType_", static_cast<UnsignedInteger>(coverageType_));
  adv.saveAttribute("requiredCoverage_", static_cast<UnsignedInteger>(requiredCoverage_));
  adv.saveAttribute("effectiveSampleSize_", effectiveSampleSize_);
  adv.saveAttribute("sampleSize_", sampleSize_);
  adv.saveAttribute("blockLength_", blockLength_);
  adv.saveAttribute("blockCount_", blockCount_);
  adv.saveAttribute("blockQuantileSample_", blockQuantileSample_);
  adv.saveAttribute("combinationType_", static_cast<UnsignedInteger>(combinationType_));
  adv.saveAttribute("trimmingFraction_", trimmingFraction_);
  adv.saveAttribute("blockLengthPolicyDescription_", blockLengthPolicyDescription_);
  adv.saveAttribute("peakMemory_", peakMemory_);
  adv.saveAttribute("biasEstimate_", biasEstimate_);
  adv.saveAttribute("hasBiasEstimate_", hasBiasEstimate_);
  adv.saveAttribute("tailType_", static_cast<UnsignedInteger>(tailType_));
  adv.saveAttribute("retainedCount_", retainedCount_);
  adv.saveAttribute("lowerRank_", lowerRank_);
  adv.saveAttribute("upperRank_", upperRank_);
  adv.saveAttribute("replicateCount_", replicateCount_);
  adv.saveAttribute("replicateEstimates_", replicateEstimates_);
  adv.saveAttribute("pilotSize_", pilotSize_);
  adv.saveAttribute("pilotQuantileEstimate_", pilotQuantileEstimate_);
  adv.saveAttribute("hasPilotQuantileEstimate_", hasPilotQuantileEstimate_);
  adv.saveAttribute("pilotDensity_", pilotDensity_);
  adv.saveAttribute("hasPilotDensity_", hasPilotDensity_);
  adv.saveAttribute("confirmationSize_", confirmationSize_);
  adv.saveAttribute("requiredConfirmationSize_", requiredConfirmationSize_);
  adv.saveAttribute("targetHalfWidth_", targetHalfWidth_);
  adv.saveAttribute("hasTargetHalfWidth_", hasTargetHalfWidth_);
  adv.saveAttribute("budgetExhausted_", budgetExhausted_);
  adv.saveAttribute("stoppingTime_", stoppingTime_);
  adv.saveAttribute("stoppingCriterion_", stoppingCriterion_);
  adv.saveAttribute("confidenceSequencePath_", confidenceSequencePath_);
  adv.saveAttribute("coverageAssumption_", coverageAssumption_);
  adv.saveAttribute("isFixedSample_", isFixedSample_);
  adv.saveAttribute("convergenceHistory_", convergenceHistory_);
}

/* Method load() reloads the object from the StorageManager */
void QuantileSimulationResultImplementation::load(Advocate & adv)
{
  SimulationResult::load(adv);
  adv.loadAttribute("output_", output_);
  adv.loadAttribute("quantileEstimate_", quantileEstimate_);
  adv.loadAttribute("standardError_", standardError_);
  adv.loadAttribute("confidenceInterval_", confidenceInterval_);
  adv.loadAttribute("alpha_", alpha_);
  adv.loadAttribute("beta_", beta_);
  adv.loadAttribute("masterSeed_", masterSeed_);
  adv.loadAttribute("hasMasterSeed_", hasMasterSeed_);
  UnsignedInteger estimatorType = static_cast<UnsignedInteger>(estimatorType_);
  adv.loadAttribute("estimatorType_", estimatorType);
  estimatorType_ = static_cast<EstimatorType>(estimatorType);
  UnsignedInteger coverageType = static_cast<UnsignedInteger>(coverageType_);
  adv.loadAttribute("coverageType_", coverageType);
  coverageType_ = static_cast<CoverageType>(coverageType);
  UnsignedInteger requiredCoverage = static_cast<UnsignedInteger>(requiredCoverage_);
  adv.loadAttribute("requiredCoverage_", requiredCoverage);
  requiredCoverage_ = static_cast<CoverageType>(requiredCoverage);
  adv.loadAttribute("effectiveSampleSize_", effectiveSampleSize_);
  adv.loadAttribute("sampleSize_", sampleSize_);
  adv.loadAttribute("blockLength_", blockLength_);
  if (adv.hasAttribute("blockCount_"))
    adv.loadAttribute("blockCount_", blockCount_);
  if (adv.hasAttribute("blockQuantileSample_"))
    adv.loadAttribute("blockQuantileSample_", blockQuantileSample_);
  if (adv.hasAttribute("combinationType_"))
  {
    UnsignedInteger combinationType = static_cast<UnsignedInteger>(combinationType_);
    adv.loadAttribute("combinationType_", combinationType);
    combinationType_ = static_cast<CombinationType>(combinationType);
  }
  if (adv.hasAttribute("trimmingFraction_"))
    adv.loadAttribute("trimmingFraction_", trimmingFraction_);
  if (adv.hasAttribute("blockLengthPolicyDescription_"))
    adv.loadAttribute("blockLengthPolicyDescription_", blockLengthPolicyDescription_);
  if (adv.hasAttribute("peakMemory_"))
    adv.loadAttribute("peakMemory_", peakMemory_);
  adv.loadAttribute("biasEstimate_", biasEstimate_);
  adv.loadAttribute("hasBiasEstimate_", hasBiasEstimate_);
  if (adv.hasAttribute("tailType_"))
  {
    UnsignedInteger tailType = static_cast<UnsignedInteger>(tailType_);
    adv.loadAttribute("tailType_", tailType);
    tailType_ = static_cast<TailType>(tailType);
  }
  if (adv.hasAttribute("retainedCount_"))
    adv.loadAttribute("retainedCount_", retainedCount_);
  if (adv.hasAttribute("lowerRank_"))
    adv.loadAttribute("lowerRank_", lowerRank_);
  if (adv.hasAttribute("upperRank_"))
    adv.loadAttribute("upperRank_", upperRank_);
  if (adv.hasAttribute("replicateCount_"))
    adv.loadAttribute("replicateCount_", replicateCount_);
  if (adv.hasAttribute("replicateEstimates_"))
    adv.loadAttribute("replicateEstimates_", replicateEstimates_);
  if (adv.hasAttribute("pilotSize_"))
    adv.loadAttribute("pilotSize_", pilotSize_);
  if (adv.hasAttribute("pilotQuantileEstimate_"))
    adv.loadAttribute("pilotQuantileEstimate_", pilotQuantileEstimate_);
  if (adv.hasAttribute("hasPilotQuantileEstimate_"))
    adv.loadAttribute("hasPilotQuantileEstimate_", hasPilotQuantileEstimate_);
  if (adv.hasAttribute("pilotDensity_"))
    adv.loadAttribute("pilotDensity_", pilotDensity_);
  if (adv.hasAttribute("hasPilotDensity_"))
    adv.loadAttribute("hasPilotDensity_", hasPilotDensity_);
  if (adv.hasAttribute("confirmationSize_"))
    adv.loadAttribute("confirmationSize_", confirmationSize_);
  if (adv.hasAttribute("requiredConfirmationSize_"))
    adv.loadAttribute("requiredConfirmationSize_", requiredConfirmationSize_);
  if (adv.hasAttribute("targetHalfWidth_"))
    adv.loadAttribute("targetHalfWidth_", targetHalfWidth_);
  if (adv.hasAttribute("hasTargetHalfWidth_"))
    adv.loadAttribute("hasTargetHalfWidth_", hasTargetHalfWidth_);
  if (adv.hasAttribute("budgetExhausted_"))
    adv.loadAttribute("budgetExhausted_", budgetExhausted_);
  if (adv.hasAttribute("stoppingTime_"))
    adv.loadAttribute("stoppingTime_", stoppingTime_);
  if (adv.hasAttribute("stoppingCriterion_"))
    adv.loadAttribute("stoppingCriterion_", stoppingCriterion_);
  if (adv.hasAttribute("confidenceSequencePath_"))
    adv.loadAttribute("confidenceSequencePath_", confidenceSequencePath_);
  if (adv.hasAttribute("coverageAssumption_"))
    adv.loadAttribute("coverageAssumption_", coverageAssumption_);
  adv.loadAttribute("isFixedSample_", isFixedSample_);
  adv.loadAttribute("convergenceHistory_", convergenceHistory_);
}

END_NAMESPACE_OPENTURNS
