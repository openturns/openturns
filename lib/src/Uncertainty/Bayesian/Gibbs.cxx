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

#include "openturns/Gibbs.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(Gibbs)

static const Factory<Gibbs> Factory_Gibbs;

/* Default constructor */
Gibbs::Gibbs()
  : RandomVectorImplementation()
{
  // Nothing to do
}


/* Parameters constructor */
Gibbs::Gibbs(const MetropolisHastingsCollection & samplers)
  : RandomVectorImplementation()
  , samplers_(samplers)
  , previouslyChosenSampler_(samplers.getSize()) // no sampler has that index
{
  if (!samplers.getSize())
    throw InvalidArgumentException(HERE) << "Gibbs samplers list cannot be empty";

  currentState_ = samplers_[0].getImplementation()->currentState_;
  for (UnsignedInteger j = 0; j < samplers_.getSize(); ++ j)
  {
    if (samplers[j].getDimension() != getDimension())
      throw InvalidArgumentException(HERE) << "Sampler dimension is not compatible";
  }
  setDescription(samplers_[0].getImplementation()->getDescription());
  setUpdatingMethod(UpdatingMethod(ResourceMap::GetAsUnsignedInteger("Gibbs-DefaultUpdatingMethod")));
}


/* String converter */
String Gibbs::__repr__() const
{
  return OSS() << "class=" << Gibbs::GetClassName()
         << " name=" << getName()
         << " derived from " << RandomVectorImplementation::__repr__()
         << " samplers=" << samplers_
         << " updatingMethod=" << updatingMethod_;
}


Gibbs* Gibbs::clone() const
{
  return new Gibbs(*this);
}

UnsignedInteger Gibbs::getDimension() const
{
  return currentState_.getDimension();
}

// Sequentially sample from the MH blocks
void Gibbs::computeRealizationDeterministicUpdating() const
{
  for (UnsignedInteger j = 0; j < samplers_.getSize(); ++ j)
  {
    // get the current state from the previous sampler
    samplers_[j].getImplementation()->currentState_ = currentState_;
    if (recomputeLogPosterior_[j])
      currentLogPosterior_ = samplers_[j].computeLogPosterior(currentState_);
    samplers_[j].getImplementation()->currentLogPosterior_ = currentLogPosterior_;

    // pass the current state to the next sampler
    currentState_ = samplers_[j].getRealization();
    currentLogPosterior_ = samplers_[j].getImplementation()->currentLogPosterior_;
  }
}

// Sample from a randomly chosen MH block
void Gibbs::computeRealizationRandomUpdating() const
{
  // randomly choose the sampler
  const UnsignedInteger chosenSampler = RandomGenerator::IntegerGenerate(samplers_.getSize());

  if (chosenSampler != previouslyChosenSampler_)
  {
    // get the current state from the previous sampler
    samplers_[chosenSampler].getImplementation()->currentState_ = currentState_;
    if (recomputeLogPosterior_[chosenSampler])
      currentLogPosterior_ = samplers_[chosenSampler].computeLogPosterior(currentState_);
    samplers_[chosenSampler].getImplementation()->currentLogPosterior_ = currentLogPosterior_;
  }

  // pass the current state to the next sampler
  currentState_ = samplers_[chosenSampler].getRealization();
  currentLogPosterior_ = samplers_[chosenSampler].getImplementation()->currentLogPosterior_;

  // store chosenSampler for the next iteration
  previouslyChosenSampler_ = chosenSampler;
}


Point Gibbs::getRealization() const
{
  const UnsignedInteger nbSamplers = samplers_.getSize();

  // perform burnin if necessary
  const UnsignedInteger size = 1 + ((samplesNumber_ < getBurnIn()) ? getBurnIn() : 0);

  // check the first log-posterior
  if (samplesNumber_ == 0)
  {
    for (UnsignedInteger j = 0; j < nbSamplers; ++ j)
    {
      const Scalar currentLogPosterior = samplers_[j].computeLogPosterior(samplers_[j].getImplementation()->currentState_);
      if (currentLogPosterior <= SpecFunc::LowestScalar)
        throw InvalidArgumentException(HERE) << "The initial state should have non-zero posterior probability density";
    }
  }

  // see which components will need to recompute the posterior
  if (samplesNumber_ == 0)
  {
    Point samplersLogPosteriors(nbSamplers);
    for (UnsignedInteger j = 0; j < nbSamplers; ++ j)
      samplersLogPosteriors[j] = samplers_[j].computeLogPosterior(samplers_[j].getImplementation()->currentState_);
    recomputeLogPosterior_ = Indices(nbSamplers);

    // For each sampler, check if it needs to recompute the log-posterior:
    // if it shares the same log-posterior as the previous sampler, it does not.
    // For the first sampler, the "previous" sampler is the last.
    recomputeLogPosterior_[0] = (samplersLogPosteriors[0] == samplersLogPosteriors[nbSamplers - 1]) ? 0 : 1;
    for (UnsignedInteger j = 1; j < nbSamplers; ++ j)
      recomputeLogPosterior_[j] = (samplersLogPosteriors[j] == samplersLogPosteriors[j - 1]) ? 0 : 1;

    if (updatingMethod_ == RANDOM_UPDATING) // if one of the blocks needs to recompute the log posterior, then all do
    {
      Bool recompute = false;
      for (UnsignedInteger j = 0; j < nbSamplers; ++j)
        if (recomputeLogPosterior_[j]) recompute = true;
      if (recompute)
      {
        for (UnsignedInteger j = 0; j < nbSamplers; ++j)
          recomputeLogPosterior_[j] = true;
      }
    } // random updating
  }

  // for each new sample
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    switch (updatingMethod_)
    {
      case DETERMINISTIC_UPDATING:
        computeRealizationDeterministicUpdating();
        break;
      case RANDOM_UPDATING:
        computeRealizationRandomUpdating();
        break;
      default:
        throw InvalidArgumentException(HERE) << "Error: invalid updating method, must be DETERMINISTIC_UPDATING or RANDOM_UPDATING, here updatingMethod_=" << updatingMethod_;
    } /* end switch */
  }
  samplesNumber_ += size;

  // Save the current state
  history_.store(currentState_);

  return currentState_;
}

/* Samplers accessor */
Gibbs::MetropolisHastingsCollection Gibbs::getMetropolisHastingsCollection() const
{
  return samplers_;
}

/* Is order random accessors */
void Gibbs::setUpdatingMethod(const UpdatingMethod updatingMethod)
{
  if (updatingMethod > 1)
    throw InvalidArgumentException(HERE) << "Updating method should be 0 (DETERMINISTIC_UPDATING) or 1 (RANDOM_UPDATING)";
  if (updatingMethod != updatingMethod_) // reset
  {
    samplesNumber_ = 0;
    previouslyChosenSampler_ = samplers_.getSize(); // no sampler has that index
    updatingMethod_ = updatingMethod;
  }
}

Gibbs::UpdatingMethod Gibbs::getUpdatingMethod() const
{
  return static_cast<UpdatingMethod>(updatingMethod_);
}

void Gibbs::setBurnIn(const UnsignedInteger burnIn)
{
  burnIn_ = burnIn;
}


UnsignedInteger Gibbs::getBurnIn() const
{
  return burnIn_;
}


/* Indices for which the posterior is recomputed */
Indices Gibbs::getRecomputeLogPosterior() const
{
  return recomputeLogPosterior_;
}

void Gibbs::setHistory(const HistoryStrategy & history)
{
  history_ = history;
  history_.setDimension(currentState_.getDimension());
}

HistoryStrategy Gibbs::getHistory() const
{
  return history_;
}

/* Method save() stores the object through the StorageManager */
void Gibbs::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute("samplers_", samplers_);
  adv.saveAttribute("updatingMethod_", updatingMethod_);
  adv.saveAttribute("previouslyChosenSampler_", previouslyChosenSampler_);
  adv.saveAttribute("currentState_", currentState_);
  adv.saveAttribute("burnIn_", burnIn_);
}

/* Method load() reloads the object from the StorageManager */
void Gibbs::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute("samplers_", samplers_);
  adv.loadAttribute("updatingMethod_", updatingMethod_);
  adv.loadAttribute("previouslyChosenSampler_", previouslyChosenSampler_);
  adv.loadAttribute("currentState_", currentState_);
  adv.loadAttribute("burnIn_", burnIn_);
}


END_NAMESPACE_OPENTURNS
