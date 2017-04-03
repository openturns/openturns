//                                               -*- C++ -*-
/**
 *  @brief Monte Carlo simulation using low-discrepancy sequences
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
#include <cmath>

#include "openturns/RandomizedQuasiMonteCarlo.hxx"
#include "openturns/ComparisonOperatorImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/RandomGenerator.hxx"

BEGIN_NAMESPACE_OPENTURNS

/*
 * @class RandomizedQuasiMonteCarlo
 */
CLASSNAMEINIT(RandomizedQuasiMonteCarlo);

static const Factory<RandomizedQuasiMonteCarlo> Factory_RandomizedQuasiMonteCarlo;

/* Constructor with parameters */
RandomizedQuasiMonteCarlo::RandomizedQuasiMonteCarlo()
  :
  Simulation(),
  dimension_(0),
  lowDiscrepancySequence_()
{
  // Nothing to do
}

/* Constructor with parameters */
RandomizedQuasiMonteCarlo::RandomizedQuasiMonteCarlo(const Event & event,
    const LowDiscrepancySequence & lowDiscrepancySequence) :
  Simulation(event),
  dimension_(event.getImplementation()->getAntecedent()->getDimension()),
  lowDiscrepancySequence_(lowDiscrepancySequence)
{
  if (!event.getImplementation()->getAntecedent()->getDistribution().hasIndependentCopula()) throw InvalidArgumentException(HERE) << "Error: cannot use the RandomizedQuasiMonteCarlo algorithm with an input distribution whose components are not independent.";
  // retrieve the marginal laws
  for (UnsignedInteger index = 0; index < dimension_; ++index) marginals_.add(event.getImplementation()->getAntecedent()->getDistribution().getMarginal(index));

  // initialize the low-discrepancy sequence
  lowDiscrepancySequence_.initialize(dimension_);
}


/* Virtual constructor */
RandomizedQuasiMonteCarlo * RandomizedQuasiMonteCarlo::clone() const
{
  return new RandomizedQuasiMonteCarlo(*this);
}


/* Compute the block sample */
Sample RandomizedQuasiMonteCarlo::computeBlockSample()
{
  // Size of a block
  const UnsignedInteger blockSize = getBlockSize();

  // allocate the input sample
  Sample inputSample(lowDiscrepancySequence_.generate(blockSize));

  // for each point of the sample
  for(UnsignedInteger index = 0; index < blockSize; ++index)
  {
    // for each component
    for(UnsignedInteger component = 0; component < dimension_; ++component)
    {
      // use marginal laws to compute quantile from the low-discrepancy value to build the input sample
      // with a cyclic scrambling of the low discrepancy point as in R. Cranley and T.N.L. Patterson, "Randomization of number theoretic methods for multiple integration.", SIAM Journal of Numerical Analysis, 13:904-914, 1976.
      NumericalScalar tmp = -1.0;
      inputSample[index][component] = marginals_[component].computeQuantile(modf(inputSample[index][component] + RandomGenerator::Generate(), &tmp))[0];
    } // For component
  } // For index

  // Then, evaluate the function on this sample
  Sample blockSample(event_.getImplementation()->getFunction()(inputSample));
  for (UnsignedInteger i = 0; i < blockSize_; ++i) blockSample[i][0] = getEvent().getOperator()(blockSample[i][0], event_.getThreshold());
  return blockSample;
}


/* String converter */
String RandomizedQuasiMonteCarlo::__repr__() const
{
  OSS oss;
  oss << "class=" << RandomizedQuasiMonteCarlo::GetClassName()
      << " derived from " << Simulation::__repr__();
  return oss;
}

/* Method save() stores the object through the StorageManager */
void RandomizedQuasiMonteCarlo::save(Advocate & adv) const
{
  Simulation::save(adv);
  adv.saveAttribute("dimension_", dimension_);
  adv.saveAttribute("lowDiscrepancySequence_", lowDiscrepancySequence_);
  adv.saveAttribute("marginals_", marginals_);
}

/* Method load() reloads the object from the StorageManager */
void RandomizedQuasiMonteCarlo::load(Advocate & adv)
{
  Simulation::load(adv);
  adv.loadAttribute("dimension_", dimension_);
  adv.loadAttribute("lowDiscrepancySequence_", lowDiscrepancySequence_);
  adv.loadAttribute("marginals_", marginals_);
}


END_NAMESPACE_OPENTURNS
