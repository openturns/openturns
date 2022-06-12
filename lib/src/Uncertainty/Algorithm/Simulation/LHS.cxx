//                                               -*- C++ -*-
/**
 *  @brief LHS is an implementation of the hit or miss Monte Carlo simulation method
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
#include "openturns/LHS.hxx"
#include "openturns/LHSExperiment.hxx"
#include "openturns/Point.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class LHS
 */

CLASSNAMEINIT(LHS)

static const Factory<LHS> Factory_LHS;

/* Default constructor */
LHS::LHS()
  : EventSimulation()
  , dimension_(0)
  , blockIndex_(0)
{
  // Nothing to do
}

/* Constructor with parameters */
LHS::LHS(const RandomVector & event)
  : EventSimulation(event)
  , dimension_(event.getImplementation()->getAntecedent().getDimension())
  , blockIndex_(0)
{
  if (!event.isComposite()) throw InvalidArgumentException(HERE) << "LHS requires a composite event";
  // Check if the distribution associated to the antecedent of the antecedent of the event has independent components
  if(!event.getImplementation()->getAntecedent().getDistribution().hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error the LHS simulation method requires independent components for the event second antecedent";
  // Get the marginals
  for (UnsignedInteger index = 0; index < dimension_; index++) marginals_.add(event.getImplementation()->getAntecedent().getDistribution().getMarginal(index));
}

/* Virtual constructor */
LHS * LHS::clone() const
{
  return new LHS(*this);
}

/* Compute the block sample */
Sample LHS::computeBlockSample()
{
  // Size of a block
  const UnsignedInteger blockSize = getBlockSize();
  // Compute the total sample size
  const UnsignedInteger totalSize = blockSize * getMaximumOuterSampling();
  // Compute the total sample base position
  UnsignedInteger basePosition = blockIndex_ * blockSize;
  // First, compute the input sub-sample based on the shuffling
  Sample inputSample(blockSize, Point(dimension_));
  for(UnsignedInteger index = 0; index < blockSize; ++index)
  {
    const Point u(RandomGenerator::Generate(dimension_));
    for(UnsignedInteger component = 0; component < dimension_; ++component)
    {
      Scalar xi = (shuffle_(component, basePosition) + u[component]) / totalSize;
      inputSample(index, component) = marginals_[component].computeQuantile(xi)[0];
    }
    // Update the base position
    ++basePosition;
  }
  // Then, evaluate the function on this sample
  Sample blockSample(getEvent().getImplementation()->getFunction()(inputSample));
  const DomainImplementation::BoolCollection realized(getEvent().getDomain().contains(blockSample));
  for (UnsignedInteger i = 0; i < blockSize; ++i) blockSample(i, 0) = realized[i];
  // Update the block index
  ++blockIndex_;
  return blockSample;
}

/* Performs the actual computation. */
void LHS::run()
{
  shuffle_ = LHSExperiment::ComputeShuffle(dimension_, getBlockSize() * getMaximumOuterSampling());
  EventSimulation::run();
}

/* String converter */
String LHS::__repr__() const
{
  OSS oss;
  oss << "class=" << LHS::GetClassName()
      << " derived from " << EventSimulation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LHS::save(Advocate & adv) const
{
  EventSimulation::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("blockIndex_", blockIndex_);
  adv.saveAttribute("shuffle_", shuffle_);
  adv.saveAttribute("marginals_", marginals_);
}

/* Method load() reloads the object from the StorageManager */
void LHS::load(Advocate & adv)
{
  EventSimulation::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("blockIndex_", blockIndex_);
  adv.loadAttribute("shuffle_", shuffle_);
  adv.loadAttribute("marginals_", marginals_);
}

END_NAMESPACE_OPENTURNS
