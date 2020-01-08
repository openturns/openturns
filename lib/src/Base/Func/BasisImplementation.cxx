//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for basis
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
#include "openturns/BasisImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(BasisImplementation)

static const Factory<BasisImplementation> Factory_BasisImplementation;


/* Default constructor */
BasisImplementation::BasisImplementation()
  : PersistentObject()
{
  // Nothing to do
}

/* Virtual constructor */
BasisImplementation * BasisImplementation::clone() const
{
  return new BasisImplementation(*this);
}

/* Build the Function of the given index */
Function BasisImplementation::build(const UnsignedInteger ) const
{
  throw NotYetImplementedException(HERE) << "In Function BasisImplementation::build(const UnsignedInteger index) const";
}

/* Build the Function of the given index */
Function BasisImplementation::operator[](const UnsignedInteger index) const
{
  return build(index);
}

Function & BasisImplementation::operator[](const UnsignedInteger )
{
  throw NotYetImplementedException(HERE) << "In BasisImplementation::operator[](const UnsignedInteger index)";
}

BasisImplementation::FunctionCollection BasisImplementation::getSubBasis(const Indices& indices) const
{
  const UnsignedInteger size = indices.getSize();
  FunctionCollection coll(size);
  for (UnsignedInteger i = 0; i < size; ++ i)
  {
    coll[i] = operator[](indices[i]);
  }
  return coll;
}

void BasisImplementation::add(const Function & )
{
  throw NotYetImplementedException(HERE) << "In BasisImplementation::add(const Function & elt)";
}

UnsignedInteger BasisImplementation::getDimension() const
{
  throw NotYetImplementedException(HERE) << "In BasisImplementation::getDimension() const";
}

UnsignedInteger BasisImplementation::getSize() const
{
  throw NotYetImplementedException(HERE) << "In BasisImplementation::getSize() const";
}

Bool BasisImplementation::isOrthogonal() const
{
  return false;
}

Bool BasisImplementation::isFinite() const
{
  return false;
}

String BasisImplementation::__repr__() const
{
  return OSS() << "class=" << getClassName();
}

// String BasisImplementation::__str__(const String & offset) const
// {
//   return OSS() << "class=" << getClassName();
// }

void BasisImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
}

/* Method load() reloads the object from the StorageManager */
void BasisImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
}

END_NAMESPACE_OPENTURNS
