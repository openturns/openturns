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
#ifndef OPENTURNS_BLOCKQUANTILESIMULATION_HXX
#define OPENTURNS_BLOCKQUANTILESIMULATION_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/WeightedExperiment.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class BlockQuantileSimulation
 *
 * Memory-efficient quantile estimator.  The sample is split into
 * independent blocks, one quantile is computed per block, and the block
 * quantiles are combined with a mean, a median or a trimmed mean.  Only
 * one block and the block quantiles are kept in memory.
 */
class OT_API BlockQuantileSimulation
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Statistical block-length policy */
  enum BlockLengthPolicy
  {
    FIXED,
    SQRT_N,
    ASYMPTOTIC,
    BIAS_TARGET,
    MEMORY_BOUND,
    ADAPTIVE
  };

  /** Default constructor */
  BlockQuantileSimulation();

  /** Constructor with parameters */
  explicit BlockQuantileSimulation(const RandomVector & output,
                                   const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                                   const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Constructor with parameters and experiment */
  BlockQuantileSimulation(const RandomVector & output,
                          const WeightedExperiment & experiment,
                          const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                          const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  BlockQuantileSimulation * clone() const override;

  /** Experiment accessor */
  void setExperiment(const WeightedExperiment & experiment);
  WeightedExperiment getExperiment() const;
  Bool hasExperiment() const;

  /** Sample size accessor */
  void setSampleSize(const UnsignedInteger sampleSize) override;

  /** Statistical block length accessor */
  void setBlockLength(const UnsignedInteger blockLength);
  UnsignedInteger getBlockLength() const;

  /** Block-length policy accessor */
  void setBlockLengthPolicy(const BlockLengthPolicy policy);
  BlockLengthPolicy getBlockLengthPolicy() const;

  /** Combination rule accessor */
  void setCombinationType(const QuantileSimulationResultImplementation::CombinationType type);
  QuantileSimulationResultImplementation::CombinationType getCombinationType() const;

  /** Trimming fraction accessor */
  void setTrimmingFraction(const Scalar gamma);
  Scalar getTrimmingFraction() const;

  /** Minimum block count accessor */
  void setMinimumBlockCount(const UnsignedInteger minimumBlockCount);
  UnsignedInteger getMinimumBlockCount() const;

  /** Memory budget accessor, in scalar values */
  void setMemoryBudget(const UnsignedInteger memoryBudget);
  UnsignedInteger getMemoryBudget() const;

  /** Bias tolerance accessor */
  void setBiasTolerance(const Scalar tolerance);
  Scalar getBiasTolerance() const;

  /** Bootstrap replicate count accessor */
  void setBootstrapSize(const UnsignedInteger bootstrapSize);
  UnsignedInteger getBootstrapSize() const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object through the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check the input parameters */
  void checkInputs() const override;

  /** Run a fixed-sample experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  /** Default block-length policy from the ResourceMap */
  static BlockLengthPolicy getDefaultBlockLengthPolicy();

  /** Draw a block from the experiment */
  Sample drawBlock(WeightedExperiment & experiment,
                   const UnsignedInteger blockLength) const;

  /** Block quantile from one block */
  Scalar blockQuantile(const Sample & block) const;

  /** Combine block quantiles */
  Scalar combine(const Point & blockQuantiles) const;

  /** Resolve the experiment, building a Monte Carlo experiment when needed */
  WeightedExperiment resolveExperiment() const;

  /** Compute the block length for a given policy */
  UnsignedInteger computeBlockLength(const UnsignedInteger sampleSize,
                                     WeightedExperiment & experiment,
                                     Scalar & biasEstimate,
                                     UnsignedInteger & pilotEvaluationCount) const;

  /** Estimate the bias constant from a pilot sample */
  Scalar estimateBiasConstant(const UnsignedInteger sampleSize,
                              WeightedExperiment & experiment,
                              UnsignedInteger & pilotBlockLength,
                              Scalar & pilotDeviation,
                              UnsignedInteger & pilotEvaluationCount) const;

  /** Description of the block-length policy */
  String getBlockLengthPolicyDescription() const;

  /** Bootstrap percentile interval */
  Interval bootstrapInterval(const Point & blockQuantiles,
                             Scalar & estimate,
                             Scalar & standardError) const;

  WeightedExperiment experiment_;
  Bool isExperimentProvided_ = false;
  // 0 means that no fixed length was set: the FIXED policy requires setBlockLength().
  UnsignedInteger blockLength_ = 0;
  // Block length used by the last run, 0 before the first run.  It is kept
  // separate from blockLength_ so that a non-fixed policy never overwrites
  // the user value that a later FIXED policy must reuse.
  UnsignedInteger lastBlockLength_ = 0;
  BlockLengthPolicy blockLengthPolicy_ = getDefaultBlockLengthPolicy();
  QuantileSimulationResultImplementation::CombinationType combinationType_ = QuantileSimulationResultImplementation::MEAN;
  Scalar trimmingFraction_ = ResourceMap::GetAsScalar("QuantileSimulation-DefaultTrimmingFraction");
  UnsignedInteger minimumBlockCount_ = ResourceMap::GetAsUnsignedInteger("QuantileSimulation-MinimumBlockCount");
  // 0 means that no budget was set: the MEMORY_BOUND policy requires setMemoryBudget().
  UnsignedInteger memoryBudget_ = 0;
  Scalar biasTolerance_ = ResourceMap::GetAsScalar("BlockQuantileSimulation-DefaultBiasTolerance");
  UnsignedInteger bootstrapSize_ = ResourceMap::GetAsUnsignedInteger("BlockQuantileSimulation-DefaultBootstrapSize");

}; /* class BlockQuantileSimulation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BLOCKQUANTILESIMULATION_HXX */
