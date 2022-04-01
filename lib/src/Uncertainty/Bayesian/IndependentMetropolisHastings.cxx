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

#include "openturns/IndependentMetropolisHastings.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(IndependentMetropolisHastings)

static const Factory<IndependentMetropolisHastings> Factory_IndependentMetropolisHastings;

/* Default constructor */
IndependentMetropolisHastings::IndependentMetropolisHastings()
  : MetropolisHastingsImplementation()
{
  // Nothing to do
}


/* Parameters constructor */
IndependentMetropolisHastings::IndependentMetropolisHastings(const Distribution & targetDistribution,
                                                          const Point & initialState,
                                                          const Distribution & proposal,
                                                           const Indices & marginalIndices)
  : MetropolisHastingsImplementation(targetDistribution, initialState, marginalIndices)
{
  setProposal(proposal);
}

/* Parameters constructor */
IndependentMetropolisHastings::IndependentMetropolisHastings(const Function & targetLogPDF,
                                                          const Domain & support,
                                                          const Point & initialState,
                                                          const Distribution & proposal,
                                                           const Indices & marginalIndices)
  : MetropolisHastingsImplementation(targetLogPDF, support, initialState, marginalIndices)
{
  setProposal(proposal);
}


/* String converter */
String IndependentMetropolisHastings::__repr__() const
{
  return OSS() << "class=" << IndependentMetropolisHastings::GetClassName()
         << " name=" << getName()
         << " derived from " << MetropolisHastingsImplementation::__repr__()
         << " proposal=" << proposal_;
}


IndependentMetropolisHastings* IndependentMetropolisHastings::clone() const
{
  return new IndependentMetropolisHastings(*this);
}


Point IndependentMetropolisHastings::getCandidate() const
{
  const Point candidate(proposal_.getRealization());

  // LogPDF of the proposal distribution at the candidate point
  const Scalar logProbNew = proposal_.computeLogPDF(candidate);

  // LogPDF of the proposal distribution at the current point
  const UnsignedInteger proposalDimension = proposal_.getDimension();
const Point current(currentState_.select(marginalIndices_));
  const Scalar logProbCurrent = proposal_.computeLogPDF(current);

  setConditionalLogProbabilities(logProbNew, logProbCurrent);
  return candidate;
}


void IndependentMetropolisHastings::setProposal(const Distribution & proposal)
{
  if (proposal.getDimension() != marginalIndices_.getSize())
    throw InvalidArgumentException(HERE) << "The proposal density dimension (" << proposal.getDimension()
                                         << ") does not match the block size (" << marginalIndices_.getSize() << ")";
  proposal_ = proposal;
}


Distribution IndependentMetropolisHastings::getProposal() const
{
  return proposal_;
}



/* Method save() stores the object through the StorageManager */
void IndependentMetropolisHastings::save(Advocate & adv) const
{
  MetropolisHastingsImplementation::save(adv);
  adv.saveAttribute("proposal_", proposal_);
}

/* Method load() reloads the object from the StorageManager */
void IndependentMetropolisHastings::load(Advocate & adv)
{
  MetropolisHastingsImplementation::load(adv);
  adv.loadAttribute("proposal_", proposal_);
}


END_NAMESPACE_OPENTURNS
