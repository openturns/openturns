//                                               -*- C++ -*-
/**
 *  @brief DomainUnion represents the union of two Domain
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
#include "openturns/DomainUnion.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
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
DomainUnion::DomainUnion(const Domain & left, const Domain & right)
  : DomainImplementation(left.getDimension())
  , left_(left)
  , right_(right)
{
  if (right.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot build a DomainUnion from two Domain of different dimensions";
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
         << " left=" << left_
         << " right=" << right_;

}

String DomainUnion::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainUnion::contains(const Point & point) const
{
  return left_.contains(point) || right_.contains(point);
}

/* Check if the given points are inside of the domain */
DomainUnion::BoolCollection DomainUnion::contains(const Sample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  // We do not know whether contains() is costly or not, thus we prefer to
  // minimize calls to right_.contains()
  BoolCollection leftResult(left_.contains(sample));
  Indices notInLeft(0);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    if (!leftResult[i])
      notInLeft.add(i);
  }
  const BoolCollection rightResult(right_.contains(sample.select(notInLeft)));
  BoolCollection result(size, 1);
  for(UnsignedInteger i = 0; i < rightResult.getSize(); ++i)
    result[notInLeft[i]] = rightResult[i];
  return result;
}

Bool DomainUnion::operator == (const DomainUnion & other) const
{
  if (this == &other) return true;
  return left_ == other.left_ && right_ == other.right_;
}

Bool DomainUnion::operator != (const DomainUnion & other) const
{
  return !operator==(other);
}

/* Method save() stores the object through the StorageManager */
void DomainUnion::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("left_", left_);
  adv.saveAttribute("right_", right_);
}

/* Method load() reloads the object from the StorageManager */
void DomainUnion::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  adv.loadAttribute("left_", left_);
  adv.loadAttribute("right_", right_);
}

END_NAMESPACE_OPENTURNS
