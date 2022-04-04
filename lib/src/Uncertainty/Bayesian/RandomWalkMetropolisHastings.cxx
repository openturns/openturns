//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/RandomWalkMetropolisHastings.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(RandomWalkMetropolisHastings)

static const Factory<RandomWalkMetropolisHastings> Factory_RandomWalkMetropolisHastings;

/* Default constructor */
RandomWalkMetropolisHastings::RandomWalkMetropolisHastings()
  : MetropolisHastingsImplementation()
{
  // Nothing to do
}


/* Parameters constructor */
RandomWalkMetropolisHastings::RandomWalkMetropolisHastings(const Distribution & targetDistribution,
                                                          const Point & initialState,
                                                          const Distribution & proposal,
                                                           const Indices & marginalIndices)
  : MetropolisHastingsImplementation(targetDistribution, initialState, marginalIndices)
  , adaptationRange_(Interval(ResourceMap::GetAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationLowerBound"), ResourceMap::GetAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationUpperBound")))
  , adaptationExpansionFactor_(ResourceMap::GetAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationExpansionFactor"))
  , adaptationShrinkFactor_(ResourceMap::GetAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationShrinkFactor"))
  , adaptationPeriod_(ResourceMap::GetAsUnsignedInteger("RandomWalkMetropolisHastings-DefaultAdaptationPeriod"))
{
  setProposal(proposal);
}

/* Parameters constructor */
RandomWalkMetropolisHastings::RandomWalkMetropolisHastings(const Function & targetLogPDF,
                                                          const Domain & support,
                                                          const Point & initialState,
                                                          const Distribution & proposal,
                                                           const Indices & marginalIndices)
  : MetropolisHastingsImplementation(targetLogPDF, support, initialState, marginalIndices)
  , adaptationRange_(Interval(ResourceMap::GetAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationLowerBound"), ResourceMap::GetAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationUpperBound")))
  , adaptationExpansionFactor_(ResourceMap::GetAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationExpansionFactor"))
  , adaptationShrinkFactor_(ResourceMap::GetAsScalar("RandomWalkMetropolisHastings-DefaultAdaptationShrinkFactor"))
  , adaptationPeriod_(ResourceMap::GetAsUnsignedInteger("RandomWalkMetropolisHastings-DefaultAdaptationPeriod"))
{
  setProposal(proposal);
}


/* String converter */
String RandomWalkMetropolisHastings::__repr__() const
{
  return OSS() << "class=" << RandomWalkMetropolisHastings::GetClassName()
         << " name=" << getName()
         << " derived from " << MetropolisHastingsImplementation::__repr__()
         << " proposal=" << proposal_;
}


RandomWalkMetropolisHastings* RandomWalkMetropolisHastings::clone() const
{
  return new RandomWalkMetropolisHastings(*this);
}


Point RandomWalkMetropolisHastings::getCandidate() const
{
  // re-adapt if necessary
  if ((samplesNumber_ < getBurnIn()) && ((samplesNumber_ % adaptationPeriod_) == (adaptationPeriod_ - 1)))
  {
    // compute the current acceptation rate
    Scalar rho = 1.0 * acceptedNumberAdaptation_ / (1.0 * adaptationPeriod_);

    if (rho < adaptationRange_.getLowerBound()[0])
      // if the acceptance rate it too low, make smaller steps
      adaptationFactor_ *= adaptationShrinkFactor_;
    else if (rho > adaptationRange_.getUpperBound()[0])
      // if the acceptance rate is too high, make larger steps
      adaptationFactor_ *= adaptationExpansionFactor_;

    // reset the counter
    acceptedNumberAdaptation_ = 0;

    if (getVerbose())
      LOGTRACE(OSS() << "rho=" << rho << " delta=" << adaptationFactor_);
  }

  Point prop(proposal_.getRealization());
  if(!isProposalSymmetric_)
    setConditionalLogProbabilities(proposal_.computeLogPDF(prop), proposal_.computeLogPDF(-1.0 * prop));
  return currentState_.select(marginalIndices_) + adaptationFactor_ * prop;
}


void RandomWalkMetropolisHastings::setProposal(const Distribution & proposal)
{
  if (proposal.getDimension() != marginalIndices_.getSize())
    throw InvalidArgumentException(HERE) << "The proposal density dimension (" << proposal.getDimension()
                                         << ") does not match the block size (" << marginalIndices_.getSize() << ")";
  proposal_ = proposal;

  // In the following, we try do determine whether the proposal distribution is symmetric.
  // If the proposal distribution has an independent copula, we test a sufficient and necessary condition.
  // Otherwise, we test a condition that is only sufficient.

  // Necessary condition for symmetry: the distribution has zero mean.
  isProposalSymmetric_ = proposal.getMean().norm() < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon");
  if (proposal.hasIndependentCopula())
  {
    for (UnsignedInteger j = 0; j < proposal.getDimension(); ++ j)
    {
      if (!isProposalSymmetric_) break;
      isProposalSymmetric_ = isProposalSymmetric_ && proposal.getMarginal(j).isElliptical();
    }
  } // independent copula: zero-mean marginals are elliptical
  else
  {
    isProposalSymmetric_ = isProposalSymmetric_ && proposal.isElliptical();
  } // dependent copula: the whole zero-mean distribution is elliptical
}


Distribution RandomWalkMetropolisHastings::getProposal() const
{
  return proposal_;
}


void RandomWalkMetropolisHastings::setAdaptationRange(const Interval & adaptationRange)
{
  if (adaptationRange.getDimension() != 1)
    throw InvalidDimensionException(HERE) << "Range should be 1-d. Got " << adaptationRange.getDimension();
  adaptationRange_ = adaptationRange;
}

Interval RandomWalkMetropolisHastings::getAdaptationRange() const
{
  return adaptationRange_;
}


void RandomWalkMetropolisHastings::setAdaptationExpansionFactor(const Scalar adaptationExpansionFactor)
{
  if (!(adaptationExpansionFactor > 1.0))
    throw InvalidArgumentException(HERE) << "Expansion factor should be > 1. Got " << adaptationExpansionFactor;
  adaptationExpansionFactor_ = adaptationExpansionFactor;
}

Scalar RandomWalkMetropolisHastings::getAdaptationExpansionFactor() const
{
  return adaptationExpansionFactor_;
}


void RandomWalkMetropolisHastings::setAdaptationShrinkFactor(const Scalar adaptationShrinkFactor)
{
  if (!(adaptationShrinkFactor > 0.0) || !(adaptationShrinkFactor < 1.0))
    throw InvalidArgumentException(HERE) << "Shrink factor should be in (0, 1). Got " << adaptationShrinkFactor;
  adaptationShrinkFactor_ = adaptationShrinkFactor;
}

Scalar RandomWalkMetropolisHastings::getAdaptationShrinkFactor() const
{
  return adaptationShrinkFactor_;
}

void RandomWalkMetropolisHastings::setAdaptationPeriod(const UnsignedInteger adaptationPeriod)
{
  if (!adaptationPeriod)
    throw InvalidArgumentException(HERE) << "The adaptation period must be positive.";
  adaptationPeriod_ = adaptationPeriod;
}


UnsignedInteger RandomWalkMetropolisHastings::getAdaptationPeriod() const
{
  return adaptationPeriod_;
}

Scalar RandomWalkMetropolisHastings::getAdaptationFactor() const
{
  return adaptationFactor_;
}

/* Method save() stores the object through the StorageManager */
void RandomWalkMetropolisHastings::save(Advocate & adv) const
{
  MetropolisHastingsImplementation::save(adv);
  adv.saveAttribute("proposal_", proposal_);
  adv.saveAttribute("isProposalSymmetric_", isProposalSymmetric_);
  adv.saveAttribute("adaptationFactor_", adaptationFactor_);
  adv.saveAttribute("adaptationRange_", adaptationRange_);
  adv.saveAttribute("adaptationExpansionFactor_", adaptationExpansionFactor_);
  adv.saveAttribute("adaptationShrinkFactor_", adaptationShrinkFactor_);
  adv.saveAttribute("adaptationPeriod_", adaptationPeriod_);
}

/* Method load() reloads the object from the StorageManager */
void RandomWalkMetropolisHastings::load(Advocate & adv)
{
  MetropolisHastingsImplementation::load(adv);
  adv.loadAttribute("proposal_", proposal_);
  adv.loadAttribute("isProposalSymmetric_", isProposalSymmetric_);
  adv.loadAttribute("adaptationFactor_", adaptationFactor_);
  adv.loadAttribute("adaptationRange_", adaptationRange_);
  adv.loadAttribute("adaptationExpansionFactor_", adaptationExpansionFactor_);
  adv.loadAttribute("adaptationShrinkFactor_", adaptationShrinkFactor_);
  adv.loadAttribute("adaptationPeriod_", adaptationPeriod_);
}


END_NAMESPACE_OPENTURNS
