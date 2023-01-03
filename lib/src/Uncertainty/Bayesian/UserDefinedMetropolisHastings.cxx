//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
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

#include "openturns/UserDefinedMetropolisHastings.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(UserDefinedMetropolisHastings)

static const Factory<UserDefinedMetropolisHastings> Factory_UserDefinedMetropolisHastings;

/* Default constructor */
UserDefinedMetropolisHastings::UserDefinedMetropolisHastings()
  : MetropolisHastingsImplementation()
{
  // Nothing to do
}


/* Parameters constructor */
UserDefinedMetropolisHastings::UserDefinedMetropolisHastings(const Distribution & targetDistribution,
    const Point & initialState,
    const Distribution & proposal,
    const Function & linkFunction,
    const Indices & marginalIndices)
  : MetropolisHastingsImplementation(targetDistribution, initialState, marginalIndices)
{
  setProposalAndLinkFunction(proposal, linkFunction);
}

/* Parameters constructor */
UserDefinedMetropolisHastings::UserDefinedMetropolisHastings(const Function & targetLogPDF,
    const Domain & support,
    const Point & initialState,
    const Distribution & proposal,
    const Function & linkFunction,
    const Indices & marginalIndices)
  : MetropolisHastingsImplementation(targetLogPDF, support, initialState, marginalIndices)
{
  setProposalAndLinkFunction(proposal, linkFunction);
}


/* String converter */
String UserDefinedMetropolisHastings::__repr__() const
{
  return OSS() << "class=" << UserDefinedMetropolisHastings::GetClassName()
         << " name=" << getName()
         << " derived from " << MetropolisHastingsImplementation::__repr__()
         << " proposal=" << proposal_;
}


UserDefinedMetropolisHastings* UserDefinedMetropolisHastings::clone() const
{
  return new UserDefinedMetropolisHastings(*this);
}


Point UserDefinedMetropolisHastings::getCandidate() const
{
  // proposal_ distribution conditioned on the currentState_
  proposal_.setParameter(proposalLinkFunction_(currentState_));

  const Point candidate(proposal_.getRealization());

  // LogPDF of the proposal distribution at the candidate point
  // conditional to the currentState_
  const Scalar logProbNew = proposal_.computeLogPDF(candidate);

  // Build potential new state from candidate
  Point candidateState(currentState_);
  for (UnsignedInteger j = 0; j < marginalIndices_.getSize(); ++ j)
    candidateState[marginalIndices_[j]] = candidate[j];

  // proposal_ distribution conditioned on the candidateState
  proposal_.setParameter(proposalLinkFunction_(candidateState));

  // LogPDF of the proposal distribution at the current point
  // conditional to the candidateState
  const Point current(currentState_.select(marginalIndices_));
  const Scalar logProbCurrent = proposal_.computeLogPDF(current);

  setConditionalLogProbabilities(logProbNew, logProbCurrent);
  return candidate;
}

/* Accessors */
Distribution UserDefinedMetropolisHastings::getProposal() const
{
  return proposal_;
}

Function UserDefinedMetropolisHastings::getProposalLinkFunction() const
{
  return proposalLinkFunction_;
}

void UserDefinedMetropolisHastings::setProposalAndLinkFunction(const Distribution & proposal,
    const Function & linkFunction)
{
  if (proposal.getDimension() != marginalIndices_.getSize())
    throw InvalidArgumentException(HERE) << "The proposal density dimension (" << proposal.getDimension()
                                         << ") does not match the block size (" << marginalIndices_.getSize() << ")";

  if (!linkFunction.getEvaluation().getImplementation()->isActualImplementation())
    throw InvalidArgumentException(HERE) << "The link function cannot be evaluated.";

  if (linkFunction.getInputDimension() != initialState_.getDimension())
    throw InvalidDimensionException(HERE) << "The link function input dimension (" << linkFunction.getInputDimension()
                                          << ") does not match the dimension of the state (" << initialState_.getDimension() << ").";

  if (linkFunction.getOutputDimension() != proposal.getParameter().getDimension())
    throw InvalidDimensionException(HERE) << "The link function output dimension (" << linkFunction.getOutputDimension()
                                          << ") does not match the parameter dimension of the proposal distribution (" << proposal.getParameter().getDimension() << ").";
  proposal_ = proposal;
  proposalLinkFunction_ = linkFunction;
}

/* Method save() stores the object through the StorageManager */
void UserDefinedMetropolisHastings::save(Advocate & adv) const
{
  MetropolisHastingsImplementation::save(adv);
  adv.saveAttribute("proposal_", proposal_);
  adv.saveAttribute("proposalLinkFunction_", proposalLinkFunction_);
}

/* Method load() reloads the object from the StorageManager */
void UserDefinedMetropolisHastings::load(Advocate & adv)
{
  MetropolisHastingsImplementation::load(adv);
  adv.loadAttribute("proposal_", proposal_);
  adv.loadAttribute("proposalLinkFunction_", proposalLinkFunction_);
}


END_NAMESPACE_OPENTURNS
