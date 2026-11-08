//                                               -*- C++ -*-
/**
 *  @brief Handle of quantile simulation results
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
#ifndef OPENTURNS_QUANTILESIMULATIONRESULT_HXX
#define OPENTURNS_QUANTILESIMULATIONRESULT_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/QuantileSimulationResultImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuantileSimulationResult
 *
 * Handle to a quantile simulation result.  The handle preserves the
 * concrete result type through clone(), so a derived result is not
 * sliced when it is returned by an algorithm.
 */
class OT_API QuantileSimulationResult
  : public TypedInterfaceObject<QuantileSimulationResultImplementation>
{
  CLASSNAME
public:
  typedef Pointer<QuantileSimulationResultImplementation> Implementation;

  /** Default constructor */
  QuantileSimulationResult();

  /** Constructor from implementation */
  QuantileSimulationResult(const QuantileSimulationResultImplementation & result);

  /** Constructor from implementation pointer */
  QuantileSimulationResult(const Implementation & p_implementation);

  /** Output random vector accessor */
  void setOutput(const RandomVector & output);
  RandomVector getOutput() const;

  /** Quantile level accessor */
  void setQuantileLevel(const Scalar alpha);
  Scalar getQuantileLevel() const;

  /** Confidence level accessor */
  void setConfidenceLevel(const Scalar beta);
  Scalar getConfidenceLevel() const;

  /** Master seed accessor */
  void setMasterSeed(const UnsignedInteger seed);
  UnsignedInteger getMasterSeed() const;
  Bool hasMasterSeed() const;

  /** Quantile estimate accessor */
  void setQuantileEstimate(const Scalar estimate);
  Scalar getQuantileEstimate() const;

  /** Standard error accessor */
  void setStandardError(const Scalar standardError);
  Scalar getStandardError() const;

  /** Confidence interval accessor */
  void setConfidenceInterval(const Interval & interval);
  Interval getConfidenceInterval() const;

  /** Estimator type accessor */
  void setEstimatorType(const QuantileSimulationResultImplementation::EstimatorType type);
  QuantileSimulationResultImplementation::EstimatorType getEstimatorType() const;

  /** Achieved coverage accessor */
  void setCoverageType(const QuantileSimulationResultImplementation::CoverageType type);
  QuantileSimulationResultImplementation::CoverageType getCoverageType() const;

  /** Required coverage accessor */
  void setRequiredCoverage(const QuantileSimulationResultImplementation::CoverageType type);
  QuantileSimulationResultImplementation::CoverageType getRequiredCoverage() const;

  /** Effective sample size accessor */
  void setEffectiveSampleSize(const Scalar ess);
  Scalar getEffectiveSampleSize() const;

  /** Sample size accessor */
  void setSampleSize(const UnsignedInteger sampleSize);
  UnsignedInteger getSampleSize() const;

  /** Statistical block length accessor */
  void setBlockLength(const UnsignedInteger blockLength);
  UnsignedInteger getBlockLength() const;

  /** Block count accessor */
  void setBlockCount(const UnsignedInteger blockCount);
  UnsignedInteger getBlockCount() const;

  /** Block quantile sample accessor */
  void setBlockQuantileSample(const Point & blockQuantiles);
  Point getBlockQuantileSample() const;

  /** Combination rule accessor */
  void setCombinationType(const QuantileSimulationResultImplementation::CombinationType type);
  QuantileSimulationResultImplementation::CombinationType getCombinationType() const;
  String getCombinationDescription() const;

  /** Trimming fraction accessor */
  void setTrimmingFraction(const Scalar gamma);
  Scalar getTrimmingFraction() const;

  /** Block-length policy description accessor */
  void setBlockLengthPolicyDescription(const String & description);
  String getBlockLengthPolicyDescription() const;

  /** Peak memory accessor, in scalar values */
  void setPeakMemory(const UnsignedInteger peakMemory);
  UnsignedInteger getPeakMemory() const;
  UnsignedInteger getPeakMemoryInBytes() const;

  /** Bias estimate accessor */
  void setBiasEstimate(const Scalar bias);
  Scalar getBiasEstimate() const;
  Bool hasBiasEstimate() const;
  Scalar getBiasToStandardErrorRatio() const;

  /** Exact-tail diagnostics accessors */
  void setTailType(const QuantileSimulationResultImplementation::TailType type);
  QuantileSimulationResultImplementation::TailType getTailType() const;
  String getTailDescription() const;
  void setRetainedCount(const UnsignedInteger retainedCount);
  UnsignedInteger getRetainedCount() const;
  void setLowerRank(const UnsignedInteger lowerRank);
  UnsignedInteger getLowerRank() const;
  void setUpperRank(const UnsignedInteger upperRank);
  UnsignedInteger getUpperRank() const;

  /** Threshold-scheme diagnostics accessors */
  void setThresholdSequence(const Point & thresholds);
  Point getThresholdSequence() const;
  void setLevelProbabilitySequence(const Point & probabilities);
  Point getLevelProbabilitySequence() const;
  void setStepCount(const UnsignedInteger stepCount);
  UnsignedInteger getStepCount() const;
  void setAcceptanceRate(const Scalar acceptanceRate);
  Scalar getAcceptanceRate() const;
  void setPoissonRanks(const UnsignedInteger lowerRank, const UnsignedInteger upperRank);
  UnsignedInteger getPoissonRankLower() const;
  UnsignedInteger getPoissonRankUpper() const;
  Bool hasPoissonRanks() const;
  void setParticleCount(const UnsignedInteger particleCount);
  UnsignedInteger getParticleCount() const;
  void setReplicateCount(const UnsignedInteger replicateCount);
  UnsignedInteger getReplicateCount() const;
  void setReplicateEstimates(const Point & estimates);
  Point getReplicateEstimates() const;

  /** Cross-entropy diagnostics accessors */
  void setAuxiliaryDistribution(const Distribution & distribution);
  Distribution getAuxiliaryDistribution() const;
  Bool hasAuxiliaryDistribution() const;
  void setIntermediateQuantileLevel(const Scalar level);
  Scalar getIntermediateQuantileLevel() const;
  void setConditionalProbability(const Scalar probability);
  Scalar getConditionalProbability() const;

  /** Root-finding diagnostics accessors */
  void setDirectionCount(const UnsignedInteger directionCount);
  UnsignedInteger getDirectionCount() const;
  void setConditionalProbabilitySample(const Point & probabilities);
  Point getConditionalProbabilitySample() const;
  void setImportantDirection(const Point & direction);
  Point getImportantDirection() const;
  Bool hasImportantDirection() const;
  void setRootBracket(const Interval & bracket);
  Interval getRootBracket() const;
  Bool hasRootBracket() const;
  void setRootIterationCount(const UnsignedInteger iterationCount);
  UnsignedInteger getRootIterationCount() const;
  void setProbabilityEvaluationCount(const UnsignedInteger evaluationCount);
  UnsignedInteger getProbabilityEvaluationCount() const;
  void setRootSolverType(const QuantileSimulationResultImplementation::RootSolverType type);
  QuantileSimulationResultImplementation::RootSolverType getRootSolverType() const;
  String getRootSolverDescription() const;
  void setCommonRandomNumbers(const Bool commonRandomNumbers);
  Bool getCommonRandomNumbers() const;
  void setRandomStreamCapability(const Bool capability);
  Bool getRandomStreamCapability() const;
  void setDelegatedToDirectExtraction(const Bool delegated);
  Bool getDelegatedToDirectExtraction() const;
  void setWrappedAlgorithmDescription(const String & description);
  String getWrappedAlgorithmDescription() const;
  Bool hasWrappedAlgorithmDescription() const;

  /** Post-analytical diagnostics accessors */
  void setAnalyticalProbability(const Scalar probability);
  Scalar getAnalyticalProbability() const;
  Bool hasAnalyticalProbability() const;
  void setDesignPoint(const Point & designPoint);
  Point getDesignPoint() const;
  Bool hasDesignPoint() const;
  void setReliabilityIndex(const Scalar index);
  Scalar getReliabilityIndex() const;
  Bool hasReliabilityIndex() const;
  void setCurvatureCorrection(const Scalar correction);
  Scalar getCurvatureCorrection() const;
  Bool hasCurvatureCorrection() const;
  void setRefinedProbability(const Scalar probability);
  Scalar getRefinedProbability() const;
  Bool hasRefinedProbability() const;
  void setRefinementStandardError(const Scalar standardError);
  Scalar getRefinementStandardError() const;

  /** Two-stage design diagnostics accessors */
  void setPilotSize(const UnsignedInteger pilotSize);
  UnsignedInteger getPilotSize() const;
  void setPilotQuantileEstimate(const Scalar estimate);
  Scalar getPilotQuantileEstimate() const;
  Bool hasPilotQuantileEstimate() const;
  void setPilotDensity(const Scalar density);
  Scalar getPilotDensity() const;
  Bool hasPilotDensity() const;
  void setConfirmationSize(const UnsignedInteger confirmationSize);
  UnsignedInteger getConfirmationSize() const;
  void setRequiredConfirmationSize(const UnsignedInteger requiredSize);
  UnsignedInteger getRequiredConfirmationSize() const;
  void setTargetHalfWidth(const Scalar halfWidth);
  Scalar getTargetHalfWidth() const;
  Bool hasTargetHalfWidth() const;
  void setBudgetExhausted(const Bool exhausted);
  Bool getBudgetExhausted() const;

  /** Sequential design diagnostics accessors */
  void setStoppingTime(const UnsignedInteger stoppingTime);
  UnsignedInteger getStoppingTime() const;
  void setStoppingCriterion(const String & criterion);
  String getStoppingCriterion() const;
  void setConfidenceSequencePath(const Sample & path);
  Sample getConfidenceSequencePath() const;
  void setCoverageAssumption(const String & assumption);
  String getCoverageAssumption() const;

  /** Fixed sample flag accessor */
  void setIsFixedSample(const Bool fixedSample);
  Bool isFixedSample() const;

  /** Convergence history accessor */
  void setConvergenceHistory(const Sample & history);
  Sample getConvergenceHistory() const;

  /** Outer sampling accessor */
  void setOuterSampling(const UnsignedInteger outerSampling);
  UnsignedInteger getOuterSampling() const;

  /** Evaluation block size accessor */
  void setBlockSize(const UnsignedInteger blockSize);
  UnsignedInteger getBlockSize() const;

  /** Elapsed time accessor */
  void setTimeDuration(const Scalar time);
  Scalar getTimeDuration() const;

  /** Estimator and coverage descriptions */
  String getEstimatorDescription() const;
  String getCoverageDescription() const;

  /** Draw the quantile convergence */
  Graph drawQuantileConvergence() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

}; /* class QuantileSimulationResult */

typedef Collection<QuantileSimulationResult> QuantileSimulationResultCollection;

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILESIMULATIONRESULT_HXX */
