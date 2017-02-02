//                                               -*- C++ -*-
/**
 *  @brief LHS is an implementation of the hit or miss Monte Carlo simulation method
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
#include "openturns/LHS.hxx"
#include "openturns/LHSExperiment.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/RandomVector.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class LHS
 */

CLASSNAMEINIT(LHS);

static const Factory<LHS> Factory_LHS;

/* Default constructor */
LHS::LHS()
  : Simulation()
  , dimension_(0)
  , blockIndex_(0)
{
  // Nothing to do
}

/* Constructor with parameters */
LHS::LHS(const Event & event)
  : Simulation(event)
  , dimension_(event.getImplementation()->getAntecedent()->getDimension())
  , blockIndex_(0)
{
  // Check if the distribution associated to the antecedent of the antecedent of the event has independent components
  if(!event.getImplementation()->getAntecedent()->getDistribution().hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error the LHS simulation method requires independent components for the event second antecedent";
  // Get the marginals
  for (UnsignedInteger index = 0; index < dimension_; index++) marginals_.add(event.getImplementation()->getAntecedent()->getDistribution().getMarginal(index));
}

/* Virtual constructor */
LHS * LHS::clone() const
{
  return new LHS(*this);
}

/* Compute the block sample */
NumericalSample LHS::computeBlockSample()
{
  // Size of a block
  const UnsignedInteger blockSize = getBlockSize();
  // Compute the total sample size
  const UnsignedInteger totalSize = blockSize * getMaximumOuterSampling();
  // Compute the total sample base position
  UnsignedInteger basePosition = blockIndex_ * blockSize;
  // First, compute the input sub-sample based on the shuffling
  NumericalSample inputSample(blockSize, NumericalPoint(dimension_));
  for(UnsignedInteger index = 0; index < blockSize; ++index)
  {
    const NumericalPoint u(RandomGenerator::Generate(dimension_));
    for(UnsignedInteger component = 0; component < dimension_; ++component)
    {
      NumericalScalar xi = (shuffle_(component, basePosition) + u[component]) / totalSize;
      inputSample[index][component] = marginals_[component].computeQuantile(xi)[0];
    }
    // Update the base position
    ++basePosition;
  }
  // Then, evaluate the function on this sample
  NumericalSample blockSample(getEvent().getImplementation()->getFunction()(inputSample));
  for (UnsignedInteger i = 0; i < blockSize; ++i) blockSample[i][0] = getEvent().getOperator()(blockSample[i][0], event_.getThreshold());
  // Update the block index
  ++blockIndex_;
  return blockSample;
}

/* Performs the actual computation. */
void LHS::run()
{
  shuffle_ = LHSExperiment::ComputeShuffle(dimension_, getBlockSize() * getMaximumOuterSampling());
  Simulation::run();
}

/* String converter */
String LHS::__repr__() const
{
  OSS oss;
  oss << "class=" << LHS::GetClassName()
      << " derived from " << Simulation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void LHS::save(Advocate & adv) const
{
  Simulation::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("blockIndex_", blockIndex_);
  adv.saveAttribute("shuffle_", shuffle_);
  adv.saveAttribute("marginals_", marginals_);
}

/* Method load() reloads the object from the StorageManager */
void LHS::load(Advocate & adv)
{
  Simulation::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("blockIndex_", blockIndex_);
  adv.loadAttribute("shuffle_", shuffle_);
  adv.loadAttribute("marginals_", marginals_);
}

END_NAMESPACE_OPENTURNS
