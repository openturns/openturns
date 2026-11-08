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
#ifndef OPENTURNS_QUANTILESIMULATIONRESULTIMPLEMENTATION_HXX
#define OPENTURNS_QUANTILESIMULATIONRESULTIMPLEMENTATION_HXX

#include "openturns/SimulationResult.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/Interval.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Graph.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class QuantileSimulationResultImplementation
 *
 * The class stores the statistical contract of a quantile simulation:
 * the estimated quantile, its standard error, the confidence interval,
 * the estimator and coverage conventions, and the diagnostics that are
 * common to every quantile algorithm.
 */
class OT_API QuantileSimulationResultImplementation
  : public SimulationResult
{
  CLASSNAME
public:
  /** Estimator convention */
  enum EstimatorType
  {
    EMPIRICAL_INVERSE,
    INTERPOLATED
  };

  /** Coverage guarantee */
  enum CoverageType
  {
    COVERAGE_EXACT,
    COVERAGE_ASYMPTOTIC,
    COVERAGE_ANYTIME_VALID,
    COVERAGE_NONE
  };

  /** Combination rule used by block algorithms */
  enum CombinationType
  {
    MEAN,
    MEDIAN,
    TRIMMED_MEAN
  };

  /** Retained tail used by exact-tail algorithms */
  enum TailType
  {
    NO_TAIL,
    LOWER_TAIL,
    UPPER_TAIL
  };

  /** Default constructor */
  QuantileSimulationResultImplementation();

  /** Standard constructor */
  explicit QuantileSimulationResultImplementation(const RandomVector & output,
                                                  const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                                                  const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  QuantileSimulationResultImplementation * clone() const override;

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
  void setEstimatorType(const EstimatorType type);
  EstimatorType getEstimatorType() const;

  /** Achieved coverage accessor */
  void setCoverageType(const CoverageType type);
  CoverageType getCoverageType() const;

  /** Required coverage accessor */
  void setRequiredCoverage(const CoverageType type);
  CoverageType getRequiredCoverage() const;

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
  void setCombinationType(const CombinationType type);
  CombinationType getCombinationType() const;
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
  void setTailType(const TailType type);
  TailType getTailType() const;
  String getTailDescription() const;
  void setRetainedCount(const UnsignedInteger retainedCount);
  UnsignedInteger getRetainedCount() const;
  void setLowerRank(const UnsignedInteger lowerRank);
  UnsignedInteger getLowerRank() const;
  void setUpperRank(const UnsignedInteger upperRank);
  UnsignedInteger getUpperRank() const;

  /** Fixed sample flag accessor */
  void setIsFixedSample(const Bool fixedSample);
  Bool isFixedSample() const;

  /** Convergence history accessor */
  void setConvergenceHistory(const Sample & history);
  Sample getConvergenceHistory() const;

  /** Estimator and coverage descriptions */
  String getEstimatorDescription() const;
  String getCoverageDescription() const;

  /** Draw the quantile convergence */
  Graph drawQuantileConvergence() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  RandomVector output_;
  Scalar quantileEstimate_ = 0.0;
  Scalar standardError_ = 0.0;
  Interval confidenceInterval_;
  Scalar alpha_ = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha");
  Scalar beta_ = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel");
  UnsignedInteger masterSeed_ = 0;
  Bool hasMasterSeed_ = false;
  EstimatorType estimatorType_ = EMPIRICAL_INVERSE;
  CoverageType coverageType_ = COVERAGE_NONE;
  CoverageType requiredCoverage_ = COVERAGE_NONE;
  Scalar effectiveSampleSize_ = 0.0;
  UnsignedInteger sampleSize_ = ResourceMap::GetAsUnsignedInteger("QuantileSimulation-DefaultSampleSize");
  UnsignedInteger blockLength_ = 0;
  UnsignedInteger blockCount_ = 0;
  Point blockQuantileSample_;
  CombinationType combinationType_ = MEAN;
  Scalar trimmingFraction_ = ResourceMap::GetAsScalar("QuantileSimulation-DefaultTrimmingFraction");
  String blockLengthPolicyDescription_;
  UnsignedInteger peakMemory_ = 0;
  Scalar biasEstimate_ = 0.0;
  Bool hasBiasEstimate_ = false;
  TailType tailType_ = NO_TAIL;
  UnsignedInteger retainedCount_ = 0;
  UnsignedInteger lowerRank_ = 0;
  UnsignedInteger upperRank_ = 0;
  Bool isFixedSample_ = true;
  Sample convergenceHistory_;

}; /* class QuantileSimulationResultImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_QUANTILESIMULATIONRESULTIMPLEMENTATION_HXX */
