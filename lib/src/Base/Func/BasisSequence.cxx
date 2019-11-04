//                                               -*- C++ -*-
/**
 *  @brief A sequence of numerical math function collection
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

#include "openturns/BasisSequence.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(BasisSequence)

/* Default constructor */
BasisSequence::BasisSequence()
  : TypedInterfaceObject<BasisSequenceImplementation>( new BasisSequenceImplementation )
{
  // Nothing to do
}

/* Default constructor */
BasisSequence::BasisSequence(const Basis & psi)
  : TypedInterfaceObject<BasisSequenceImplementation>( new BasisSequenceImplementation(psi) )
{
  // Nothing to do
}

/* Constructor from implementation */
BasisSequence::BasisSequence(const BasisSequenceImplementation & implementation)
  : TypedInterfaceObject<BasisSequenceImplementation>( implementation.clone() )
{
  // Nothing to do
}

/* Dimension accessor */
UnsignedInteger BasisSequence::getDimension() const
{
  return getImplementation()->getDimension();
}

/* Returns the master basis */
Basis BasisSequence::getMasterBasis() const
{
  return getImplementation()->getMasterBasis();
}

/* Sub-basis accessor */
Basis BasisSequence::getBasis(const UnsignedInteger index) const
{
  return getImplementation()->getBasis( index );
}

/* Indices accessor */
Indices BasisSequence::getIndices(const UnsignedInteger index) const
{
  return getImplementation()->getIndices( index );
}

/* Size accessor */
UnsignedInteger BasisSequence::getSize() const
{
  return getImplementation()->getSize();
}

/** Extend basis */
void BasisSequence::add(const Indices & indices)
{
  getImplementation()->add( indices );
}

/* Converter to Basis collection */
BasisSequence::operator IndicesCollection () const
{
  return getImplementation()->getIndicesCollection();
}

/* String converter */
String BasisSequence::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " implementation=" << getImplementation()->__repr__();
}

String BasisSequence::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}



END_NAMESPACE_OPENTURNS
