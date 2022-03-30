//                                               -*- C++ -*-
/**
 *  @brief NAIS implement Non Parametric Adaptive Importance Sampling algorithm
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
#include "openturns/NAIS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NAIS
 */

CLASSNAMEINIT(NAIS);

static const Factory<NAIS> Factory_NAIS;

// Default constructor
NAIS::NAIS()
  : EventSimulation()
{
  // Nothing TO DO
}


// Default constructor
NAIS::NAIS(const RandomVector & event,
           const Scalar rhoQuantile)
  : EventSimulation(event)
  , initialDistribution_(getEvent().getAntecedent().getDistribution())
{
  const Interval range(initialDistribution_.getRange());
  const Interval::BoolCollection rangeUpper(range.getFiniteUpperBound());
  const Interval::BoolCollection rangeLower(range.getFiniteLowerBound());
  for (UnsignedInteger i = 0; i < rangeUpper.getSize(); ++i)
    if (rangeUpper[i] || rangeLower[i]) throw InvalidArgumentException(HERE) << "Current version of NAIS is only adapted to unbounded distribution" ;
  rhoQuantile_ = (getEvent().getOperator()(0, 1) ? rhoQuantile : 1.0 - rhoQuantile);
}

/* Virtual constructor */
NAIS * NAIS::clone() const
{
  return new NAIS(*this);
}
    
// Get weights
Point NAIS::getWeights() const
{
  return weights_;
}

// Get auxiliary input sample
Sample NAIS::getAuxiliaryInputSample() const
{
  return auxiliaryInputSample_;
}

// Get rhoQuantile
Scalar NAIS::getRhoQuantile() const
{
  return rhoQuantile_;
}

// Set rhoQuantile
void NAIS::setRhoQuantile(const Scalar & rhoQuantile)
{
  rhoQuantile_ = rhoQuantile;
}

// Get auxiliary output samples
Sample NAIS::getAuxiliaryOutputSample() const
{
  return auxiliaryOutputSample_;
}

// Function computing the auxiliary distribution as a function of current sample and associated weights_
Distribution NAIS::computeAuxiliaryDistribution(const Sample & sample,
    const Point & weights_)
{
  Scalar weightsPower2 = weights_.normSquare();
  Scalar sumWeights = 0.0;
  for (UnsignedInteger i = 0; i < weights_.getSize(); ++i)
  {
    sumWeights += weights_[i];
  }
  // Computation of weight
  const Scalar neff = sumWeights * sumWeights / weightsPower2;

  // Computation of bandwidth using Silverman rule
  const Point stdPerComponent(sample.computeStandardDeviation());
  const UnsignedInteger dimensionSample = getEvent().getAntecedent().getDimension();
  const Point silverman(stdPerComponent * std::pow(neff * (dimensionSample + 2.0) / 4.0, -1.0 / (dimensionSample + 4.0)));

  Collection<Distribution> margins(dimensionSample);

  // Computation of auxiliary distribution using ot.Mixture
  for (UnsignedInteger k = 0; k < dimensionSample ; ++k)
  {
    const UnsignedInteger numberOfSample = getMaximumOuterSampling() * getBlockSize();
    Collection<Distribution> collectionOfDistribution(numberOfSample);
    for (UnsignedInteger i = 0; i < numberOfSample ; ++i)
    {
      collectionOfDistribution[i] = Normal(sample(i, k), silverman[k]);
    }
    margins[k] = Mixture(collectionOfDistribution, weights_);
  } // for k
  return ComposedDistribution(margins);
}

// Function computing weigths of sample
Point NAIS::computeWeights(const Sample & sample,
                           const Sample & outputSample,
                           const Scalar eventThresholdLocal,
                           const Distribution & auxiliaryDistribution)
{
  Indices indiceCritic(0);
  for (UnsignedInteger i = 0; i < sample.getSize(); ++i)
  {
    const Bool weightBool = getEvent().getOperator()(outputSample(i, 0), eventThresholdLocal);
    if (weightBool)
      indiceCritic.add(i);
  } // for i

  // Extract the relevant sample
  const Sample criticalSample(sample.select(indiceCritic));
  // Compute initial distribution logPDF in parallel
  const Sample initialLogPDF = initialDistribution_.computeLogPDF(criticalSample);
  // Compute initial distribution logPDF in parallel
  const Sample auxilliaryLogPDF = auxiliaryDistribution.computeLogPDF(criticalSample);
  weights_ = Point(sample.getSize());
  for (UnsignedInteger i = 0; i < criticalSample.getSize(); ++i)
  {
    weights_[indiceCritic[i]] = std::exp(initialLogPDF(i,0) - auxilliaryLogPDF(i,0));
  }
  return weights_;
}

// Main function that computes the failure probability
void NAIS::run()
{
  const UnsignedInteger numberOfSample = getMaximumOuterSampling() * getBlockSize();

  // Drawing of samples using initial density
  auxiliaryInputSample_ = initialDistribution_.getSample(numberOfSample);

  // Evaluation on limit state function
  Sample auxiliaryOutputSample(getEvent().getFunction()(auxiliaryInputSample_));

  // Computation of current quantile
  Scalar currentQuantile = auxiliaryOutputSample.computeQuantile(rhoQuantile_)[0];
  Distribution auxiliaryDistribution;
  if (getEvent().getOperator()(currentQuantile, getEvent().getThreshold()))
  {
    currentQuantile = getEvent().getThreshold();
    auxiliaryDistribution = initialDistribution_;
  }
  else
  {
    // Computation of weights_
    weights_ = computeWeights(auxiliaryInputSample_, auxiliaryOutputSample, currentQuantile, initialDistribution_);

    // Computation of auxiliary distribution
    auxiliaryDistribution = computeAuxiliaryDistribution(auxiliaryInputSample_, weights_);
  }

  while ((getEvent().getOperator()(getEvent().getThreshold(), currentQuantile)) && (currentQuantile != getEvent().getThreshold()))
  {
    // Drawing of samples using auxiliary density and evaluation on limit state function   
    auxiliaryInputSample_ = Sample(0, initialDistribution_.getDimension());
    auxiliaryOutputSample = Sample(0, 1);

    for (UnsignedInteger i = 0; i < getMaximumOuterSampling(); ++ i)
    {
      const Sample blockSample(auxiliaryDistribution.getSample(getBlockSize()));
      auxiliaryInputSample_.add(blockSample);
      auxiliaryOutputSample.add(getEvent().getFunction()(blockSample));
      
      if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
        throw InternalException(HERE) << "User stopped simulation";
    } 
    
    // Computation of current quantile
    currentQuantile = auxiliaryOutputSample.computeQuantile(rhoQuantile_)[0];

    // If failure probability reached, stop the adaptation
    if (getEvent().getOperator()(currentQuantile, getEvent().getThreshold()))
    {
      currentQuantile = getEvent().getThreshold();
    }
    else
    {
      // Computation of weights_
      weights_ = computeWeights(auxiliaryInputSample_, auxiliaryOutputSample, currentQuantile, auxiliaryDistribution);

      // Update of auxiliary distribution
      auxiliaryDistribution = computeAuxiliaryDistribution(auxiliaryInputSample_, weights_);
    }
    
    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InternalException(HERE) << "User stopped simulation";
      
  } // while

  // Find failure sample indices
  Indices indicesCritic(0);
  for (UnsignedInteger i = 0; i < auxiliaryOutputSample.getSize(); ++i)
  {
    // Find failure Points
    if (getEvent().getOperator()(auxiliaryOutputSample(i, 0), getEvent().getThreshold()))
      indicesCritic.add(i);
  } // for i

  const Sample resp_sampleCritic(auxiliaryOutputSample.select(indicesCritic));
  const Sample sampleCritic(auxiliaryInputSample_.select(indicesCritic));

  // Evaluate initial log PDF in parallel on failure sample
  Sample logPDFInitCritic(initialDistribution_.computeLogPDF(sampleCritic));

  // Evaluate auxiliary log PDF in parallel on failure sample
  Sample logPDFAuxiliaryCritic(auxiliaryDistribution.computeLogPDF(sampleCritic));

  Scalar sumPdfCritic = 0.0;
  for(UnsignedInteger i = 0; i < indicesCritic.getSize(); ++i)
  {
    sumPdfCritic += std::exp(logPDFInitCritic(i, 0) - logPDFAuxiliaryCritic(i, 0));
  }
  Scalar failureProbability = sumPdfCritic / numberOfSample;
  Scalar varianceCritic = 0.0;
  Scalar varianceCriticTemporary;
  for(UnsignedInteger i = 0; i < indicesCritic.getSize(); ++i)
  {
    varianceCriticTemporary = std::exp(logPDFInitCritic(i, 0) - logPDFAuxiliaryCritic(i, 0))-failureProbability;
    varianceCritic += varianceCriticTemporary*varianceCriticTemporary;
  }  // for i 
  
  Scalar variancenonCritic = (numberOfSample -indicesCritic.getSize()) * (failureProbability*failureProbability);
  Scalar varianceEstimate = (varianceCritic + variancenonCritic)/(numberOfSample-1) ;

  // Save of data in Simulation naisResult_ structure
  naisResult_.setProbabilityEstimate(sumPdfCritic / numberOfSample);
  naisResult_.setAuxiliaryDistribution(auxiliaryDistribution);
  naisResult_.setAuxiliaryInputSample(auxiliaryInputSample_);
  naisResult_.setAuxiliaryOutputSample(auxiliaryOutputSample);
  naisResult_.setOuterSampling(getMaximumOuterSampling());
  naisResult_.setBlockSize(getBlockSize());
  naisResult_.setVarianceEstimate(varianceEstimate);
  auxiliaryOutputSample_ = auxiliaryOutputSample;

}



// Accessor to naisResult_s
NAISResult NAIS::getResult() const
{
  return naisResult_;
}

END_NAMESPACE_OPENTURNS
