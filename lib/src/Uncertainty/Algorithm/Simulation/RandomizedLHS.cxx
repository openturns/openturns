//                                               -*- C++ -*-
/**
 *  @brief RandomizedLHS is an implementation of the hit or miss Monte Carlo simulation method
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
#include "RandomizedLHS.hxx"
#include "NumericalPoint.hxx"
#include "RandomVector.hxx"
#include "RandomGenerator.hxx"
#include "ComparisonOperatorImplementation.hxx"
#include "LHSExperiment.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS
/*
 * @class RandomizedLHS
 */

CLASSNAMEINIT(RandomizedLHS);

static Factory<RandomizedLHS> RegisteredFactory("RandomizedLHS");

/* Default constructor */
RandomizedLHS::RandomizedLHS():
  Simulation(),
  dimension_(0)
{
  // Nothing to do
}

/* Constructor with parameters */
RandomizedLHS::RandomizedLHS(const Event & event):
  Simulation(event),
  dimension_(event.getImplementation()->getAntecedent()->getDimension())
{
  // Get the marginals
  for (UnsignedInteger index = 0; index < dimension_; ++index) marginals_.add(event.getImplementation()->getAntecedent()->getDistribution().getMarginal(index));
}

/* Virtual constructor */
RandomizedLHS * RandomizedLHS::clone() const
{
  return new RandomizedLHS(*this);
}

/* Compute the block sample */
NumericalSample RandomizedLHS::computeBlockSample()
{
  // Size of a block
  const UnsignedInteger blockSize(getBlockSize());
  // Compute a shuffle of given dimension and blocksize
  const Matrix shuffle(LHSExperiment::ComputeShuffle(dimension_, blockSize));
  // First, compute the input sub-sample based on the shuffling
  NumericalSample inputSample(blockSize, NumericalPoint(dimension_));
  for(UnsignedInteger index = 0; index < blockSize; ++index)
  {
    const NumericalPoint u(RandomGenerator::Generate(dimension_));
    for(UnsignedInteger component = 0; component < dimension_; ++component)
    {
      NumericalScalar xi((shuffle(component, index) + u[component]) / blockSize);
      inputSample[index][component] = marginals_[component].computeQuantile(xi)[0];
    }
  }
  // Then, evaluate the function on this sample
  NumericalSample blockSample(getEvent().getImplementation()->getFunction()(inputSample));
  for (UnsignedInteger i = 0; i < blockSize; ++i) blockSample[i][0] = getEvent().getOperator()(blockSample[i][0], event_.getThreshold());
  return blockSample;
}

/* String converter */
String RandomizedLHS::__repr__() const
{
  OSS oss;
  oss << "class=" << RandomizedLHS::GetClassName()
      << " derived from " << Simulation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void RandomizedLHS::save(Advocate & adv) const
{
  Simulation::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("marginals_", marginals_);
}

/* Method load() reloads the object from the StorageManager */
void RandomizedLHS::load(Advocate & adv)
{
  Simulation::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("marginals_", marginals_);
}

END_NAMESPACE_OPENTURNS
