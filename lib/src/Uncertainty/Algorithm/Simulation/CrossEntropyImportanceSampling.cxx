//                                               -*- C++ -*-
/**
 *  @brief CrossEntropyImportanceSampling implement parent class for Cross Entropy Importance Sampling algorithms
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
    const Scalar quantileLevel)
  : EventSimulation(event.getImplementation()->asComposedEvent())
  , initialDistribution_(getEvent().getAntecedent().getDistribution())
{
  if (quantileLevel > 1.)
    throw InvalidArgumentException(HERE) << "In CrossEntropyImportanceSampling::CrossEntropyImportanceSampling, quantileLevel parameter value should be between 0.0 and 1.0";

  if (quantileLevel < 0.)
    throw InvalidArgumentException(HERE) << "In CrossEntropyImportanceSampling::CrossEntropyImportanceSampling, quantileLevel parameter value should be between 0.0 and 1.0";


  quantileLevel_ = (getEvent().getOperator()(0, 1) ? quantileLevel : 1.0 - quantileLevel);
}

/* Virtual constructor */
CrossEntropyImportanceSampling * CrossEntropyImportanceSampling::clone() const
{
  return new CrossEntropyImportanceSampling(*this);
}

// Get quantileLevel
Scalar CrossEntropyImportanceSampling::getQuantileLevel() const
{
  return quantileLevel_;
}

// Set quantileLevel
void CrossEntropyImportanceSampling::setQuantileLevel(const Scalar & quantileLevel)
{
  quantileLevel_ = quantileLevel;
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


// Reset auxiliary distribution parameters
void CrossEntropyImportanceSampling::resetAuxiliaryDistribution()
{
  throw NotYetImplementedException(HERE) << "In CrossEntropyImportanceSampling::resetAuxiliaryDistribution()";
}

// Main function that computes the failure probability
void CrossEntropyImportanceSampling::run()
{

  // First, initialize some parameters
  inputSample_.clear();
  outputSample_.clear();
  thresholdPerStep_.clear();

  // Initialization of auxiliary distribution (in case of multiple runs of algorithms)
  resetAuxiliaryDistribution();

  const UnsignedInteger sampleSize = getMaximumOuterSampling() * getBlockSize();

  // Drawing of samples using initial density
  Sample auxiliaryInputSample = auxiliaryDistribution_.getSample(sampleSize);

  // Evaluation on limit state function
  Sample auxiliaryOutputSample = computeOutputSamples(auxiliaryInputSample);

  // Computation of current quantile
  Scalar currentQuantile = auxiliaryOutputSample.computeQuantile(quantileLevel_)[0];

  ++ numberOfSteps_;
  if (keepSample_)
  {
    inputSample_.add(auxiliaryInputSample);
    outputSample_.add(auxiliaryOutputSample);
  }
    
    
  Point auxiliaryDistributionParameters;

  const ComparisonOperator comparator(getEvent().getOperator());
  const Scalar threshold = getEvent().getThreshold();

  if (comparator(currentQuantile, threshold))
  {
    currentQuantile = threshold;
    thresholdPerStep_.add(currentQuantile);
  }
  else
  {
    thresholdPerStep_.add(currentQuantile);
    Indices indiceCritic(0);

    for (UnsignedInteger i = 0; i < auxiliaryInputSample.getSize(); ++i)
    {
      const Bool weightBool = comparator(auxiliaryOutputSample(i, 0), currentQuantile);
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

  while ((comparator(threshold, currentQuantile)) && (currentQuantile != threshold))
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
    } // for i


    ++ numberOfSteps_;
    if (keepSample_)
    {
      inputSample_.add(auxiliaryInputSample);
      outputSample_.add(auxiliaryOutputSample);
    }
    
    // Computation of current quantile
    currentQuantile = auxiliaryOutputSample.computeQuantile(quantileLevel_)[0];

    // If failure probability reached, stop the adaptation
    if (comparator(currentQuantile, threshold))
    {
      currentQuantile = threshold;
      thresholdPerStep_.add(currentQuantile);
    }
    else
    {
      thresholdPerStep_.add(currentQuantile);
      Indices indiceCritic(0);
      for (UnsignedInteger i = 0; i < auxiliaryInputSample.getSize(); ++i)
      {
        const Bool weightBool = comparator(auxiliaryOutputSample(i, 0), currentQuantile);
        if (weightBool)
          indiceCritic.add(i);
      }

      // Extract the relevant sample
      const Sample auxiliaryCriticInputSamples(auxiliaryInputSample.select(indiceCritic));

      // Optimize auxiliary distribution parameters
      auxiliaryDistributionParameters = optimizeAuxiliaryDistributionParameters(auxiliaryCriticInputSamples);

      // Update auxiliary Distribution Parameters
      updateAuxiliaryDistribution(auxiliaryDistributionParameters);


    } // if comparator(currentQuantile, threshold)

    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InternalException(HERE) << "User stopped simulation";

  } // while

  // Find failure sample indices
  Indices indicesCritic(0);
  for (UnsignedInteger i = 0; i < auxiliaryOutputSample.getSize(); ++i)
  {
    // Find failure Points
    if (comparator(auxiliaryOutputSample(i, 0), threshold))
      indicesCritic.add(i);
  } // for i


  const Sample inputSampleCritic(auxiliaryInputSample.select(indicesCritic));

  // Evaluate initial log PDF in parallel on failure sample
  const Sample logPDFInitCritic(initialDistribution_.computeLogPDF(inputSampleCritic));

  // Evaluate auxiliary log PDF in parallel on failure sample
  const Sample logPDFAuxiliaryCritic(auxiliaryDistribution_.computeLogPDF(inputSampleCritic));

  Scalar sumPdfCritic = 0.0;
  for(UnsignedInteger i = 0; i < indicesCritic.getSize(); ++i)
  {
    sumPdfCritic += std::exp(logPDFInitCritic(i, 0) - logPDFAuxiliaryCritic(i, 0));
  }
  const Scalar failureProbability = sumPdfCritic / sampleSize;
  Scalar varianceCritic = 0.0;
  for(UnsignedInteger i = 0; i < indicesCritic.getSize(); ++i)
  {
    const Scalar varianceCriticTemporary = std::exp(logPDFInitCritic(i, 0) - logPDFAuxiliaryCritic(i, 0)) - failureProbability;
    varianceCritic += pow(varianceCriticTemporary, 2);
  }  // for i

  const Scalar variancenonCritic = (sampleSize - indicesCritic.getSize()) * (failureProbability * failureProbability);


  if (sampleSize == 1)
    throw InvalidArgumentException(HERE) << "In CrossEntropyImportanceSampling::run, sample size has to be greater than one for variance estimation";



  const Scalar varianceEstimate = (varianceCritic + variancenonCritic) / (sampleSize - 1) / sampleSize ;

  // Save of data in Simulation crossEntropyResult_ structure
  crossEntropyResult_.setProbabilityEstimate(failureProbability);
  crossEntropyResult_.setAuxiliaryDistribution(auxiliaryDistribution_);
  crossEntropyResult_.setAuxiliaryInputSample(auxiliaryInputSample);
  crossEntropyResult_.setAuxiliaryOutputSample(auxiliaryOutputSample);
  crossEntropyResult_.setOuterSampling(getMaximumOuterSampling() * (iterationNumber + 1));
  crossEntropyResult_.setBlockSize(getBlockSize());
  crossEntropyResult_.setVarianceEstimate(varianceEstimate);
}

// Accessor to CrossEntropyImportanceSampling Result_s
CrossEntropyResult CrossEntropyImportanceSampling::getResult() const
{
  return crossEntropyResult_;
}


Point CrossEntropyImportanceSampling::getThresholdPerStep() const
{
  return thresholdPerStep_;
}

UnsignedInteger CrossEntropyImportanceSampling::getStepsNumber() const
{
  return numberOfSteps_;
}

/* Event input/output sample accessor */
Sample CrossEntropyImportanceSampling::getInputSample(const UnsignedInteger step, const UnsignedInteger select) const
{
  if (!keepSample_)
    throw InvalidArgumentException(HERE) << "CrossEntropy keepSample was not set";
  if (step >= getStepsNumber())
    throw InvalidArgumentException(HERE) << "CrossEntropy step index (" << step << ") should be < " << getStepsNumber();
  if (select > 2)
    throw InvalidArgumentException(HERE) << "CrossEntropy select flag (" << select << ") must be in [0-2]";
  return (select == 2) ? inputSample_[step] : inputSample_[step].select(getSampleIndices(step, (select == EVENT1)));
}

Sample CrossEntropyImportanceSampling::getOutputSample(const UnsignedInteger step, const UnsignedInteger select) const
{
  if (!keepSample_)
    throw InvalidArgumentException(HERE) << "CrossEntropy keepSample was not set";
  if (step >= getStepsNumber())
    throw InvalidArgumentException(HERE) << "CrossEntropy step index (" << step << ") should be < " << getStepsNumber();
  if (select > 2)
    throw InvalidArgumentException(HERE) << "CrossEntropy select flag (" << select << ") must be in [0-2]";
  return (select == 2) ? outputSample_[step] : outputSample_[step].select(getSampleIndices(step, (select == EVENT1)));
}

Indices CrossEntropyImportanceSampling::getSampleIndices(const UnsignedInteger step, const Bool status) const
{
  Indices result;
  const Sample outputSample(outputSample_[step]);
  const Scalar threshold = getThresholdPerStep()[step];
  for (UnsignedInteger i = 0; i < outputSample.getSize(); ++ i)
    if (getEvent().getOperator()(outputSample(i, 0), threshold) == status)
      result.add(i);
  return result;
}


END_NAMESPACE_OPENTURNS
