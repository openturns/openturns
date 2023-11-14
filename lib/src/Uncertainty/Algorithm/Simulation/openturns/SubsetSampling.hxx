//                                               -*- C++ -*-
/**
 *  @brief Subset simulation method
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/WeightedExperiment.hxx"

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
  UnsignedInteger getStepsNumber() const;

  /** Stepwise result accessors */
  Point getThresholdPerStep() const;
  Point getGammaPerStep() const;
  Point getCoefficientOfVariationPerStep() const;
  Point getProbabilityEstimatePerStep() const;

  /** Keep event sample */
  void setKeepSample(const Bool keepSample);

  /** Input/output sample accessor according to select flag */
  enum SelectSample {EVENT0, EVENT1, BOTH};
  Sample getInputSample(const UnsignedInteger step, const UnsignedInteger select = BOTH) const;
  Sample getOutputSample(const UnsignedInteger step, const UnsignedInteger select = BOTH) const;

  /** Experiment for first step */
  void setInitialExperiment(const WeightedExperiment & initialExperiment);
  WeightedExperiment getInitialExperiment() const;

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

  /** Select sample indices according to status */
  Indices getSampleIndices(const UnsignedInteger step, const Bool status) const;

  // some parameters
  Scalar proposalRange_ = 0.0;// width of the proposal pdf
  Scalar conditionalProbability_ = 0.0;// target probability at each subset
  Scalar minimumProbability_ = 0.0;// limit on the smallest probability
  WeightedExperiment initialExperiment_; // experiment for first step

  // some results
  UnsignedInteger numberOfSteps_ = 0;// number of subset steps
  Point thresholdPerStep_;// intermediate thresholds
  Point gammaPerStep_;// intermediate gammas
  Point coefficientOfVariationPerStep_;// intermediate COVS
  Point probabilityEstimatePerStep_;// intermediate PFs

  // attributes used for convenience, not to be saved/loaded
  StandardEvent standardEvent_;// the algorithm happens in U
  UnsignedInteger dimension_;// input dimension
  Sample currentPointSample_;// X
  Sample currentLevelSample_;//f(X)
  UnsignedInteger seedNumber_ = 0;// number of seed points

  // keep samples generated at each step
  Bool keepSample_ = false;
  PersistentCollection<Sample> inputSample_;
  PersistentCollection<Sample> outputSample_;

} ; /* class SubsetSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SUBSETSAMPLING_HXX */
