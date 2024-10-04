//                                               -*- C++ -*-
/**
 *  @brief NAIS implement Non Parametric Adaptive Importance Sampling algorithm
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
#include <chrono>

#include "openturns/NAIS.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NAIS
 */

CLASSNAMEINIT(NAIS)

static const Factory<NAIS> Factory_NAIS;

// Default constructor
NAIS::NAIS()
  : EventSimulation()
{
  // Nothing TO DO
}


// Default constructor
NAIS::NAIS(const RandomVector & event,
           const Scalar quantileLevel)
  : EventSimulation(event.getImplementation()->asComposedEvent())
  , initialDistribution_(getEvent().getAntecedent().getDistribution())
  , quantileLevel_(getEvent().getOperator()(0, 1) ? quantileLevel : 1.0 - quantileLevel)
{
  const Interval range(initialDistribution_.getRange());
  const Interval::BoolCollection rangeUpper(range.getFiniteUpperBound());
  const Interval::BoolCollection rangeLower(range.getFiniteLowerBound());
  for (UnsignedInteger i = 0; i < rangeUpper.getSize(); ++i)
    if (rangeUpper[i] || rangeLower[i]) throw InvalidArgumentException(HERE) << "Current version of NAIS is only adapted to unbounded distribution";
}

/* Virtual constructor */
NAIS * NAIS::clone() const
{
  return new NAIS(*this);
}

/* Keep event sample */
void NAIS::setKeepSample(const Bool keepSample)
{
  keepSample_ = keepSample;
}


// Get quantileLevel
Scalar NAIS::getQuantileLevel() const
{
  return quantileLevel_;
}

// Set quantileLevel
void NAIS::setQuantileLevel(const Scalar & quantileLevel)
{
  quantileLevel_ = quantileLevel;
}

// Function computing the auxiliary distribution as a function of current sample and associated weights
Distribution NAIS::computeAuxiliaryDistribution(const Sample & sample,
    const Point & weights)
{
  Scalar weightsPower2 = weights.normSquare();
  Scalar sumWeights = 0.0;
  for (UnsignedInteger i = 0; i < weights.getSize(); ++i)
  {
    sumWeights += weights[i];
  }
  // Computation of weight
  const Scalar neff = sumWeights * sumWeights / weightsPower2;

  // Computation of bandwidth using Silverman rule
  const Point stdPerComponent(sample.computeStandardDeviation());
  const UnsignedInteger dimensionSample = getEvent().getAntecedent().getDimension();
  const Point silverman(stdPerComponent * std::pow(neff * (dimensionSample + 2.0) / 4.0, -1.0 / (dimensionSample + 4.0)));

  // Computation of auxiliary distribution using ot.Mixture
  const UnsignedInteger sampleSize = getMaximumOuterSampling() * getBlockSize();
  Collection<Distribution> collectionOfDistribution(sampleSize);
  for (UnsignedInteger i = 0; i < sampleSize ; ++i)
  {
    const Point meanNormal(sample[i]);

    collectionOfDistribution[i] = Normal(meanNormal, silverman);
  }
  const Mixture auxiliaryDistribution(collectionOfDistribution, weights);

  return auxiliaryDistribution;
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
  Point weights = Point(sample.getSize());
  for (UnsignedInteger i = 0; i < criticalSample.getSize(); ++i)
  {
    weights[indiceCritic[i]] = std::exp(initialLogPDF(i, 0) - auxilliaryLogPDF(i, 0));
  }
  return weights;
}

// Main function that computes the failure probability
void NAIS::run()
{

  // First, initialize some parameters
  inputSample_.clear();
  outputSample_.clear();
  thresholdPerStep_.clear();
  numberOfSteps_ = 0;
  result_ = NAISResult();

  const UnsignedInteger sampleSize = getMaximumOuterSampling() * getBlockSize();
  if (sampleSize < 2)
    throw InvalidArgumentException(HERE) << "In CrossEntropyImportanceSampling::run, sample size has to be greater than one for variance estimation";

  Sample auxiliaryInputSample(0, initialDistribution_.getDimension());
  Sample auxiliaryOutputSample(0, 1);

  std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();

  // initial experiment evaluation with the initial distribution
  for (UnsignedInteger i = 0; i < getMaximumOuterSampling(); ++ i)
  {
    const Sample blockSample(initialDistribution_.getSample(getBlockSize()));
    auxiliaryInputSample.add(blockSample);
    auxiliaryOutputSample.add(getEvent().getFunction()(blockSample));

    result_.setOuterSampling(i + 1);

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0).count();
    result_.setTimeDuration(timeDuration);
    if ((getMaximumTimeDuration() > 0.0) && (timeDuration > getMaximumTimeDuration()))
      throw TimeoutException(HERE) << "Duration (" << timeDuration << "s) exceeds maximum duration (" << getMaximumTimeDuration() << " s)";

    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InternalException(HERE) << "User stopped simulation";
  } // for i

  const ComparisonOperator comparator(getEvent().getOperator());

  ++ numberOfSteps_;

  if (keepSample_)
  {
    inputSample_.add(auxiliaryInputSample);
    outputSample_.add(auxiliaryOutputSample);
  }


  // Computation of current quantile
  Scalar currentQuantile = auxiliaryOutputSample.computeQuantile(quantileLevel_)[0];

  Point weights;
  Distribution auxiliaryDistribution;
  if (comparator(currentQuantile, getEvent().getThreshold()))
  {
    currentQuantile = getEvent().getThreshold();
    thresholdPerStep_.add(currentQuantile);
    auxiliaryDistribution = initialDistribution_;
  }
  else
  {
    thresholdPerStep_.add(currentQuantile);
    // Computation of weights
    weights = computeWeights(auxiliaryInputSample, auxiliaryOutputSample, currentQuantile, initialDistribution_);

    // Computation of auxiliary distribution
    auxiliaryDistribution = computeAuxiliaryDistribution(auxiliaryInputSample, weights);
  }

  while ((comparator(getEvent().getThreshold(), currentQuantile)) && (currentQuantile != getEvent().getThreshold()))
  {
    // Drawing of samples using auxiliary density and evaluation on limit state function
    auxiliaryInputSample = Sample(0, initialDistribution_.getDimension());
    auxiliaryOutputSample = Sample(0, 1);

    for (UnsignedInteger i = 0; i < getMaximumOuterSampling(); ++ i)
    {
      const Sample blockSample(auxiliaryDistribution.getSample(getBlockSize()));
      auxiliaryInputSample.add(blockSample);
      auxiliaryOutputSample.add(getEvent().getFunction()(blockSample));

      result_.setOuterSampling(numberOfSteps_ * getMaximumOuterSampling() + (i + 1));

      std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
      const Scalar timeDuration = std::chrono::duration<Scalar>(t1 - t0).count();
      result_.setTimeDuration(timeDuration);
      if ((getMaximumTimeDuration() > 0.0) && (timeDuration > getMaximumTimeDuration()))
        throw TimeoutException(HERE) << "Duration (" << timeDuration << "s) exceeds maximum duration (" << getMaximumTimeDuration() << " s)";

      if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
        throw InternalException(HERE) << "User stopped simulation";
    }

    if (keepSample_)
    {
      inputSample_.add(auxiliaryInputSample);
      outputSample_.add(auxiliaryOutputSample);
    }

    // Computation of current quantile
    currentQuantile = auxiliaryOutputSample.computeQuantile(quantileLevel_)[0];

    // If failure probability reached, stop the adaptation
    if (comparator(currentQuantile, getEvent().getThreshold()))
    {
      currentQuantile = getEvent().getThreshold();
      thresholdPerStep_.add(currentQuantile);
    }
    else
    {
      thresholdPerStep_.add(currentQuantile);
      // Computation of weights
      weights = computeWeights(auxiliaryInputSample, auxiliaryOutputSample, currentQuantile, auxiliaryDistribution);

      // Update of auxiliary distribution
      auxiliaryDistribution = computeAuxiliaryDistribution(auxiliaryInputSample, weights);
    }

    ++ numberOfSteps_;

    if (stopCallback_.first && stopCallback_.first(stopCallback_.second))
      throw InternalException(HERE) << "User stopped simulation";

  } // while

  // Find failure sample indices
  Indices indicesCritic(0);
  for (UnsignedInteger i = 0; i < auxiliaryOutputSample.getSize(); ++i)
  {
    // Find failure Points
    if (comparator(auxiliaryOutputSample(i, 0), getEvent().getThreshold()))
      indicesCritic.add(i);
  } // for i

  const Sample sampleCritic(auxiliaryInputSample.select(indicesCritic));

  // Evaluate initial log PDF in parallel on failure sample
  Sample logPDFInitCritic(initialDistribution_.computeLogPDF(sampleCritic));

  // Evaluate auxiliary log PDF in parallel on failure sample
  Sample logPDFAuxiliaryCritic(auxiliaryDistribution.computeLogPDF(sampleCritic));

  Scalar sumPdfCritic = 0.0;
  for(UnsignedInteger i = 0; i < indicesCritic.getSize(); ++ i)
  {
    sumPdfCritic += std::exp(logPDFInitCritic(i, 0) - logPDFAuxiliaryCritic(i, 0));
  }
  const Scalar failureProbability = sumPdfCritic / sampleSize;
  Scalar varianceCritic = 0.0;
  for(UnsignedInteger i = 0; i < indicesCritic.getSize(); ++ i)
  {
    const Scalar varianceCriticTemporary = std::exp(logPDFInitCritic(i, 0) - logPDFAuxiliaryCritic(i, 0)) - failureProbability;
    varianceCritic += varianceCriticTemporary * varianceCriticTemporary;
  }  // for i

  const Scalar variancenonCritic = (sampleSize - indicesCritic.getSize()) * (failureProbability * failureProbability);
  const Scalar varianceEstimate = (varianceCritic + variancenonCritic) / (sampleSize - 1) / sampleSize ;

  // Save of data in Simulation naisResult_ structure
  naisResult_.setProbabilityEstimate(failureProbability);
  naisResult_.setAuxiliaryDistribution(auxiliaryDistribution);
  naisResult_.setAuxiliaryInputSample(auxiliaryInputSample);
  naisResult_.setAuxiliaryOutputSample(auxiliaryOutputSample);
  naisResult_.setWeights(weights);
  naisResult_.setOuterSampling(getMaximumOuterSampling() * numberOfSteps_);
  naisResult_.setBlockSize(getBlockSize());
  naisResult_.setVarianceEstimate(varianceEstimate);

}

/* Event input/output sample accessor */
Sample NAIS::getInputSample(const UnsignedInteger step, const UnsignedInteger select) const
{
  if (!keepSample_)
    throw InvalidArgumentException(HERE) << "NAIS keepSample was not set";
  if (step >= getStepsNumber())
    throw InvalidArgumentException(HERE) << "NAIS step index (" << step << ") should be < " << getStepsNumber();
  if (select > 2)
    throw InvalidArgumentException(HERE) << "NAIS select flag (" << select << ") must be in [0-2]";
  return (select == 2) ? inputSample_[step] : inputSample_[step].select(getSampleIndices(step, (select == EVENT1)));
}

Sample NAIS::getOutputSample(const UnsignedInteger step, const UnsignedInteger select) const
{
  if (!keepSample_)
    throw InvalidArgumentException(HERE) << "NAIS keepSample was not set";
  if (step >= getStepsNumber())
    throw InvalidArgumentException(HERE) << "NAIS step index (" << step << ") should be < " << getStepsNumber();
  if (select > 2)
    throw InvalidArgumentException(HERE) << "NAIS select flag (" << select << ") must be in [0-2]";
  return (select == 2) ? outputSample_[step] : outputSample_[step].select(getSampleIndices(step, (select == EVENT1)));
}

Indices NAIS::getSampleIndices(const UnsignedInteger step, const Bool status) const
{
  Indices result;
  const Sample outputSample(outputSample_[step]);
  const Scalar threshold = getThresholdPerStep()[step];
  for (UnsignedInteger i = 0; i < outputSample.getSize(); ++ i)
    if (getEvent().getOperator()(outputSample(i, 0), threshold) == status)
      result.add(i);
  return result;
}

Point NAIS::getThresholdPerStep() const
{
  return thresholdPerStep_;
}

UnsignedInteger NAIS::getStepsNumber() const
{
  return numberOfSteps_;
}


// Accessor to naisResult_s
NAISResult NAIS::getResult() const
{
  return naisResult_;
}

END_NAMESPACE_OPENTURNS
