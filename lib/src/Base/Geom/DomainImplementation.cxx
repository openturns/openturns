//                                               -*- C++ -*-
/**
 *  @brief DomainImplementation is defined as a domain of \mathbb{R}^d
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
#include "openturns/DomainImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainImplementation)

static const Factory<DomainImplementation> Factory_DomainImplementation;


/* Default constructor */
DomainImplementation::DomainImplementation(UnsignedInteger dimension)
  : PersistentObject()
  , dimension_(dimension)
{
  // Nothing to do
}

/* Clone method */
DomainImplementation * DomainImplementation::clone() const
{
  return new DomainImplementation(*this);
}

/* String converter */
String DomainImplementation::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " dimension=" << dimension_;

}

String DomainImplementation::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainImplementation::contains(const Point & ) const
{
  throw NotYetImplementedException(HERE) << "In DomainImplementation::contains(const Point & point) const";
}

/* Check if the given points are inside of the domain */
DomainImplementation::BoolCollection DomainImplementation::contains(const Sample & sample) const
{
  BoolCollection result(sample.getSize());
  for(UnsignedInteger i = 0; i < sample.getSize(); ++i)
    result[i] = contains(sample[i]);
  return result;
}

/* Get the dimension of the domain*/
UnsignedInteger DomainImplementation::getDimension() const
{
  return dimension_;
}

/* Method save() stores the object through the StorageManager */
void DomainImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("dimension_", dimension_);
}

/* Method load() reloads the object from the StorageManager */
void DomainImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("dimension_", dimension_);
}

END_NAMESPACE_OPENTURNS
