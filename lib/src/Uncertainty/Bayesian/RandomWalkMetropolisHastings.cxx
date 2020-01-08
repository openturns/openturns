//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/RandomGenerator.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ConditionalDistribution.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<CalibrationStrategy>)
static const Factory<PersistentCollection<CalibrationStrategy> > Factory_PersistentCollection_CalibrationStrategy;


CLASSNAMEINIT(RandomWalkMetropolisHastings)

static const Factory<RandomWalkMetropolisHastings> Factory_RandomWalkMetropolisHastings;

/* Default constructor */
RandomWalkMetropolisHastings::RandomWalkMetropolisHastings()
  : MCMC()
  , calibrationStrategy_(0)
  , samplesNumber_(0)
  , acceptedNumber_(0)
  , currentLogLikelihood_(0.)
{
  // Nthing to do
}


/* Parameters constructor */
RandomWalkMetropolisHastings::RandomWalkMetropolisHastings( const Distribution & prior,
    const Distribution & conditional,
    const Sample & observations,
    const Point & initialState,
    const DistributionCollection & proposal)
  : MCMC(prior, conditional, observations, initialState)
  , calibrationStrategy_(proposal.getSize())
  , samplesNumber_(0)
  , acceptedNumber_(initialState.getDimension())
  , currentLogLikelihood_(0.)
{
  setProposal(proposal);
}


/* Parameters constructor */
RandomWalkMetropolisHastings::RandomWalkMetropolisHastings( const Distribution & prior,
    const Distribution & conditional,
    const Function & model,
    const Sample & parameters,
    const Sample & observations,
    const Point & initialState,
    const DistributionCollection & proposal)
  : MCMC(prior, conditional, model, parameters, observations, initialState)
  , calibrationStrategy_(proposal.getSize())
  , samplesNumber_(0)
  , acceptedNumber_(initialState.getDimension())
  , currentLogLikelihood_(0.)
{
  setProposal(proposal);
}


/* String converter */
String RandomWalkMetropolisHastings::__repr__() const
{
  return OSS() << "class=" << RandomWalkMetropolisHastings::GetClassName()
         << " name=" << getName()
         << " derived from " << MCMC::__repr__()
         << " proposal=" << proposal_
         << " calibrationStrategy=" << calibrationStrategy_;
}


RandomWalkMetropolisHastings* RandomWalkMetropolisHastings::clone() const
{
  return new RandomWalkMetropolisHastings(*this);
}

/* Here is the interface that all derived class must implement */

Point RandomWalkMetropolisHastings::getRealization() const
{
  const UnsignedInteger dimension = initialState_.getDimension();

  // update factor
  Point delta(dimension, 1.0);

  // number of samples accepted until calibration step
  Indices accepted(dimension);

  // perform burning if necessary
  const UnsignedInteger size = getThinning() + ((samplesNumber_ < getBurnIn()) ? getBurnIn() : 0);

  // check the first likelihood
  if (samplesNumber_ == 0)
  {
    currentLogLikelihood_ = computeLogLikelihood(currentState_);
    if (currentLogLikelihood_ <= SpecFunc::LogMinScalar)
      throw InvalidArgumentException(HERE) << "The likelihood of the initial state should be positive";
  }

  // for each new sample
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    // accumulates the updates over each components
    Point newState(currentState_);

    history_.store(currentState_);

    Scalar logLikelihoodCandidate = currentLogLikelihood_;

    // update each chain component
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      // new candidate for the j-th component
      Point nextState(newState);

      Bool nonRejectedComponent = nonRejectedComponents_.contains(j);
      if (!nonRejectedComponent)
      {
        // regular MCMC
        nextState[j] += delta[j] * proposal_[j].getRealization()[0];
      }
      else
      {
        // non-rejected component
        nextState[j] = getPrior().getMarginal(j).getRealization()[0];
      }

      const Scalar nextLogLikelihood = computeLogLikelihood(nextState);

      // alpha = likehood(newstate)/likehood(oldstate)
      const Scalar alphaLog = nextLogLikelihood  - currentLogLikelihood_;

      // acceptance test
      const Scalar uLog = log(RandomGenerator::Generate());
      if (nonRejectedComponent || (uLog < alphaLog))
      {
        // the likelihood can be 0 wrt the observations because of a non-rejected component (always ok wrt prior)
        if (nextLogLikelihood == SpecFunc::LogMinScalar)
          throw InternalException(HERE) << "Cannot update the (non-accepted) component #" << j << " with null likelihood wrt observations";

        logLikelihoodCandidate = nextLogLikelihood;
        ++ acceptedNumber_[j];
        ++ accepted[j];

        newState[j] = nextState[j];
      }
    }

    // reuse the likelihood of the last accepted update
    currentLogLikelihood_ = logLikelihoodCandidate;

    // update state
    currentState_ = newState;

    // recalibrate for each component if necessary
    if (samplesNumber_ < getBurnIn())
    {
      for (UnsignedInteger j = 0; j < dimension; ++ j)
      {
        const UnsignedInteger calibrationStep = calibrationStrategy_[j].getCalibrationStep();
        if ((samplesNumber_ % calibrationStep) == (calibrationStep - 1))
        {
          // compute the current acceptation rate
          Scalar rho = 1.0 * accepted[j] / (1.0 * calibrationStep);

          // compute factor
          Scalar factor = calibrationStrategy_[j].computeUpdateFactor(rho);

          // update delta
          delta[j] *= factor;

          // reset the counter
          accepted[j] = 0;

          if (getVerbose())
          {
            LOGINFO( OSS() << "rho=" << rho << " factor=" << factor << " delta=" << delta.getCollection() << " accept=" << getAcceptanceRate().getCollection() );
          }

        } // calibrationStep
      } // j
    } // burn-in

    ++ samplesNumber_;

  } // for i

  // Save the last state
  history_.store(currentState_);
  return currentState_;
}


Point RandomWalkMetropolisHastings::getAcceptanceRate() const
{
  const UnsignedInteger dimension = initialState_.getDimension();
  Point acceptanceRate(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    acceptanceRate[j] = static_cast<Scalar>(acceptedNumber_[j]) / samplesNumber_;
  }
  return acceptanceRate;
}


void RandomWalkMetropolisHastings::setCalibrationStrategy(const CalibrationStrategy& calibrationStrategy)
{
  for (UnsignedInteger i = 0; i < calibrationStrategy_.getSize(); ++ i)
  {
    calibrationStrategy_[i] = calibrationStrategy;
  }
}

RandomWalkMetropolisHastings::CalibrationStrategyCollection RandomWalkMetropolisHastings::getCalibrationStrategyPerComponent() const
{
  return calibrationStrategy_;
}

void RandomWalkMetropolisHastings::setCalibrationStrategyPerComponent(const CalibrationStrategyCollection& calibrationStrategy)
{
  const UnsignedInteger dimension = proposal_.getSize();
  if(dimension != calibrationStrategy.getSize()) throw InvalidDimensionException(HERE) << "The proposal dimension (" << dimension << ") does not match the size of calibration strategy (" << calibrationStrategy.getSize() << ").";
  calibrationStrategy_ = calibrationStrategy;
}


void RandomWalkMetropolisHastings::setProposal(const RandomWalkMetropolisHastings::DistributionCollection& proposal)
{
  const UnsignedInteger dimension = getPrior().getDimension();
  if (proposal.getSize() != dimension) throw InvalidDimensionException(HERE) << "The proposal dimension (" << proposal.getSize() << ") does not match the prior dimension (" << dimension << ").";

  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    Bool symmetric = proposal[i].isElliptical();
    symmetric = symmetric && (std::abs(proposal[i].getMean()[0]) < ResourceMap::GetAsScalar("Distribution-DefaultQuantileEpsilon"));
    if (!symmetric) throw InvalidArgumentException(HERE) << "The proposal density is not symmetric.";
  }
  proposal_ = proposal;
}


RandomWalkMetropolisHastings::DistributionCollection RandomWalkMetropolisHastings::getProposal() const
{
  return proposal_;
}


/* Method save() stores the object through the StorageManager */
void RandomWalkMetropolisHastings::save(Advocate & adv) const
{
  MCMC::save(adv);
  adv.saveAttribute("proposal_", proposal_);
  adv.saveAttribute("calibrationStrategy_", calibrationStrategy_);
  adv.saveAttribute("samplesNumber_", samplesNumber_);
  adv.saveAttribute("acceptedNumber_", acceptedNumber_);
  adv.saveAttribute("currentLogLikelihood_", currentLogLikelihood_);
}

/* Method load() reloads the object from the StorageManager */
void RandomWalkMetropolisHastings::load(Advocate & adv)
{
  MCMC::load(adv);
  adv.loadAttribute("proposal_", proposal_);
  adv.loadAttribute("calibrationStrategy_", calibrationStrategy_);
  adv.loadAttribute("samplesNumber_", samplesNumber_);
  adv.loadAttribute("acceptedNumber_", acceptedNumber_);
  adv.loadAttribute("currentLogLikelihood_", currentLogLikelihood_);
}


END_NAMESPACE_OPENTURNS
