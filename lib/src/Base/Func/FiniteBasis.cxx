//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for basis
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
#include "openturns/FiniteBasis.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(FiniteBasis)

static const Factory<FiniteBasis> Factory_FiniteBasis;

FiniteBasis::FiniteBasis(const UnsignedInteger size)
  : BasisImplementation()
  , collection_(size)
{

}


FiniteBasis::FiniteBasis(const FunctionCollection & collection)
  : BasisImplementation()
  , collection_(collection)
{
  // Nothing to do
}


/* Virtual constructor */
FiniteBasis * FiniteBasis::clone() const
{
  return new FiniteBasis(*this);
}

/* Build the Function of the given index */
Function FiniteBasis::build(const UnsignedInteger index) const
{
  if (index >= collection_.getSize()) throw InvalidArgumentException(HERE) << "Cannot build " << index << "-th term";
  return collection_[index];
}

Function & FiniteBasis::operator[](const UnsignedInteger index)
{
  return collection_[index];
}

String FiniteBasis::__repr__() const
{
  return OSS() << "class=" << "Basis"
         " coll=" << collection_;
}

String FiniteBasis::__str__(const String & offset) const
{
  return OSS() << "Basis( " << collection_.__str__(offset) << " )";
}

UnsignedInteger FiniteBasis::getDimension() const
{
  return (collection_.getSize() > 0) ? collection_[0].getInputDimension() : 0;
}

UnsignedInteger FiniteBasis::getSize() const
{
  return collection_.getSize();
}

void FiniteBasis::add(const Function & elt)
{
  collection_.add(elt);
}

Bool FiniteBasis::isFinite() const
{
  return true;
}

void FiniteBasis::save(Advocate & adv) const
{
  BasisImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
}

/* Method load() reloads the object from the StorageManager */
void FiniteBasis::load(Advocate & adv)
{
  BasisImplementation::load(adv);
  adv.loadAttribute("collection_", collection_);
}

END_NAMESPACE_OPENTURNS
