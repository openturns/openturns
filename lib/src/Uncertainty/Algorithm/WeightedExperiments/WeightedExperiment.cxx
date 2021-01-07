//                                               -*- C++ -*-
/*
 *  @brief WeightedExperiment interface
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/WeightedExperiment.hxx"
#include "openturns/MonteCarloExperiment.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(WeightedExperiment)

/* Default constructor */
WeightedExperiment::WeightedExperiment() :
  TypedInterfaceObject<WeightedExperimentImplementation>(new MonteCarloExperiment())
{
  // Nothing to do
}

/* Constructor from an implementation */
WeightedExperiment::WeightedExperiment(const WeightedExperimentImplementation & implementation) :
  TypedInterfaceObject<WeightedExperimentImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor from a Pointer to an implementation */
WeightedExperiment::WeightedExperiment(const Implementation & p_implementation) :
  TypedInterfaceObject<WeightedExperimentImplementation>(p_implementation)
{
  // Nothing to do
}

/* String converter */
String WeightedExperiment::__repr__() const
{
  return getImplementation()->__repr__();
}

/* Distribution accessor */
void WeightedExperiment::setDistribution(const Distribution & distribution)
{
  copyOnWrite();
  getImplementation()->setDistribution(distribution);
}

Distribution WeightedExperiment::getDistribution() const
{
  return getImplementation()->getDistribution();
}

/* Size accessor */
void WeightedExperiment::setSize(const UnsignedInteger size)
{
  copyOnWrite();
  getImplementation()->setSize(size);
}

UnsignedInteger WeightedExperiment::getSize() const
{
  return getImplementation()->getSize();
}
/* Here is the interface that all derived class must implement */

/* Sample generation */
Sample WeightedExperiment::generate() const
{
  return getImplementation()->generate();
}

/* Sample generation with weights*/
Sample WeightedExperiment::generateWithWeights(Point & weights) const
{
  return getImplementation()->generateWithWeights(weights);
}

Bool WeightedExperiment::hasUniformWeights() const
{
  return getImplementation()->hasUniformWeights();
}

END_NAMESPACE_OPENTURNS
