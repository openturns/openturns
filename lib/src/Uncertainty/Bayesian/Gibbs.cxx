//                                               -*- C++ -*-
/**
 *  @brief Metropolis-Hastings algorithm
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
}


/* String converter */
String Gibbs::__repr__() const
{
  return OSS() << "class=" << Gibbs::GetClassName()
         << " name=" << getName()
         << " derived from " << RandomVectorImplementation::__repr__()
         << " samplers=" << samplers_;
}


Gibbs* Gibbs::clone() const
{
  return new Gibbs(*this);
}

UnsignedInteger Gibbs::getDimension() const
{
  return currentState_.getDimension();
}

Point Gibbs::getRealization() const
{
  // perform burnin if necessary
  const UnsignedInteger size = getThinning() + ((samplesNumber_ < getBurnIn()) ? getBurnIn() : 0);

  // check the first log-posterior
  if (samplesNumber_ == 0)
  {
    for (UnsignedInteger j = 0; j < samplers_.getSize(); ++ j)
    {
      const Scalar currentLogPosterior = samplers_[j].computeLogPosterior(samplers_[j].getImplementation()->currentState_);
      if (currentLogPosterior <= SpecFunc::LowestScalar)
        throw InvalidArgumentException(HERE) << "The initial state should have non-zero posterior probability density";
    }
  }

  // see which components will need to recompute the posterior
  if (samplesNumber_ == 0)
  {
    Point lp(getDimension());
    for (UnsignedInteger j = 0; j < samplers_.getSize(); ++ j)
      lp[j] = samplers_[j].computeLogPosterior(samplers_[j].getImplementation()->currentState_);
    recomputeLogPosterior_ = Indices(getDimension());
    for (UnsignedInteger j = 0; j < samplers_.getSize(); ++ j)
      recomputeLogPosterior_[j] = (lp[j] != lp[(j + getDimension() - 1) % getDimension()]) ? 1 : 0;
  }

  // for each new sample
  for (UnsignedInteger i = 0; i < size; ++ i)
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

void Gibbs::setBurnIn(const UnsignedInteger burnIn)
{
  burnIn_ = burnIn;
}


UnsignedInteger Gibbs::getBurnIn() const
{
  return burnIn_;
}


void Gibbs::setThinning(const UnsignedInteger thinning)
{
  if (thinning == 0) throw InvalidArgumentException(HERE) << "The thinning parameter should be positive.";
  thinning_ = thinning;
}


UnsignedInteger Gibbs::getThinning() const
{
  return thinning_;
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
  adv.saveAttribute("currentState_", currentState_);
  adv.saveAttribute("burnIn_", burnIn_);
  adv.saveAttribute("thinning_", thinning_);
}

/* Method load() reloads the object from the StorageManager */
void Gibbs::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute("samplers_", samplers_);
  adv.loadAttribute("currentState_", currentState_);
  adv.loadAttribute("burnIn_", burnIn_);
  adv.loadAttribute("thinning_", thinning_);
}


END_NAMESPACE_OPENTURNS
