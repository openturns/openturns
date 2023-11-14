//                                               -*- C++ -*-
/**
 *  @brief A collection of numerical math function
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

#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Basis.hxx"
#include "openturns/FiniteBasis.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Basis)

/* Default constructor */
Basis::Basis()
  : TypedInterfaceObject<BasisImplementation>(new FiniteBasis(0))
{
  // Nothing to do
}

/* Constructor from a collection */
Basis::Basis(const Collection<Function> & coll)
  : TypedInterfaceObject<BasisImplementation>(new FiniteBasis(coll))
{
  // Nothing to do
}

/* Constructor from a collection */
Basis::Basis(const UnsignedInteger size)
  : TypedInterfaceObject<BasisImplementation>(new FiniteBasis(size))
{
  // Nothing to do
}

/* Constructor from implementation */
Basis::Basis(const BasisImplementation & implementation)
  : TypedInterfaceObject<BasisImplementation>(implementation.clone())
{
  // Nothing to do
}


Basis::operator FunctionCollection() const
{
  FunctionCollection coll;
  for (UnsignedInteger i = 0; i < getSize(); ++ i)
  {
    coll.add(build(i));
  }
  return coll;
}


/* Constructor from implementation */
// Basis::Basis(BasisImplementation * p_implementation)
//   : TypedInterfaceObject<BasisImplementation>(p_implementation->clone())
// {
//   // Nothing to do
// }

/* String converter */
String Basis::__repr__() const
{
//   return OSS(true) << "class=" << GetClassName();
  return getImplementation()->__repr__();
}

String Basis::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

Function Basis::build(const UnsignedInteger index) const
{
  return getImplementation()->build(index);
}


Function Basis::operator[](const UnsignedInteger index) const
{
  return getImplementation()->operator[](index);
}

Function & Basis::operator[](const UnsignedInteger index)
{
  copyOnWrite();
  return getImplementation()->operator[](index);
}

Basis::FunctionCollection Basis::getSubBasis(const Indices & indices) const
{
  return getImplementation()->getSubBasis(indices);
}

void Basis::add(const Function & elt)
{
  copyOnWrite();
  getImplementation()->add(elt);
}

/* Dimension accessor */
UnsignedInteger Basis::getInputDimension() const
{
  return getImplementation()->getInputDimension();
}

UnsignedInteger Basis::getOutputDimension() const
{
  return getImplementation()->getOutputDimension();
}

UnsignedInteger Basis::getSize() const
{
  return getImplementation()->getSize();
}


Bool Basis::isOrthogonal() const
{
  return getImplementation()->isOrthogonal();
}

Bool Basis::isFinite() const
{
  return getImplementation()->isFinite();
}

END_NAMESPACE_OPENTURNS

