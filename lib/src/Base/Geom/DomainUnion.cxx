//                                               -*- C++ -*-
/**
 *  @brief DomainUnion represents the union of two Domain
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
#include "openturns/DomainUnion.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainUnion)

static const Factory<DomainUnion> Factory_DomainUnion;


/* Default constructor */
DomainUnion::DomainUnion()
  : DomainImplementation()
{
  // Nothing to do
}

/* Default constructor */
DomainUnion::DomainUnion(const DomainCollection & collection)
  : DomainImplementation(collection.getSize() ? collection[0].getDimension() : 0)
  , collection_(collection)
{
  for (UnsignedInteger i = 1; i < collection.getSize(); ++ i)
    if (collection[i].getDimension() != getDimension())
      throw InvalidArgumentException(HERE) << "Error: cannot build a DomainUnion from domains of different dimensions";
}

/* Default constructor */
DomainUnion::DomainUnion(const Domain & left, const Domain & right)
  : DomainUnion(DomainCollection({left, right}))
{
  LOGWARN(OSS() << "DomainUnion(Domain, Domain) is deprecated in favor of DomainUnion(List[Domain])");
}

/* Clone method */
DomainUnion * DomainUnion::clone() const
{
  return new DomainUnion(*this);
}

/* String converter */
String DomainUnion::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " collection=" << collection_;

}

String DomainUnion::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainUnion::contains(const Point & point) const
{
  Bool result = false;
  for (UnsignedInteger i = 0; !result && (i < collection_.getSize()); ++ i)
    result = result || collection_[i].contains(point);
  return result;
}

/* Compute the Euclidean distance from a given point to the domain */
Scalar DomainUnion::computeDistance(const Point & point) const
{
  Scalar distance = SpecFunc::MaxScalar;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++ i)
    distance = std::min(distance, collection_[i].computeDistance(point));
  return distance;
}

Bool DomainUnion::operator == (const DomainUnion & other) const
{
  if (this == &other) return true;
  return collection_ == other.collection_;
}

Bool DomainUnion::operator != (const DomainUnion & other) const
{
  return !operator==(other);
}

/* Method save() stores the object through the StorageManager */
void DomainUnion::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
}

/* Method load() reloads the object from the StorageManager */
void DomainUnion::load(Advocate & adv)
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
