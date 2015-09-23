//                                               -*- C++ -*-
/**
 *  @brief RandomWalkMetropolisHastings updates the chain
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "RandomWalkMetropolisHastings.hxx"
#include "RandomGenerator.hxx"
#include "PersistentObjectFactory.hxx"
#include "ConditionalDistribution.hxx"
#include "Log.hxx"
#include "SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection<CalibrationStrategy>);
static const Factory<PersistentCollection<CalibrationStrategy> > RegisteredFactory2;


CLASSNAMEINIT(RandomWalkMetropolisHastings);

static const Factory<RandomWalkMetropolisHastings> RegisteredFactory;

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
    const NumericalSample & observations,
    const NumericalPoint & initialState,
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
    const NumericalMathFunction & model,
    const NumericalSample & parameters,
    const NumericalSample & observations,
    const NumericalPoint & initialState,
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

NumericalPoint RandomWalkMetropolisHastings::getRealization() const
{
  const UnsignedInteger dimension = initialState_.getDimension();

  // update factor
  NumericalPoint delta(dimension, 1.0);

  // number of samples accepted until calibration step
  Indices accepted(dimension);

  // perform burning if necessary
  const UnsignedInteger size = getThinning() + ((samplesNumber_ < getBurnIn()) ? getBurnIn() : 0);

  // check the first likelihood
  if (samplesNumber_ == 0)
  {
    currentLogLikelihood_ = computeLogLikelihood(currentState_);
    if (currentLogLikelihood_ <= -SpecFunc::MaxNumericalScalar)
      throw InvalidArgumentException(HERE) << "The likelihood of the initial state should be positive";
  }

  // for each new sample
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    // accumulates the updates over each components
    NumericalPoint newState(currentState_);

    history_.store(currentState_);

    NumericalScalar logLikelihoodCandidate = currentLogLikelihood_;

    // update each chain component
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      // new candidate for the j-th component
      NumericalPoint nextState(newState);

      Bool nonRejectedComponent = nonRejectedComponents_.__contains__(j);
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

      const NumericalScalar nextLogLikelihood = computeLogLikelihood(nextState);

      // alpha = likehood(newstate)/likehood(oldstate)
      const NumericalScalar alphaLog = nextLogLikelihood  - currentLogLikelihood_;

      // acceptance test
      const NumericalScalar uLog = log(RandomGenerator::Generate());
      if (nonRejectedComponent || (uLog < alphaLog))
      {
        // the likelihood can be 0 wrt the observations because of a non-rejected component (always ok wrt prior)
        if (nextLogLikelihood == -SpecFunc::MaxNumericalScalar)
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
          NumericalScalar rho = 1.0 * accepted[j] / (1.0 * calibrationStep);

          // compute factor
          NumericalScalar factor = calibrationStrategy_[j].computeUpdateFactor(rho);

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


NumericalPoint RandomWalkMetropolisHastings::getAcceptanceRate() const
{
  const UnsignedInteger dimension = initialState_.getDimension();
  NumericalPoint acceptanceRate(dimension);
  for (UnsignedInteger j = 0; j < dimension; ++ j)
  {
    acceptanceRate[j] = static_cast<NumericalScalar>(acceptedNumber_[j]) / samplesNumber_;
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
    symmetric = symmetric && (fabs(proposal[i].getMean()[0]) < ResourceMap::GetAsNumericalScalar("DistributionImplementation-DefaultQuantileEpsilon"));
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
