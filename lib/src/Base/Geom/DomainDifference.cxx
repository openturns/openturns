//                                               -*- C++ -*-
/**
 *  @brief DomainDifference represents the difference of two Domain
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
#include "openturns/DomainDifference.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DomainDifference)

static const Factory<DomainDifference> Factory_DomainDifference;


/* Default constructor */
DomainDifference::DomainDifference()
  : DomainImplementation()
{
  // Nothing to do
}

/* Default constructor */
DomainDifference::DomainDifference(const Domain & left, const Domain & right)
  : DomainImplementation(left.getDimension())
  , left_(left)
  , right_(right)
{
  if (right.getDimension() != getDimension()) throw InvalidArgumentException(HERE) << "Error: cannot build a DomainDifference from two Domain of different dimensions";
}

/* Clone method */
DomainDifference * DomainDifference::clone() const
{
  return new DomainDifference(*this);
}

/* String converter */
String DomainDifference::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " left=" << left_
         << " right=" << right_;

}

String DomainDifference::__str__(const String & ) const
{
  return OSS(false) << __repr__();
}

/* Check if the given point is inside of the domain */
Bool DomainDifference::contains(const Point & point) const
{
  return left_.contains(point) && (!right_.contains(point));
}

/* Check if the given points are inside of the domain */
DomainDifference::BoolCollection DomainDifference::contains(const Sample & sample) const
{
  const UnsignedInteger size(sample.getSize());
  // We do not know whether contains() is costly or not, thus we prefer to
  // minimize calls to right_.contains()
  BoolCollection leftResult(left_.contains(sample));
  Indices inLeft(0);
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    if (leftResult[i])
      inLeft.add(i);
  }
  const BoolCollection rightResult(right_.contains(sample.select(inLeft)));
  BoolCollection result(size);
  for(UnsignedInteger i = 0; i < rightResult.getSize(); ++i)
    result[inLeft[i]] = (rightResult[i] == 0 ? 1 : 0);
  return result;
}

Bool DomainDifference::operator == (const DomainDifference & other) const
{
  if (this == &other) return true;
  return left_ == other.left_ && right_ == other.right_;
}

Bool DomainDifference::operator != (const DomainDifference & other) const
{
  return !operator==(other);
}

/* Method save() stores the object through the StorageManager */
void DomainDifference::save(Advocate & adv) const
{
  DomainImplementation::save(adv);
  adv.saveAttribute("left_", left_);
  adv.saveAttribute("right_", right_);
}

/* Method load() reloads the object from the StorageManager */
void DomainDifference::load(Advocate & adv)
{
  DomainImplementation::load(adv);
  adv.loadAttribute("left_", left_);
  adv.loadAttribute("right_", right_);
}

END_NAMESPACE_OPENTURNS
