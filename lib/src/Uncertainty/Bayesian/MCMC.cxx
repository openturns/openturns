//                                               -*- C++ -*-
/**
 *  @brief MCMC base class
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

#include "openturns/MCMC.hxx"
#include "openturns/ConditionalDistribution.hxx"
#include "openturns/Normal.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Full.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/ParametricFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MCMC)

static const Factory<MCMC> Factory_MCMC;

/* Default constructor */
MCMC::MCMC()
  : RandomVectorImplementation()
  , burnIn_(0)
  , thinning_(0)
  , verbose_(false)
{
  // Nothing to do
}


/* Parameters constructor */
MCMC::MCMC( const Distribution & prior,
            const Distribution & conditional,
            const Sample & observations,
            const Point & initialState)
  : RandomVectorImplementation()
  , initialState_(initialState)
  , currentState_(initialState)
  , history_(Full())
  , conditional_(conditional)

    // when not provided, set the model to the identity
  , model_()
  , burnIn_(ResourceMap::GetAsUnsignedInteger("MCMC-DefaultBurnIn"))
  , thinning_(ResourceMap::GetAsUnsignedInteger("MCMC-DefaultThinning"))
  , verbose_(false)
{
  const SymbolicFunction fullFunction(Description::BuildDefault(initialState.getDimension(), "x"), Description::BuildDefault(initialState.getDimension(), "x"));
  model_ = ParametricFunction(fullFunction, Indices(0), Point(0));
  setPrior(prior);
  if (model_.getInputDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The model input dimension (" << model_.getInputDimension() << ") does not match the dimension of the prior (" << prior.getDimension() << ").";
  setParameters(Sample(observations.getSize(), 0));
  setObservations(observations);
  if (conditional.getParameterDimension() != model_.getOutputDimension()) throw InvalidDimensionException(HERE) << "The parameter dimension" << conditional.getParameterDimension() << " does not match the output dimension of the model (" << model_.getOutputDimension() << ").";
  if (initialState.getDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The initialState state dimension (" << initialState.getDimension() << ") does not match the prior dimension (" << prior.getDimension() << ").";
  history_.setDimension(initialState.getDimension());
}


/* Parameters constructor */
MCMC::MCMC( const Distribution & prior,
            const Distribution & conditional,
            const Function & model,
            const Sample & parameters,
            const Sample & observations,
            const Point & initialState)
  : RandomVectorImplementation()
  , initialState_(initialState)
  , currentState_(initialState)
  , history_(Full())
  , conditional_(conditional)
  , model_(model)
  , burnIn_(ResourceMap::GetAsUnsignedInteger("MCMC-DefaultBurnIn"))
  , thinning_(ResourceMap::GetAsUnsignedInteger("MCMC-DefaultThinning"))
  , verbose_(false)
{
  setPrior(prior);
  if (model.getInputDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The model input dimension (" << model.getInputDimension() << ") does not match the dimension of the prior (" << prior.getDimension() << ").";
  setParameters(parameters);
  setObservations(observations);
  if (model_.getInputDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The model input dimension (" << model_.getInputDimension() << ") does not match the dimension of the prior (" << prior.getDimension() << ").";
  if (initialState.getDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The initialState state dimension (" << initialState.getDimension() << ") does not match the prior dimension (" << prior.getDimension() << ").";
  history_.setDimension(initialState.getDimension());
}


/* String converter */
String MCMC::__repr__() const
{
  return OSS() << "class=" << MCMC::GetClassName()
         << " prior=" << prior_
         << " conditional=" << conditional_
         << " model=" << model_
         << " parameters=" << parameters_
         << " observations=" << observations_
         << " burnIn=" << burnIn_
         << " thinning=" << thinning_;
}


MCMC* MCMC::clone() const
{
  return new MCMC(*this);
}


UnsignedInteger MCMC::getDimension() const
{
  return prior_.getDimension();
}


/* Compute the log-likelihood */
Scalar MCMC::computeLogLikelihood(const Point & xi) const
{
  Scalar value = 0.0;

  const UnsignedInteger size = observations_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    // retrieve model data if available
    model_.setParameter(parameters_[i]);
    const Point zi(model_(xi));

    Distribution pI(conditional_);
    pI.setParameter(zi);
    Scalar logPdf = pI.computeLogPDF(observations_[i]);
    value += logPdf;
  }
  return value;
}

void MCMC::setPrior(const Distribution& prior)
{
  //  if (!prior.isContinuous()) throw InvalidArgumentException(HERE) << "The prior should be continuous.";
  prior_ = prior;
  setDescription(prior.getDescription());
}


Distribution MCMC::getPrior() const
{
  return prior_;
}


Distribution MCMC::getConditional() const
{
  return conditional_;
}


Function MCMC::getModel() const
{
  return model_;
}


void MCMC::setObservations(const Sample& observations)
{
  if (!(observations.getSize() > 0)) throw InvalidArgumentException(HERE) << "No observation provided.";
  observations_ = observations;
}


Sample MCMC::getObservations() const
{
  return observations_;
}


void MCMC::setParameters(const Sample& parameters)
{
  if (!(parameters.getSize() > 0)) throw InvalidArgumentException(HERE) << "No parameter provided.";
  parameters_ = parameters;
}


Sample MCMC::getParameters() const
{
  return parameters_;
}


void MCMC::setBurnIn(UnsignedInteger burnIn)
{
  burnIn_ = burnIn;
}


UnsignedInteger MCMC::getBurnIn() const
{
  return burnIn_;
}


void MCMC::setThinning(UnsignedInteger thinning)
{
  if (thinning == 0) throw InvalidArgumentException(HERE) << "The thinning parameter should be positive.";
  thinning_ = thinning;
}


UnsignedInteger MCMC::getThinning() const
{
  return thinning_;
}


void MCMC::setHistory(const HistoryStrategy & history)
{
  history_ = history;
  history_.setDimension(initialState_.getDimension());
}


HistoryStrategy MCMC::getHistory() const
{
  return history_;
}


void MCMC::setNonRejectedComponents(const Indices & nonRejectedComponents)
{
  const UnsignedInteger dimension = initialState_.getDimension();
  if (!nonRejectedComponents.check(dimension)) throw InvalidDimensionException(HERE) << "The non-rejected components must have values within the chain dimension (" << initialState_.getDimension() << ").";
  nonRejectedComponents_ = nonRejectedComponents;
}


Indices MCMC::getNonRejectedComponents() const
{
  return nonRejectedComponents_;
}


void MCMC::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}


Bool MCMC::getVerbose() const
{
  return verbose_;
}

/* Method save() stores the object through the StorageManager */
void MCMC::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute("initialState_", initialState_);
  adv.saveAttribute("currentState_", currentState_);
  adv.saveAttribute("prior_", prior_);
  adv.saveAttribute("conditional_", conditional_);
  adv.saveAttribute("model_", model_);
  adv.saveAttribute("parameters_", parameters_);
  adv.saveAttribute("observations_", observations_);
  adv.saveAttribute("burnIn_", burnIn_);
  adv.saveAttribute("thinning_", thinning_);
  adv.saveAttribute("nonRejectedComponents_", nonRejectedComponents_);
  adv.saveAttribute("verbose_", verbose_);
}

/* Method load() reloads the object from the StorageManager */
void MCMC::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute("initialState_", initialState_);
  adv.loadAttribute("currentState_", currentState_);
  adv.loadAttribute("prior_", prior_);
  adv.loadAttribute("conditional_", conditional_);
  adv.loadAttribute("model_", model_);
  adv.loadAttribute("parameters_", parameters_);
  adv.loadAttribute("observations_", observations_);
  adv.loadAttribute("burnIn_", burnIn_);
  adv.loadAttribute("thinning_", thinning_);
  adv.loadAttribute("nonRejectedComponents_", nonRejectedComponents_);
  adv.loadAttribute("verbose_", verbose_);
}


END_NAMESPACE_OPENTURNS
