//                                               -*- C++ -*-
/**
 *  @brief CrossEntropyImportanceSampling implement parent class for Cross Entropy Importance Sampling algorithms
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
#include "openturns/CrossEntropyImportanceSampling.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class CrossEntropyImportanceSampling
 */

CLASSNAMEINIT(CrossEntropyImportanceSampling)

static const Factory<CrossEntropyImportanceSampling> Factory_CrossEntropyImportanceSampling;

// Default constructor
CrossEntropyImportanceSampling::CrossEntropyImportanceSampling()
  : EventSimulation()
{
  // Nothing TO DO
}


// Default constructor
CrossEntropyImportanceSampling::CrossEntropyImportanceSampling(const RandomVector & event,
                                                               const Scalar rhoQuantile)
  : EventSimulation(event)
  , initialDistribution_(event.getAntecedent().getDistribution())
{
  rhoQuantile_ = (event.getOperator()(0, 1) ? rhoQuantile : 1.0 - rhoQuantile);
}

/* Virtual constructor */
CrossEntropyImportanceSampling * CrossEntropyImportanceSampling::clone() const
{
  return new CrossEntropyImportanceSampling(*this);
}

// Get rhoQuantile
Scalar CrossEntropyImportanceSampling::getRhoQuantile() const
{
  return rhoQuantile_;
}

// Set rhoQuantile
void CrossEntropyImportanceSampling::setRhoQuantile(const Scalar & rhoQuantile)
{
  rhoQuantile_ = rhoQuantile;
}

// Compute Output Samples
Sample CrossEntropyImportanceSampling::computeOutputSamples(const Sample & ) const
{
  throw NotYetImplementedException(HERE) << "In CrossEntropyImportanceSampling::computeOutputSamples(const Sample & inputSamples)";
}

// Update auxiliary distribution
void CrossEntropyImportanceSampling::updateAuxiliaryDistribution(const Point & )
{
  throw NotYetImplementedException(HERE) << "In CrossEntropyImportanceSampling::updateAuxiliaryDistribution(const Point & auxiliaryDistributionParameters)";
} 
  
// Optimize auxiliary distribution parameters
Point CrossEntropyImportanceSampling::optimizeAuxiliaryDistributionParameters(const Sample & ) const
{
  throw NotYetImplementedException(HERE) << "In CrossEntropyImportanceSampling::optimizeAuxiliaryDistributionParameters(const Sample &)";
} 
  


// Main function that computes the failure probability
void CrossEntropyImportanceSampling::run()
{
  const UnsignedInteger numberOfSample = getMaximumOuterSampling() * getBlockSize();

  // Drawing of samples using initial density
  Sample auxiliaryInputSample = auxiliaryDistribution_.getSample(numberOfSample);

  // Evaluation on limit state function
  Sample auxiliaryOutputSample = computeOutputSamples(auxiliaryInputSample);
  
  // Computation of current quantile
  Scalar currentQuantile = auxiliaryOutputSample.computeQuantile(rhoQuantile_)[0];
  
  Point  auxiliaryDistributionParameters;

  if (getEvent().getOperator()(currentQuantile, getEvent().getThreshold()))
  {
    currentQuantile = getEvent().getThreshold();
  }
  else
  {
    Indices indiceCritic(0);

    for (UnsignedInteger i = 0; i < auxiliaryInputSample.getSize(); ++i)
      {
        const Bool weightBool = getEvent().getOperator()(auxiliaryOutputSample(i, 0), currentQuantile);
        if (weightBool)
          indiceCritic.add(i);
      } 

     const Sample auxiliaryCriticInputSamples(auxiliaryInputSample.select(indiceCritic));
     
    // Optimize auxiliary distribution parameters
    auxiliaryDistributionParameters = optimizeAuxiliaryDistributionParameters(auxiliaryCriticInputSamples);

    // Update auxiliary Distribution Parameters
    updateAuxiliaryDistribution(auxiliaryDistributionParameters); 
  } // if (getEvent().getOperator()(currentQuantile, getEvent().getThreshold()))

  UnsignedInteger iterationNumber  = 0;
  
  while ((getEvent().getOperator()(getEvent().getThreshold(), currentQuantile)) && (currentQuantile != getEvent().getThreshold()))
  {
    ++iterationNumber ;
    Point currentAuxiliaryDistributionParameters = auxiliaryDistributionParameters;

    // Drawing of samples using auxiliary density and evaluation on limit state function
    auxiliaryInputSample = Sample(0, initialDistribution_.getDimension());
    auxiliaryOutputSample = Sample(0, 1);



    for (UnsignedInteger i = 0; i < getMaximumOuterSampling(); ++i)
    {
      const Sample blockSample(auxiliaryDistribution_.getSample(getBlockSize()));
      auxiliaryInputSample.add(blockSample);
      auxiliaryOutputSample.add(computeOutputSamples(blockSample));

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
    Indices indiceCritic(0);
    for (UnsignedInteger i = 0; i < auxiliaryInputSample.getSize(); ++i)
      {
        const Bool weightBool = getEvent().getOperator()(auxiliaryOutputSample(i, 0), currentQuantile);
        if (weightBool)
          indiceCritic.add(i);
      } 

   // Extract the relevant sample
   const Sample auxiliaryCriticInputSamples(auxiliaryInputSample.select(indiceCritic));
  
  
   // Optimize auxiliary distribution parameters
   Point auxiliaryDistributionParameters = optimizeAuxiliaryDistributionParameters(auxiliaryCriticInputSamples);
  
   // Update auxiliary Distribution Parameters
   updateAuxiliaryDistribution(auxiliaryDistributionParameters); 

    //throw InternalException(HERE) << "User stopped simulation"; 
      
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
  const Sample sampleCritic(auxiliaryInputSample.select(indicesCritic));

  // Evaluate initial log PDF in parallel on failure sample
  Sample logPDFInitCritic(initialDistribution_.computeLogPDF(sampleCritic));

  // Evaluate auxiliary log PDF in parallel on failure sample
  Sample logPDFAuxiliaryCritic(auxiliaryDistribution_.computeLogPDF(sampleCritic));

  Scalar sumPdfCritic = 0.0;
  for(UnsignedInteger i = 0; i < indicesCritic.getSize(); ++i)
  {
    sumPdfCritic += std::exp(logPDFInitCritic(i, 0) - logPDFAuxiliaryCritic(i, 0));
  }
  const Scalar failureProbability = sumPdfCritic / numberOfSample;
  Scalar varianceCritic = 0.0;
  for(UnsignedInteger i = 0; i < indicesCritic.getSize(); ++i)
  {
    const Scalar varianceCriticTemporary = std::exp(logPDFInitCritic(i, 0) - logPDFAuxiliaryCritic(i, 0)) - failureProbability;
    varianceCritic += varianceCriticTemporary * varianceCriticTemporary;
  }  // for i

  const Scalar variancenonCritic = (numberOfSample - indicesCritic.getSize()) * (failureProbability * failureProbability);
  const Scalar varianceEstimate = (varianceCritic + variancenonCritic) / (numberOfSample - 1) / numberOfSample ;

  // Save of data in Simulation crossEntropyResult_ structure
  crossEntropyResult_.setProbabilityEstimate(failureProbability);
  crossEntropyResult_.setAuxiliaryDistribution(auxiliaryDistribution_);
  crossEntropyResult_.setAuxiliaryInputSample(auxiliaryInputSample);
  crossEntropyResult_.setAuxiliaryOutputSample(auxiliaryOutputSample);
  crossEntropyResult_.setOuterSampling(getMaximumOuterSampling() * iterationNumber);
  crossEntropyResult_.setBlockSize(getBlockSize());
  crossEntropyResult_.setVarianceEstimate(varianceEstimate);
}



// Accessor to CrossEntropyImportanceSampling Result_s
CrossEntropyResult CrossEntropyImportanceSampling::getResult() const
{
  return crossEntropyResult_;
}

END_NAMESPACE_OPENTURNS
