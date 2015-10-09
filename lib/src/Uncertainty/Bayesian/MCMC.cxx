//                                               -*- C++ -*-
/**
 *  @brief MCMC updates the chain
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

#include "MCMC.hxx"
#include "ConditionalDistribution.hxx"
#include "Normal.hxx"
#include "SpecFunc.hxx"
#include "PersistentObjectFactory.hxx"
#include "IdentityMatrix.hxx"
#include "Full.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MCMC);

static const Factory<MCMC> RegisteredFactory;

/* Default constructor */
MCMC::MCMC()
  : SamplerImplementation()
  , burnIn_(0)
  , thinning_(0)
{
  // Nothing to do
}


/* Parameters constructor */
MCMC::MCMC( const Distribution & prior,
            const Distribution & conditional,
            const NumericalSample & observations,
            const NumericalPoint & initialState)
  : SamplerImplementation()
  , initialState_(initialState)
  , currentState_(initialState)
  , history_(Full())
  , conditional_(conditional)

  // when not provided, set the model to the identity
  , model_()
  , burnIn_(ResourceMap::GetAsUnsignedInteger("MCMC-DefaultBurnIn"))
  , thinning_(ResourceMap::GetAsUnsignedInteger("MCMC-DefaultThinning"))
{
  const NumericalMathFunction fullFunction(Description::BuildDefault(initialState.getDimension(), "x"), Description::BuildDefault(initialState.getDimension(), "x"));
  const Indices indices(0);
  model_ = NumericalMathFunction(fullFunction, indices);
  setPrior(prior);
  if (model_.getInputDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The model input dimension (" << model_.getInputDimension() << ") does not match the dimension of the prior (" << prior.getDimension() << ").";
  setParameters(NumericalSample(observations.getSize(), 0));
  setObservations(observations);
  if (conditional.getParametersNumber() != model_.getOutputDimension()) throw InvalidDimensionException(HERE) << "The parameter dimension" << conditional.getParametersNumber() << " does not match the output dimension of the model (" << model_.getOutputDimension() << ").";
  if (initialState.getDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The initialState state dimension (" << initialState.getDimension() << ") does not match the prior dimension (" << prior.getDimension() << ").";

}


/* Parameters constructor */
MCMC::MCMC( const Distribution & prior,
            const Distribution & conditional,
            const NumericalMathFunction & model,
            const NumericalSample & parameters,
            const NumericalSample & observations,
            const NumericalPoint & initialState)
  : SamplerImplementation()
  , initialState_(initialState)
  , currentState_(initialState)
  , history_(Full())
  , conditional_(conditional)
  , model_(model)
  , burnIn_(ResourceMap::GetAsUnsignedInteger("MCMC-DefaultBurnIn"))
  , thinning_(ResourceMap::GetAsUnsignedInteger("MCMC-DefaultThinning"))
{
  setPrior(prior);
  if (model.getInputDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The model input dimension (" << model.getInputDimension() << ") does not match the dimension of the prior (" << prior.getDimension() << ").";
  setParameters(parameters);
  setObservations(observations);
  if (model_.getInputDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The model input dimension (" << model_.getInputDimension() << ") does not match the dimension of the prior (" << prior.getDimension() << ").";
  if (initialState.getDimension() != prior.getDimension()) throw InvalidDimensionException(HERE) << "The initialState state dimension (" << initialState.getDimension() << ") does not match the prior dimension (" << prior.getDimension() << ").";
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


/* Compute the likelihood w.r.t. observartions */
NumericalScalar MCMC::computeLogLikelihood(const NumericalPoint & xi) const
{
  NumericalScalar value = prior_.computeLogPDF(xi);
  if (value == -SpecFunc::MaxNumericalScalar) return -SpecFunc::MaxNumericalScalar;

  const UnsignedInteger size = observations_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    // retrieve model data if available
    const NumericalPoint zi(model_(xi, parameters_[i]));

    Distribution pI(conditional_);
    pI.setParameters(zi);
    NumericalScalar logPdf = pI.computeLogPDF(observations_[i]);
    if (logPdf == -SpecFunc::MaxNumericalScalar) return -SpecFunc::MaxNumericalScalar;
    value += logPdf;
  }
  return value;
}

void MCMC::setPrior(const Distribution& prior)
{
  //  if (!prior.isContinuous()) throw InvalidArgumentException(HERE) << "The prior should be continuous.";
  prior_ = prior;
}


Distribution MCMC::getPrior() const
{
  return prior_;
}


Distribution MCMC::getConditional() const
{
  return conditional_;
}


NumericalMathFunction MCMC::getModel() const
{
  return model_;
}


void MCMC::setObservations(const NumericalSample& observations)
{
  if (!(observations.getSize() > 0)) throw InvalidArgumentException(HERE) << "No observation provided.";
  observations_ = observations;
}


NumericalSample MCMC::getObservations() const
{
  return observations_;
}


void MCMC::setParameters(const NumericalSample& parameters)
{
  if (!(parameters.getSize() > 0)) throw InvalidArgumentException(HERE) << "No parameter provided.";
  parameters_ = parameters;
}


NumericalSample MCMC::getParameters() const
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


/* Method save() stores the object through the StorageManager */
void MCMC::save(Advocate & adv) const
{
  SamplerImplementation::save(adv);
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
}

/* Method load() reloads the object from the StorageManager */
void MCMC::load(Advocate & adv)
{
  SamplerImplementation::load(adv);
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
}


END_NAMESPACE_OPENTURNS
