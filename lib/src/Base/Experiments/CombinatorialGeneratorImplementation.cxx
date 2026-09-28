//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level view of an combinatorialGenerator plane
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OTprivate.hxx"
#include "openturns/CombinatorialGeneratorImplementation.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(CombinatorialGeneratorImplementation)

/* Default constructor */
CombinatorialGeneratorImplementation::CombinatorialGeneratorImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
CombinatorialGeneratorImplementation * CombinatorialGeneratorImplementation::clone() const
{
  return new CombinatorialGeneratorImplementation(*this);
}

/* String converter */
String CombinatorialGeneratorImplementation::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName();
  return oss;
}

/* Sample generation, loop over generateNext() without altering the current state */
IndicesCollection CombinatorialGeneratorImplementation::generate() const
{
  const Implementation p_generator(clone());
  p_generator->restart();
  const UnsignedInteger size = p_generator->getSize();
  Collection<Indices> coll(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    coll[i] = p_generator->generateNext();
  return IndicesCollection(coll);
}

/* Next combination generation */
Indices CombinatorialGeneratorImplementation::generateNext() const
{
  throw NotYetImplementedException(HERE) << "In CombinatorialGeneratorImplementation::generateNext()";
}

/* Number of combinations accessor */
UnsignedInteger CombinatorialGeneratorImplementation::getSize() const
{
  throw NotYetImplementedException(HERE) << "In CombinatorialGeneratorImplementation::getSize()";
}

/* Dimension of the generated combinations accessor */
UnsignedInteger CombinatorialGeneratorImplementation::getDimension() const
{
  throw NotYetImplementedException(HERE) << "In CombinatorialGeneratorImplementation::getDimension()";
}

/* Restart the combinatorial sequence */
void CombinatorialGeneratorImplementation::restart() const
{
  // Nothing to do: stateless base implementation
}

END_NAMESPACE_OPENTURNS
