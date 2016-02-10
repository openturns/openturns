//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for orthogonal basis
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "FunctionalBasis.hxx"
#include "OSS.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FunctionalBasis);

/* Default constructor */
FunctionalBasis::FunctionalBasis()
  : TypedInterfaceObject<FunctionalBasisImplementation>(new FunctionalBasisImplementation())
{
  // Nothing to do
}

/* Constructor from implementation */
FunctionalBasis::FunctionalBasis(const FunctionalBasisImplementation & implementation)
  : TypedInterfaceObject<FunctionalBasisImplementation>(implementation.clone())
{
  // Nothing to do
}

/* Build the NumericalMathFunction of the given index */
NumericalMathFunction FunctionalBasis::build(const UnsignedInteger index) const
{
  return getImplementation()->build(index);
}

NumericalMathFunction FunctionalBasis::operator[](const UnsignedInteger index) const
{
  return getImplementation()->operator[](index);
}

Bool FunctionalBasis::isOrthogonal() const
{
  return getImplementation()->isOrthogonal();
}


/* String converter */
String FunctionalBasis::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

END_NAMESPACE_OPENTURNS
