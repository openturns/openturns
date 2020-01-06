//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SamplingStrategy.hxx"
#include "openturns/RandomDirection.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class SamplingStrategy
 */

CLASSNAMEINIT(SamplingStrategy)

/* Defaultonstructor */
SamplingStrategy::SamplingStrategy()
  : TypedInterfaceObject<SamplingStrategyImplementation>(RandomDirection().clone())
{
  // Nothing to do
}

/* Constructor with parameters */
SamplingStrategy::SamplingStrategy(Implementation & p_implementation)
  : TypedInterfaceObject<SamplingStrategyImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor with parameters */
SamplingStrategy::SamplingStrategy(const SamplingStrategyImplementation & implementation)
  : TypedInterfaceObject<SamplingStrategyImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Constructor with parameters */
SamplingStrategy::SamplingStrategy(const UnsignedInteger dimension)
  : TypedInterfaceObject<SamplingStrategyImplementation>(new RandomDirection(dimension))
{
  // Nothing to do
}

/* Generate a set of directions */
Sample SamplingStrategy::generate() const
{
  return getImplementation()->generate();
}

/* Dimension accessor */
void SamplingStrategy::setDimension(const UnsignedInteger dimension)
{
  copyOnWrite();
  getImplementation()->setDimension(dimension);
}

UnsignedInteger SamplingStrategy::getDimension() const
{
  return getImplementation()->getDimension();
}

/* String converter */
String SamplingStrategy::__repr__() const
{
  OSS oss;
  oss << "class=" << SamplingStrategy::GetClassName()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS
