//                                               -*- C++ -*-
/**
 *  @brief
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/RandomDirection.hxx"
#include "openturns/Log.hxx"
#include "openturns/PersistentObjectFactory.hxx"


BEGIN_NAMESPACE_OPENTURNS

/**
 * @class RandomDirection
 */

CLASSNAMEINIT(RandomDirection)

static const Factory<RandomDirection> Factory_RandomDirection;

/* Default constructor */
RandomDirection::RandomDirection()
  : SamplingStrategyImplementation(0)
{
  // Nothing to do
}

/* Constructor with parameters */
RandomDirection::RandomDirection(const UnsignedInteger dimension)
  : SamplingStrategyImplementation(dimension)
{
  // Nothing to do
}

/* Virtual constructor */
RandomDirection * RandomDirection::clone() const
{
  return new RandomDirection(*this);
}

/* Generate a set of directions */
Sample RandomDirection::generate() const
{
  Sample result(2, dimension_);
  result[0] = getUniformUnitVectorRealization();
  result[1] = -1.0 * result[0];
  LOGDEBUG(OSS() << "RandomDirection::generate: directions=\n" << result);
  return result;
}

/* String converter */
String RandomDirection::__repr__() const
{
  OSS oss;
  oss << "class=" << RandomDirection::GetClassName()
      << " derived from " << SamplingStrategyImplementation::__repr__();
  return oss;
}

END_NAMESPACE_OPENTURNS

