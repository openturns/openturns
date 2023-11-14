//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an combinatorialGenerator plane
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/CombinatorialGenerator.hxx"
#include "openturns/Tuples.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CombinatorialGenerator)

/* Default constructor */
CombinatorialGenerator::CombinatorialGenerator()
  : TypedInterfaceObject<CombinatorialGeneratorImplementation>(Tuples().clone())
{
  // Nothing to do
}

/* Constructor from implementation pointer */
CombinatorialGenerator::CombinatorialGenerator(const Implementation & p_implementation)
  : TypedInterfaceObject<CombinatorialGeneratorImplementation>(p_implementation)
{
  // Nothing to do
}

/* Constructor from implementation */
CombinatorialGenerator::CombinatorialGenerator(const CombinatorialGeneratorImplementation & implementation)
  : TypedInterfaceObject<CombinatorialGeneratorImplementation>(implementation.clone())
{
  // Nothing to do
}

/* String converter */
String CombinatorialGenerator::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName ()
      << " implementation=" << getImplementation()->__repr__();
  return oss;
}

/* Sample generation */
IndicesCollection CombinatorialGenerator::generate() const
{
  return getImplementation()->generate();
}

END_NAMESPACE_OPENTURNS
