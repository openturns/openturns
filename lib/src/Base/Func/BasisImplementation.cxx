//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for basis
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/BasisImplementation.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS


CLASSNAMEINIT(BasisImplementation)

static const Factory<BasisImplementation> Factory_BasisImplementation;


/* Size constructor */
BasisImplementation::BasisImplementation()
  : PersistentObject()
{
  // Nothing to do
}


/* Size constructor */
BasisImplementation::BasisImplementation(const UnsignedInteger size)
  : PersistentObject()
  , collection_(size)
{
  // Nothing to do
}

/* Collection constructor */
BasisImplementation::BasisImplementation(const FunctionCollection & collection)
  : PersistentObject()
  , collection_(collection)
{
  // Nothing to do
}

/* Virtual constructor */
BasisImplementation * BasisImplementation::clone() const
{
  return new BasisImplementation(*this);
}

/* Build the function of the given index */
Function BasisImplementation::build(const UnsignedInteger index) const
{
  if (index < getCurrentSize())
    return collection_[index];
  throw NotYetImplementedException(HERE) << "In Function BasisImplementation::build(const UnsignedInteger index) const";
}

/* Get the function of the given index */
Function BasisImplementation::operator[](const UnsignedInteger index) const
{
  const UnsignedInteger currentSize = getCurrentSize();

  if (index < currentSize)
    return collection_[index];
  if (!isFinite() && (index == getCurrentSize()))
  {
    collection_.add(build(index));
    return collection_[index];
  }
  throw OutOfBoundException(HERE) << "index=" << index << " size=" << currentSize;
}

Function & BasisImplementation::operator[](const UnsignedInteger index)
{
  const UnsignedInteger currentSize = getCurrentSize();
  if (index < currentSize)
    return collection_[index];
  throw OutOfBoundException(HERE) << "index=" << index << " size=" << currentSize;
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

void BasisImplementation::add(const Function & elt) const
{
  collection_.add(elt);
}

UnsignedInteger BasisImplementation::getDimension() const
{
  if (isFinite())
    return (collection_.getSize() > 0) ? collection_[0].getInputDimension() : 0;

  throw NotYetImplementedException(HERE) << "In BasisImplementation::getDimension() const";
}

UnsignedInteger BasisImplementation::getSize() const
{
  if (isFinite())
    return getCurrentSize();

  throw NotYetImplementedException(HERE) << "In BasisImplementation::getSize() const";
}

UnsignedInteger BasisImplementation::getCurrentSize() const
{
  return collection_.getSize();
}

Bool BasisImplementation::isOrthogonal() const
{
  return false;
}

Bool BasisImplementation::isFinite() const
{
  return true;
}

String BasisImplementation::__repr__() const
{
  if (isFinite())
    return OSS() << "class=Basis coll=" << collection_;
  return OSS() << "class=Basis";
}

String BasisImplementation::__str__(const String & offset) const
{
  if (isFinite())
    return OSS() << "Basis( " << collection_.__str__(offset) << " )";
  return __repr__();
}

void BasisImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("collection_", collection_);
}

/* Method load() reloads the object from the StorageManager */
void BasisImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("collection_", collection_);
}

END_NAMESPACE_OPENTURNS
