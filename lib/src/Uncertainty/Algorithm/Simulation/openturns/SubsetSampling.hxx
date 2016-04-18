//                                               -*- C++ -*-
/**
 *  @brief Subset simulation method
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_SUBSETSAMPLING_HXX
#define OPENTURNS_SUBSETSAMPLING_HXX

#include "openturns/Simulation.hxx"
#include "openturns/StandardEvent.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API SubsetSampling
  : public Simulation
{
  CLASSNAME
public:

  /** Default Constructor */
  SubsetSampling();

  /** Constructor with parameters */
  SubsetSampling(const Event & event,
                 const NumericalScalar proposalRange = ResourceMap::GetAsNumericalScalar("SubsetSampling-DefaultProposalRange"),
                 const NumericalScalar targetProbability = ResourceMap::GetAsNumericalScalar("SubsetSampling-DefaultTargetProbability"));

  /** Virtual constructor */
  virtual SubsetSampling * clone() const;

  /** The range of the uniform proposal pdf */
  void setProposalRange(NumericalScalar proposalRange);
  NumericalScalar getProposalRange() const;

  /** Ratio parameter */
  void setTargetProbability(NumericalScalar targetProbability);
  NumericalScalar getTargetProbability() const;

  /** Accessor to the achieved number of steps */
  UnsignedInteger getNumberOfSteps();

  /** Stepwise result accessors */
  NumericalPoint getThresholdPerStep() const;
  NumericalPoint getGammaPerStep() const;
  NumericalPoint getCoefficientOfVariationPerStep() const;
  NumericalPoint getProbabilityEstimatePerStep() const;

  /** Keep event sample */
  void setKeepEventSample(bool keepEventSample);

  /** Event input/output sample accessor */
  NumericalSample getEventInputSample() const;
  NumericalSample getEventOutputSample() const;

  /** i-subset */
  void setISubset(Bool iSubset);
  void setBetaMin(NumericalScalar betaMin);

  /** Performs the actual computation. */
  void run();

  /** String converter */
  String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:
  /** Compute the block sample */
  NumericalSample computeBlockSample();

  /** Compute the new threshold corresponding to the target failure probability */
  NumericalScalar computeThreshold();

  /** compute probability estimate on the current sample */
  NumericalScalar computeProbability(NumericalScalar probabilityEstimate, NumericalScalar threshold);

  /** Sort new seeds */
  void initializeSeed(NumericalScalar threshold);

  /** Compute the correlation on markov chains at the current state of the algorithm */
  NumericalScalar computeVarianceGamma(NumericalScalar currentFailureProbability, NumericalScalar threshold);

  /** Generate new points in the conditional failure domain */
  void generatePoints(NumericalScalar threshold);

  // some parameters
  NumericalScalar proposalRange_;// width of the proposal pdf
  NumericalScalar targetProbability_;// target probability at each subset
  Bool iSubset_;// conditional pre-sampling
  NumericalScalar betaMin_;// pre-sampling hypersphere exclusion radius
  Bool keepEventSample_;// do we keep the event sample ?

  // some results
  UnsignedInteger numberOfSteps_;// number of subset steps
  NumericalPoint thresholdPerStep_;// intermediate thresholds
  NumericalPoint gammaPerStep_;// intermediate gammas
  NumericalPoint coefficientOfVariationPerStep_;// intermediate COVS
  NumericalPoint probabilityEstimatePerStep_;// intermediate PFs
  NumericalSample eventInputSample_;// event input sample
  NumericalSample eventOutputSample_;// event output sample

  // attributes used for conveniency, not to be saved/loaded
  StandardEvent standardEvent_;// the algorithm happens in U
  UnsignedInteger dimension_;// input dimension
  NumericalSample currentPointSample_;// X
  NumericalSample currentLevelSample_;//f(X)

} ; /* class SubsetSampling */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SUBSETSAMPLING_HXX */
