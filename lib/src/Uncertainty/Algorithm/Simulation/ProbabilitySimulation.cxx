//                                               -*- C++ -*-
/**
 *  @brief ProbabilitySimulation is a generic view of simulation methods for computing
 * probabilities and related quantities by sampling and estimation
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/ProbabilitySimulation.hxx"
#include "openturns/Log.hxx"
#include "openturns/Curve.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class ProbabilitySimulation
 */

CLASSNAMEINIT(ProbabilitySimulation);

static const Factory<ProbabilitySimulation> Factory_ProbabilitySimulation;


/* Constructor with parameters */
ProbabilitySimulation::ProbabilitySimulation(const Event & event,
                                             const WeightedExperiment & experiment,
                                             const Bool verbose,
                                             const HistoryStrategy & convergenceStrategy)
  : Simulation(event, verbose, convergenceStrategy)
{
  if (!getEvent().isComposite()) throw InvalidArgumentException(HERE) << "ProbabilitySimulation requires a composite event";
  setExperiment(experiment);
}

ProbabilitySimulation::ProbabilitySimulation(const Bool verbose,
                                             const HistoryStrategy & convergenceStrategy)
  : Simulation(verbose, convergenceStrategy)
{
  // Nothing to do
}

/* Virtual constructor */
ProbabilitySimulation * ProbabilitySimulation::clone() const
{
  return new ProbabilitySimulation(*this);
}


void ProbabilitySimulation::setExperiment(const WeightedExperiment & experiment)
{
  experiment_ = experiment;
  experiment_.setSize(blockSize_);
  experiment_.setDistribution(getEvent().getAntecedent()->getDistribution());
}


WeightedExperiment ProbabilitySimulation::getExperiment() const
{
  return experiment_;
}


/* String converter */
String ProbabilitySimulation::__repr__() const
{
  OSS oss;
  oss << "class=" << ProbabilitySimulation::GetClassName()
      << " experiment=" << experiment_
      << " derived from " << Simulation::__repr__();
  return oss;
}

/* Compute the block sample and the points that realized the event */
Sample ProbabilitySimulation::computeBlockSample()
{
  Point weights;
  const Sample inputSample(experiment_.generateWithWeights(weights));
  Sample blockSample(getEvent().getImplementation()->getFunction()(inputSample));
  for (UnsignedInteger i = 0; i < blockSize_; ++ i)
    blockSample[i][0] = getEvent().getDomain().contains(blockSample[i]);
  if (!experiment_.hasUniformWeights())
    for (UnsignedInteger i = 0; i < blockSize_; ++ i)
      blockSample[i][0] *= weights[i];
  return blockSample;
}


/* Method save() stores the object through the StorageManager */
void ProbabilitySimulation::save(Advocate & adv) const
{
  Simulation::save(adv);
  adv.saveAttribute("experiment_", experiment_);
}

/* Method load() reloads the object from the StorageManager */
void ProbabilitySimulation::load(Advocate & adv)
{
  Simulation::load(adv);
  adv.loadAttribute("experiment_", experiment_);
}

void ProbabilitySimulation::setBlockSize(const UnsignedInteger blockSize)
{
  Simulation::setBlockSize(blockSize);
  experiment_.setSize(blockSize);
}

END_NAMESPACE_OPENTURNS
