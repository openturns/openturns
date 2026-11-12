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
#ifndef OPENTURNS_SUBSETSAMPLINGQUANTILE_HXX
#define OPENTURNS_SUBSETSAMPLINGQUANTILE_HXX

#include "openturns/QuantileSimulationImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SubsetSamplingQuantile
 *
 * Quantile estimator built on a fixed number of subset-simulation
 * levels.  The conditional probability p0 and the number of levels
 * m = ceil(log(p) / log(p0)) are fixed before the run, where p is the
 * target tail probability.  The estimate is the final conditional
 * order statistic, and the interval is a Student interval over
 * independent replicates.
 *
 * References:
 * S.-K. Au and J. L. Beck. Estimation of small failure probabilities in
 * high dimensions by subset simulation. Probabilistic Engineering
 * Mechanics, 16(4):263-277, 2001.  This article introduces the subset
 * splitting construction and the conditional level schedule reused here.
 * F. Cerou and A. Guyader. Adaptive multilevel splitting for rare event
 * analysis. Stochastic Analysis and Applications, 25(2):417-443, 2007.
 * The article analyses the adaptive level sequence and its conditional
 * sampling kernel.
 * A. Guyader, N. Hengartner and E. Matzner-Lober. Simulation and
 * estimation of extreme quantiles and extreme probabilities. Applied
 * Mathematics & Optimization, 64(2):171-196, 2011.  The article gives
 * the fixed-level quantile estimator and its order-statistic
 * justification.
 */
class OT_API SubsetSamplingQuantile
  : public QuantileSimulationImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  SubsetSamplingQuantile();

  /** Constructor with parameters */
  explicit SubsetSamplingQuantile(const RandomVector & output,
                         const Scalar alpha = ResourceMap::GetAsScalar("QuantileSimulation-DefaultAlpha"),
                         const Scalar beta = ResourceMap::GetAsScalar("QuantileSimulation-DefaultConfidenceLevel"));

  /** Virtual constructor */
  SubsetSamplingQuantile * clone() const override;

  /** Conditional probability accessor */
  void setConditionalProbability(const Scalar conditionalProbability);
  Scalar getConditionalProbability() const;

  /** Proposal range accessor */
  void setProposalRange(const Scalar proposalRange);
  Scalar getProposalRange() const;

  /** Burn-in accessor */
  void setBurnIn(const UnsignedInteger burnIn);
  UnsignedInteger getBurnIn() const;

  /** Replicate count accessor */
  void setReplicateCount(const UnsignedInteger replicateCount);
  UnsignedInteger getReplicateCount() const;

  /** Particle count accessor */
  void setParticleCount(const UnsignedInteger particleCount);
  UnsignedInteger getParticleCount() const;

  /** Sample size accessor */
  void setSampleSize(const UnsignedInteger sampleSize) override;
  UnsignedInteger getSampleSize() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Check the input parameters */
  void checkInputs() const override;

  /** Run a fixed-level subset experiment */
  QuantileSimulationResult runFixedSample() override;

private:
  /** Population size used by the level loop */
  UnsignedInteger resolveParticleCount() const;

  Scalar conditionalProbability_ = ResourceMap::GetAsScalar("SubsetSampling-DefaultConditionalProbability");
  Scalar proposalRange_ = ResourceMap::GetAsScalar("SubsetSampling-DefaultProposalRange");
  UnsignedInteger burnIn_ = ResourceMap::GetAsUnsignedInteger("SubsetSamplingQuantile-DefaultBurnIn");
  UnsignedInteger replicateCount_ = ResourceMap::GetAsUnsignedInteger("SubsetSamplingQuantile-DefaultReplicateCount");
  Bool isSampleSizeProvided_ = false;

}; /* class SubsetSamplingQuantile */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SUBSETSAMPLINGQUANTILE_HXX */
