//                                               -*- C++ -*-
/**
 *  @brief DomainDisjunctiveUnion represents the disjunctive union of two Domain
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
#include "openturns/DomainDisjunctiveUnion.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainDisjunctiveUnion)

static const Factory<DomainDisjunctiveUnion> Factory_DomainDisjunctiveUnion;


/* Default constructor */
DomainDisjunctiveUnion::DomainDisjunctiveUnion()
  : DomainImplementation()
{
  // Nothing to do
}

/* Default constructor */
DomainDisjunctiveUnion::DomainDisjunctiveUnion(const DomainCollection & collection)
  : DomainImplementation(collection.getSize() ? collection[0].getDimension() : 0)
  , collection_(collection)
{
  for (UnsignedInteger i = 1; i < collection.getSize(); ++ i)
    if (collection[i].getDimension() != getDimension())
      throw InvalidArgumentException(HERE) << "Error: cannot build a DomainDisjunctiveUnion from domains of different dimensions";
}

/* Clone method */
DomainDisjunctiveUnion * DomainDisjunctiveUnion::clone() const
{
  return new DomainDisjunctiveUnion(*this);
}

/* String converter */
String DomainDisjunctiveUnion::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " collection=" << collection_;
}

String DomainDisjunctiveUnion::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainDisjunctiveUnion::contains(const Point & point) const
{
  UnsignedInteger count = 0;
  for (UnsignedInteger i = 0; (count < 2) && (i < collection_.getSize()); ++ i)
    if (collection_[i].contains(point))
      ++ count;
  return count == 1;
}

/* Compute the Euclidean distance from a given point to the domain */
Scalar DomainDisjunctiveUnion::computeDistance(const Point & point) const
{
  Scalar best = SpecFunc::MaxScalar;
  UnsignedInteger count = 0;
  for (UnsignedInteger i = 0; i < collection_.getSize(); ++ i)
  {
    const Scalar distance = collection_[i].computeDistance(point);
    if (distance < best)
    {
      count = 1;
      best = distance;
    }
    else if (distance == best)
      ++ count;
  }
  if (count > 1)
    throw NotDefinedException(HERE) << "distance reached for at least 2 subdomains";
  return best;
}

Bool DomainDisjunctiveUnion::operator == (const DomainDisjunctiveUnion & other) const
{
  if (this == &other) return true;
  return collection_ == other.collection_;
}

Bool DomainDisjunctiveUnion::operator != (const DomainDisjunctiveUnion & other) const
{
  return !operator==(other);
}

/* Method save() stores the object through the StorageManager */
void DomainDisjunctiveUnion::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("collection_", collection_);
}

/* Method load() reloads the object from the StorageManager */
void DomainDisjunctiveUnion::load(Advocate & adv)
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
