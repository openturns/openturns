//                                               -*- C++ -*-
/**
 *  @brief DomainIntersection represents the intersection of two Domain
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/DomainIntersection.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainIntersection)

static const Factory<DomainIntersection> Factory_DomainIntersection;


/* Default constructor */
DomainIntersection::DomainIntersection()
  : DomainImplementation()
{
  // Nothing to do
}

/* Default constructor */
DomainIntersection::DomainIntersection(const DomainCollection & collection)
  : DomainImplementation(collection.getSize() ? collection[0].getDimension() : 0)
  , collection_(collection)
{
  for (UnsignedInteger i = 1; i < collection.getSize(); ++ i)
    if (collection[i].getDimension() != getDimension())
      throw InvalidArgumentException(HERE) << "Error: cannot build a DomainIntersection from domains of different dimensions";
}

/* Default constructor */
DomainIntersection::DomainIntersection(const Domain & left, const Domain & right)
  : DomainIntersection(DomainCollection({left, right}))
{
  LOGWARN(OSS() << "DomainIntersection(Domain, Domain) is deprecated in favor of DomainIntersection(List[Domain])");
}

/* Clone method */
DomainIntersection * DomainIntersection::clone() const
{
  return new DomainIntersection(*this);
}

/* String converter */
String DomainIntersection::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " collection=" << collection_;

}

String DomainIntersection::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainIntersection::contains(const Point & point) const
{
  Bool result = true;
  for (UnsignedInteger i = 0; result && (i < collection_.getSize()); ++ i)
    result = result && collection_[i].contains(point);
  return result;
}

Bool DomainIntersection::operator == (const DomainIntersection & other) const
{
  if (this == &other) return true;
  return collection_ == other.collection_;
}

Bool DomainIntersection::operator != (const DomainIntersection & other) const
{
  return !operator==(other);
}

/* Method save() stores the object through the StorageManager */
void DomainIntersection::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
}

/* Method load() reloads the object from the StorageManager */
void DomainIntersection::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  if (adv.hasAttribute("collection_"))
    adv.loadAttribute("collection_", collection_);
  else
  {
    Domain left;
    Domain right;
    adv.loadAttribute("left_", left);
    adv.loadAttribute("right_", right);
    collection_ = DomainCollection({left, right});
  }
}

END_NAMESPACE_OPENTURNS
