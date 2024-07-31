//                                               -*- C++ -*-
/**
 *  @brief Regression from a data sample upon a particular basis
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

#include "openturns/SequentialSamplingAlgorithm.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(SequentialSamplingAlgorithm)

/* Default constructor */
SequentialSamplingAlgorithm::SequentialSamplingAlgorithm()
  : TypedInterfaceObject<SequentialSamplingAlgorithmImplementation>(new SequentialSamplingAlgorithmImplementation)
{
  // Nothing to do
}

/* Constructor from implementation */
SequentialSamplingAlgorithm::SequentialSamplingAlgorithm(const SequentialSamplingAlgorithmImplementation & implementation)
  : TypedInterfaceObject<SequentialSamplingAlgorithmImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* Constructor from implementation */
SequentialSamplingAlgorithm::SequentialSamplingAlgorithm(const Implementation & p_implementation)
  : TypedInterfaceObject<SequentialSamplingAlgorithmImplementation>( p_implementation )
{
  // Nothing to do
}

/* Accessors */
Sample SequentialSamplingAlgorithm::getX() const
{
  return getImplementation()->getX();
}

Sample SequentialSamplingAlgorithm::getY() const
{
  return getImplementation()->getY();
}

/* Generation indices accessor */
Indices SequentialSamplingAlgorithm::getGenerationIndices() const
{
  return getImplementation()->getGenerationIndices();
}

/* String converter */
String SequentialSamplingAlgorithm::__repr__() const
{
  return getImplementation()->__repr__();
}

String SequentialSamplingAlgorithm::__str__(const String & offset) const
{
  return getImplementation()->__str__( offset );
}

/* Generate new samples */
Sample SequentialSamplingAlgorithm::generate(const UnsignedInteger size) const
{
  return getImplementation()->generate(size);
}

/* Update the state */
void SequentialSamplingAlgorithm::update(const Sample & x, const Sample & y)
{
  copyOnWrite();
  getImplementation()->update(x, y);
}

END_NAMESPACE_OPENTURNS
