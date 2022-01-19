//                                               -*- C++ -*-
/**
 *  @brief IterativeAlgorithm is the base class to implement iterative statistics.
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
#include "openturns/IterativeAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IterativeAlgorithm)

/**
 * Default constructor
 *
 * The constructor sets a new Id to the object,
 * so it can be later referenced by a Study object.
 * It is also declared visible if member of a study.
 *
 * The object has the default name but it does not
 * use storage for it.
 */
IterativeAlgorithm::IterativeAlgorithm()
  : TypedInterfaceObject<IterativeAlgorithmImplementation>(new IterativeAlgorithmImplementation())
{
  // Nothing to do
}

/** Constructor */
IterativeAlgorithm::IterativeAlgorithm(const UnsignedInteger dimension)
  : TypedInterfaceObject<IterativeAlgorithmImplementation>(new IterativeAlgorithmImplementation(dimension))
{
  // Nothing to do
}

/** Constructor from implementation */
IterativeAlgorithm::IterativeAlgorithm(const IterativeAlgorithmImplementation & implementation)
  : TypedInterfaceObject<IterativeAlgorithmImplementation>(implementation.clone())
{
  // Nothing to do
}

/** Constructor from implementation */
IterativeAlgorithm::IterativeAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<IterativeAlgorithmImplementation>(p_implementation)
{
  // Nothing to do
}

/**
 * Increment methods
 *
 * @warning These methods MUST be overloaded in derived classes.
 */

void IterativeAlgorithm::increment(const Point & newData)
{
  copyOnWrite();
   copyOnWrite();
   return getImplementation()->increment(newData);
}

void IterativeAlgorithm::increment(const Sample & newData)
{
  copyOnWrite();
   copyOnWrite();
   return getImplementation()->increment(newData);
}

/**
 * Iteration accessor
 *
 * This method returns the current iteration number of the algorithm.
 */
UnsignedInteger IterativeAlgorithm::getIteration() const
{
  return getImplementation()->getIteration();
}

/**
 * Dimension accessor
 *
 * This method returns the dimension of the object.
 */
UnsignedInteger IterativeAlgorithm::getDimension() const
{
  return getImplementation()->getDimension();
}

/** String converter */
String IterativeAlgorithm::__repr__() const
{
  return getImplementation()->__repr__();
}

/** String converter */
String IterativeAlgorithm::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

END_NAMESPACE_OPENTURNS
