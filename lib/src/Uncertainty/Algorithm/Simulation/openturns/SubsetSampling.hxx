//                                               -*- C++ -*-
/**
 *  @brief Subset simulation method
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SUBSETSAMPLING_HXX
#define OPENTURNS_SUBSETSAMPLING_HXX

#include "openturns/EventSimulation.hxx"
#include "openturns/StandardEvent.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API SubsetSampling
  : public EventSimulation
{
  CLASSNAME
public:

  /** Default Constructor */
  SubsetSampling();

  /** Constructor with parameters */
  SubsetSampling(const RandomVector & event,
                 const Scalar proposalRange = ResourceMap::GetAsScalar("SubsetSampling-DefaultProposalRange"),
                 const Scalar conditionalProbability = ResourceMap::GetAsScalar("SubsetSampling-DefaultConditionalProbability"));

  /** Virtual constructor */
  SubsetSampling * clone() const override;

  /** The range of the uniform proposal pdf */
  void setProposalRange(Scalar proposalRange);
  Scalar getProposalRange() const;

  /** Ratio parameter */
  void setConditionalProbability(Scalar conditionalProbability);
  Scalar getConditionalProbability() const;

  /** Accessor to the minimum probability */
  void setMinimumProbability(const Scalar minimumProbability);
  Scalar getMinimumProbability() const;

  /** Accessor to the achieved number of steps */
  UnsignedInteger getStepsNumber();

  /** Stepwise result accessors */
  Point getThresholdPerStep() const;
  Point getGammaPerStep() const;
  Point getCoefficientOfVariationPerStep() const;
  Point getProbabilityEstimatePerStep() const;

  /** Keep event sample */
  void setKeepEventSample(bool keepEventSample);

  /** Event input/output sample accessor */
  Sample getEventInputSample() const;
  Sample getEventOutputSample() const;

  /** i-subset */
  void setISubset(Bool iSubset);
  void setBetaMin(Scalar betaMin);

  /** Performs the actual computation. */
  void run() override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:
  /** Compute the block sample */
  Sample computeBlockSample() override;

  /** Compute the new threshold corresponding to the target failure probability */
  Scalar computeThreshold();

  /** compute probability estimate on the current sample */
  Scalar computeProbabilityVariance(Scalar probabilityEstimate, Scalar threshold, Scalar & varianceEstimate);

  /** Sort new seeds */
  void initializeSeed(Scalar threshold);

  /** Compute the correlation on markov chains at the current state of the algorithm */
  Scalar computeVarianceGamma(Scalar currentFailureProbability, Scalar threshold);

  /** Generate new points in the conditional failure domain */
  void generatePoints(Scalar threshold);

  // some parameters
  Scalar proposalRange_;// width of the proposal pdf
  Scalar conditionalProbability_;// target probability at each subset
  Bool iSubset_;// conditional pre-sampling
  Scalar betaMin_;// pre-sampling hypersphere exclusion radius
  Bool keepEventSample_;// do we keep the event sample ?
  Scalar minimumProbability_;// limit on the smallest probability

  // some results
  UnsignedInteger numberOfSteps_;// number of subset steps
  Point thresholdPerStep_;// intermediate thresholds
  Point gammaPerStep_;// intermediate gammas
  Point coefficientOfVariationPerStep_;// intermediate COVS
  Point probabilityEstimatePerStep_;// intermediate PFs
  Sample eventInputSample_;// event input sample
  Sample eventOutputSample_;// event output sample

  // attributes used for conveniency, not to be saved/loaded
  StandardEvent standardEvent_;// the algorithm happens in U
  UnsignedInteger dimension_;// input dimension
  Sample currentPointSample_;// X
  Sample currentLevelSample_;//f(X)
  UnsignedInteger seedNumber_;// number of seed points

} ; /* class SubsetSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SUBSETSAMPLING_HXX */
