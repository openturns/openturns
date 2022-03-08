//                                               -*- C++ -*-
/**
 *  @brief MetropolisHastingsImplementation base class
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

#include "openturns/MetropolisHastingsImplementation.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Full.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/ParametricFunction.hxx"
#include "openturns/RandomGenerator.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MetropolisHastingsImplementation)

static const Factory<MetropolisHastingsImplementation> Factory_MetropolisHastingsImplementation;

/* Default constructor */
MetropolisHastingsImplementation::MetropolisHastingsImplementation()
  : RandomVectorImplementation()
{
  // Nothing to do
}

/* Parameters constructor */
MetropolisHastingsImplementation::MetropolisHastingsImplementation(const Point & initialState,
                                                                   const Indices & marginalIndices)
  : RandomVectorImplementation()
  , initialState_(initialState)
  , currentState_(initialState)
  , history_(Full())
  , burnIn_(ResourceMap::GetAsUnsignedInteger("MetropolisHastings-DefaultBurnIn"))
  , thinning_(ResourceMap::GetAsUnsignedInteger("MetropolisHastings-DefaultThinning"))
{
  setMarginalIndices(marginalIndices);
  history_.setDimension(initialState.getDimension());
  setDescription(Description::BuildDefault(initialState.getSize(), "x"));
}

/* Parameters constructor */
MetropolisHastingsImplementation::MetropolisHastingsImplementation(const Distribution & targetDistribution,
                                                                   const Point & initialState,
                                                                   const Indices & marginalIndices)
  : RandomVectorImplementation()
  , initialState_(initialState)
  , currentState_(initialState)
  , history_(Full())
  , burnIn_(ResourceMap::GetAsUnsignedInteger("MetropolisHastings-DefaultBurnIn"))
  , thinning_(ResourceMap::GetAsUnsignedInteger("MetropolisHastings-DefaultThinning"))
{
  setTargetDistribution(targetDistribution);
  setMarginalIndices(marginalIndices);
  history_.setDimension(initialState.getDimension());
}

/* Parameters constructor */
MetropolisHastingsImplementation::MetropolisHastingsImplementation(const Function & targetLogPDF,
                                                                   const Domain & support,
                                                                   const Point & initialState,
                                                                   const Indices & marginalIndices)
  : RandomVectorImplementation()
  , initialState_(initialState)
  , currentState_(initialState)
  , marginalIndices_(marginalIndices)
  , history_(Full())
  , targetLogPDF_(targetLogPDF)
  , support_(support)
  , burnIn_(ResourceMap::GetAsUnsignedInteger("MetropolisHastings-DefaultBurnIn"))
  , thinning_(ResourceMap::GetAsUnsignedInteger("MetropolisHastings-DefaultThinning"))
{
  setTargetLogPDF(targetLogPDF, support);
  setMarginalIndices(marginalIndices);
  history_.setDimension(initialState.getDimension());
}

/* Likelihood accessor */
void MetropolisHastingsImplementation::setLikelihood(const Distribution & conditional,
                                                      const Sample & observations,
                                                      const Function & linkFunction,
                                                      const Sample & covariates)
{
  conditional_ = conditional;
  if (observations.getDimension() != conditional.getDimension())
          throw InvalidArgumentException(HERE) << "The observations dimension (" << observations.getDimension()
                                               << ") does not match the conditional dimension (" << conditional.getDimension() << ").";
  observations_ = observations;
  if (linkFunction.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (linkFunction.getInputDimension() != initialState_.getDimension())
      throw InvalidDimensionException(HERE) << "The linkFunction input dimension (" << linkFunction.getInputDimension()
                                            << ") does not match the dimension of the prior (" << initialState_.getDimension() << ").";
    linkFunction_ = linkFunction;
  }
  else
  {
    // when not provided, set the model to the identity
    const SymbolicFunction fullFunction(Description::BuildDefault(initialState_.getDimension(), "x"), Description::BuildDefault(initialState_.getDimension(), "x"));
    linkFunction_ = ParametricFunction(fullFunction, Indices(0), Point(0));
  }
  if (conditional.getParameterDimension() != linkFunction_.getOutputDimension())
    throw InvalidDimensionException(HERE) << "The parameter dimension (" << conditional.getParameterDimension()
                                          << ") does not match the output dimension of linkFunction (" << linkFunction_.getOutputDimension() << ").";
  if (covariates.getDimension())
  {
    if (covariates.getDimension() != linkFunction_.getParameterDimension())
      throw InvalidArgumentException(HERE) << "The covariate dimension (" << covariates.getDimension()
                                            << ") does not match linkFunction parameter dimension (" << linkFunction_.getParameterDimension() << ").";
    if (covariates.getSize() != observations.getSize())
      throw InvalidArgumentException(HERE) << "The covariate size (" << covariates.getSize()
                                            << ") does not match the observations size (" << observations.getSize() << ").";
    covariates_ = covariates;
  }
  else
    covariates_ = Sample(observations.getSize(), 0);
}


/* String converter */
String MetropolisHastingsImplementation::__repr__() const
{
  return OSS() << "class=" << MetropolisHastingsImplementation::GetClassName()
         << " targetDistribution=" << targetDistribution_
         << " conditional=" << conditional_
         << " linkFunction=" << linkFunction_
         << " covariates=" << covariates_
         << " observations=" << observations_
         << " burnIn=" << burnIn_
         << " thinning=" << thinning_;
}


MetropolisHastingsImplementation* MetropolisHastingsImplementation::clone() const
{
  return new MetropolisHastingsImplementation(*this);
}

Point MetropolisHastingsImplementation::getCandidate() const
{
  throw NotYetImplementedException(HERE) << "MetropolisHastingsImplementation::getCandidate";
}

Scalar MetropolisHastingsImplementation::computeLogPDFPrior(const Point & state) const
{
  if (targetLogPDF_.getEvaluation().getImplementation()->isActualImplementation())
  {
    if (support_.contains(state))
      return targetLogPDF_(state)[0];
    else
      return SpecFunc::LowestScalar;
  }
  else if (hasTargetDistribution_)
    return getTargetDistribution().computeLogPDF(state);
  return 0.0;
}

Scalar MetropolisHastingsImplementation::computeLogPosterior(const Point & state) const
{
  return computeLogLikelihood(state) + computeLogPDFPrior(state);
}

Point MetropolisHastingsImplementation::getRealization() const
{
  // perform burnin if necessary
  const UnsignedInteger size = getThinning() + ((samplesNumber_ < getBurnIn()) ? getBurnIn() : 0);

  // check the first likelihood
  if (samplesNumber_ == 0)
  {
    currentLogPosterior_ = computeLogPosterior(currentState_);
    if (currentLogPosterior_ <= SpecFunc::LowestScalar)
      throw InvalidArgumentException(HERE) << "The initial state should have non-zero posterior probability density";
  }

  // for each new sample
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    const Point newState(getCandidate());
    const Scalar newLogPosterior = computeLogPosterior(newState);
    
    // alpha = posterior(newstate)/posterior(oldstate)
    const Scalar alphaLog = newLogPosterior - currentLogPosterior_;
    const Scalar uLog = log(RandomGenerator::Generate());
    if (uLog < alphaLog)
    {
      currentLogPosterior_ = newLogPosterior;
      ++ acceptedNumber_;
      ++ acceptedNumberAdaptation_;
      currentState_ = newState;
    }

    // increment 1 by 1 as used in getCandidate 
    ++ samplesNumber_;
  }

  // Save the current state
  history_.store(currentState_);

  return currentState_;
}

UnsignedInteger MetropolisHastingsImplementation::getDimension() const
{
  return initialState_.getDimension();
}

/* Compute the log-likelihood */
Scalar MetropolisHastingsImplementation::computeLogLikelihood(const Point & xi) const
{
  if (!observations_.getDimension()) // no likelihood set
    return 0.0;

  Scalar value = 0.0;
  const UnsignedInteger size = observations_.getSize();
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    // retrieve model data if available
    Function model(linkFunction_);
    model.setParameter(covariates_[i]);
    const Point zi(model(xi));

    Distribution pI(conditional_);
    pI.setParameter(zi);
    Scalar logPdf = pI.computeLogPDF(observations_[i]);
    value += logPdf;
  }
  return value;
}

void MetropolisHastingsImplementation::setTargetDistribution(const Distribution & targetDistribution)
{
  if (initialState_.getDimension() != targetDistribution.getDimension())
    throw InvalidDimensionException(HERE) << "The initial state dimension (" << initialState_.getDimension()
                                          << ") does not match the targetDistribution dimension (" << targetDistribution.getDimension() << ").";
  targetDistribution_ = targetDistribution;
  hasTargetDistribution_ = true;
  setDescription(targetDistribution.getDescription());
}

Distribution MetropolisHastingsImplementation::getTargetDistribution() const
{
  if (!hasTargetDistribution_)
    throw InvalidArgumentException(HERE) << "No target distribution provided";
  return targetDistribution_;
}

void MetropolisHastingsImplementation::setTargetLogPDF(const Function & targetLogPDF, const Domain & support)
{
  if (support.getDimension() != targetLogPDF.getInputDimension())
    throw InvalidDimensionException(HERE) << "The prior support dimension must match the prior input dimension";
  if (targetLogPDF.getOutputDimension() != 1)
    throw InvalidDimensionException(HERE) << "The prior output dimension should be 1";
  if (initialState_.getDimension() != targetLogPDF.getInputDimension())
    throw InvalidDimensionException(HERE) << "The initial state dimension (" << initialState_.getDimension() << ") does not match the prior dimension (" << targetLogPDF.getInputDimension() << ").";
  targetLogPDF_ = targetLogPDF;
  support_ = support;
  setDescription(targetLogPDF.getInputDescription());
}

void MetropolisHastingsImplementation::setMarginalIndices(const Indices & marginalIndices)
{
  if (!marginalIndices.check(initialState_.getDimension()))
    throw InvalidArgumentException(HERE) << "Marginal indices must be < dimension";
  marginalIndices_ = marginalIndices;
  if (marginalIndices.isEmpty())
  {
    marginalIndices_ = Indices(initialState_.getDimension());
    marginalIndices_.fill();
  }
}

Function MetropolisHastingsImplementation::getTargetLogPDF() const
{
  if (!targetLogPDF_.getEvaluation().getImplementation()->isActualImplementation())
    throw InvalidArgumentException(HERE) << "No target log-pdf provided";
  return targetLogPDF_;
}

Domain MetropolisHastingsImplementation::getTargetLogPDFSupport() const
{
  if (!targetLogPDF_.getEvaluation().getImplementation()->isActualImplementation())
    throw InvalidArgumentException(HERE) << "No target log-pdf provided";
  return support_;
}
  
Point MetropolisHastingsImplementation::getInitialState() const
{
  return initialState_;
}

Indices MetropolisHastingsImplementation::getMarginalIndices() const
{
  return marginalIndices_;
}

Distribution MetropolisHastingsImplementation::getConditional() const
{
  return conditional_;
}


Function MetropolisHastingsImplementation::getLinkFunction() const
{
  return linkFunction_;
}


Sample MetropolisHastingsImplementation::getObservations() const
{
  return observations_;
}


Sample MetropolisHastingsImplementation::getCovariates() const
{
  return covariates_;
}


void MetropolisHastingsImplementation::setBurnIn(const UnsignedInteger burnIn)
{
  burnIn_ = burnIn;
}


UnsignedInteger MetropolisHastingsImplementation::getBurnIn() const
{
  return burnIn_;
}


void MetropolisHastingsImplementation::setThinning(const UnsignedInteger thinning)
{
  if (thinning == 0) throw InvalidArgumentException(HERE) << "The thinning parameter should be positive.";
  thinning_ = thinning;
}


UnsignedInteger MetropolisHastingsImplementation::getThinning() const
{
  return thinning_;
}


void MetropolisHastingsImplementation::setVerbose(const Bool verbose)
{
  verbose_ = verbose;
}


Bool MetropolisHastingsImplementation::getVerbose() const
{
  return verbose_;
}


void MetropolisHastingsImplementation::setHistory(const HistoryStrategy & history)
{
  history_ = history;
  history_.setDimension(initialState_.getDimension());
}


HistoryStrategy MetropolisHastingsImplementation::getHistory() const
{
  return history_;
}

Scalar MetropolisHastingsImplementation::getAcceptanceRate() const
{
  if (!samplesNumber_)
    throw NotDefinedException(HERE) << "Cannot compute acceptance rate as no samples were generated.";
  return static_cast<Scalar>(acceptedNumber_) / samplesNumber_;
}

/* Method save() stores the object through the StorageManager */
void MetropolisHastingsImplementation::save(Advocate & adv) const
{
  RandomVectorImplementation::save(adv);
  adv.saveAttribute("initialState_", initialState_);
  adv.saveAttribute("currentState_", currentState_);
  adv.saveAttribute("marginalIndices_", marginalIndices_);
  adv.saveAttribute("targetDistribution_", targetDistribution_);
  adv.saveAttribute("hasTargetDistribution_", hasTargetDistribution_);
  adv.saveAttribute("targetLogPDF_", targetLogPDF_);
  adv.saveAttribute("support_", support_);
  adv.saveAttribute("conditional_", conditional_);
  adv.saveAttribute("linkFunction_", linkFunction_);
  adv.saveAttribute("covariates_", covariates_);
  adv.saveAttribute("observations_", observations_);
  adv.saveAttribute("burnIn_", burnIn_);
  adv.saveAttribute("thinning_", thinning_);
  adv.saveAttribute("verbose_", verbose_);
}

/* Method load() reloads the object from the StorageManager */
void MetropolisHastingsImplementation::load(Advocate & adv)
{
  RandomVectorImplementation::load(adv);
  adv.loadAttribute("initialState_", initialState_);
  adv.loadAttribute("currentState_", currentState_);
  adv.loadAttribute("marginalIndices_", marginalIndices_);
  adv.loadAttribute("targetDistribution_", targetDistribution_);
  adv.loadAttribute("hasTargetDistribution_", hasTargetDistribution_);
  adv.loadAttribute("targetLogPDF_", targetLogPDF_);
  adv.loadAttribute("support_", support_);
  adv.loadAttribute("conditional_", conditional_);
  adv.loadAttribute("linkFunction_", linkFunction_);
  adv.loadAttribute("covariates_", covariates_);
  adv.loadAttribute("observations_", observations_);
  adv.loadAttribute("burnIn_", burnIn_);
  adv.loadAttribute("thinning_", thinning_);
  adv.loadAttribute("verbose_", verbose_);
}

END_NAMESPACE_OPENTURNS
