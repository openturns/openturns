//                                               -*- C++ -*-
/**
 *  @brief ProbabilitySimulationAlgorithm is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ProbabilitySimulationAlgorithm.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Point.hxx"
#include "openturns/MonteCarloExperiment.hxx"
BEGIN_NAMESPACE_OPENTURNS

/*
 * @class ProbabilitySimulationAlgorithm
 */

CLASSNAMEINIT(ProbabilitySimulationAlgorithm)

static const Factory<ProbabilitySimulationAlgorithm> Factory_ProbabilitySimulationAlgorithm;

ProbabilitySimulationAlgorithm::ProbabilitySimulationAlgorithm(const HistoryStrategy & convergenceStrategy)
  : EventSimulation(convergenceStrategy)
{
  // Nothing to do
}

/* Constructor with parameters */
ProbabilitySimulationAlgorithm::ProbabilitySimulationAlgorithm(const RandomVector & event,
    const HistoryStrategy & convergenceStrategy)
  : EventSimulation(event, convergenceStrategy)
{
  // Filter out if the event is composite
  if (event_.isComposite())
  {
    isExperimentProvided_ = true;
    setExperiment(MonteCarloExperiment());
  }
}

/* Constructor with parameters */
ProbabilitySimulationAlgorithm::ProbabilitySimulationAlgorithm(const RandomVector & event,
    const WeightedExperiment & experiment,
    const HistoryStrategy & convergenceStrategy)
  : EventSimulation(event, convergenceStrategy)
  , isExperimentProvided_(true)
{
  if (!event.isComposite()) throw InvalidArgumentException(HERE) << "ProbabilitySimulationAlgorithm requires a composite event";
  setExperiment(experiment);
}

/* Virtual constructor */
ProbabilitySimulationAlgorithm * ProbabilitySimulationAlgorithm::clone() const
{
  return new ProbabilitySimulationAlgorithm(*this);
}


void ProbabilitySimulationAlgorithm::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  experiment_.setSize(blockSize_);
  experiment_.setDistribution(getEvent().getAntecedent().getDistribution());
}


WeightedExperiment ProbabilitySimulationAlgorithm::getExperiment() const
{
  return experiment_;
}

Sample ProbabilitySimulationAlgorithm::getInputSample() const
{
  return inputSample_;
}

Sample ProbabilitySimulationAlgorithm::getOutputSample() const
{
  return outputSample_;
}

/* String converter */
String ProbabilitySimulationAlgorithm::__repr__() const
{
  OSS oss;
  oss << "class=" << ProbabilitySimulationAlgorithm::GetClassName()
      << " experiment=" << experiment_
      << " derived from " << EventSimulation::__repr__();
  return oss;
}

/* Compute the block sample and the points that realized the event */
Sample ProbabilitySimulationAlgorithm::computeBlockSample()
{
  if (isExperimentProvided_)
    return computeBlockSampleComposite();
  return event_.getSample(blockSize_);
}

/* Keep event sample */
void ProbabilitySimulationAlgorithm::setKeepSample(const Bool keepSample)
{
  if ((!getEvent().isComposite()) && keepSample)
    throw InvalidArgumentException(HERE) << "ProbabilitySimulationAlgorithm::setKeepSample is only available for composite events";
  keepSample_ = keepSample;
  // Clear previous samples
  inputSample_ = Sample(0, getEvent().getFunction().getInputDimension());
  outputSample_ = Sample(0, getEvent().getFunction().getOutputDimension());
}

Sample ProbabilitySimulationAlgorithm::computeBlockSampleComposite()
{
  Point weights;
  const Sample inputSample(experiment_.generateWithWeights(weights));
  Sample blockSample(getEvent().getFrozenSample(inputSample));
  if (!experiment_.hasUniformWeights())
    for (UnsignedInteger i = 0; i < blockSize_; ++ i)
      blockSample(i, 0) *= weights[i];
  // Store output sample
  if(keepSample_)
  {
    inputSample_.add(inputSample);
    outputSample_.add(blockSample);
  }
  return blockSample;
}

/* Method save() stores the object through the StorageManager */
void ProbabilitySimulationAlgorithm::save(Advocate & adv) const
{
  EventSimulation::save(adv);
  adv.saveAttribute("experiment_", experiment_);
  adv.saveAttribute("isExperimentProvided_", isExperimentProvided_);
  adv.saveAttribute("keepSample_", keepSample_);
  adv.saveAttribute("inputSample_", inputSample_);
  adv.saveAttribute("outputSample_", outputSample_);
}

/* Method load() reloads the object from the StorageManager */
void ProbabilitySimulationAlgorithm::load(Advocate & adv)
{
  EventSimulation::load(adv);
  adv.loadAttribute("experiment_", experiment_);
  adv.loadAttribute("isExperimentProvided_", isExperimentProvided_);
  if (adv.hasAttribute("keepSample_")) // OT>=1.26
  {
    adv.loadAttribute("keepSample_", keepSample_);
    adv.loadAttribute("inputSample_", inputSample_);
    adv.loadAttribute("outputSample_", outputSample_);
  }
}

void ProbabilitySimulationAlgorithm::setBlockSize(const UnsignedInteger blockSize)
{
  EventSimulation::setBlockSize(blockSize);
  experiment_.setSize(blockSize);
}

END_NAMESPACE_OPENTURNS
