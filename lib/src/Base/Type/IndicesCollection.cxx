//                                               -*- C++ -*-
/**
 *  @brief IndicesCollection stores a Collection of Indices contiguously.
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
#include "openturns/IndicesCollection.hxx"
#include "openturns/IndicesCollectionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicesCollection)

/* Default constructor */
IndicesCollection::IndicesCollection()
  : TypedInterfaceObject<IndicesCollectionImplementation>(new IndicesCollectionImplementation())
{
  // Nothing to do
}

/* Constructor from size and stride */
IndicesCollection::IndicesCollection(const UnsignedInteger size, const UnsignedInteger stride)
  : TypedInterfaceObject<IndicesCollectionImplementation>(new IndicesCollectionImplementation(size, stride))
{
  // Nothing to do
}

/* Constructor from size, stride and values */
IndicesCollection::IndicesCollection(const UnsignedInteger size,
                                     const UnsignedInteger stride, const Indices & values)
  : TypedInterfaceObject<IndicesCollectionImplementation>(new IndicesCollectionImplementation(size, stride, values))
{
  // Nothing to do
}

/* Constructor from external collection */
IndicesCollection::IndicesCollection(const Collection<Indices> & values)
  : TypedInterfaceObject<IndicesCollectionImplementation>(new IndicesCollectionImplementation(values))
{
  // Nothing to do
}

/* Constructor with implementation */
IndicesCollection::IndicesCollection(const Implementation & i)
  : TypedInterfaceObject<IndicesCollectionImplementation>(i)
{
  // Nothing to do
}

/* Constructor with implementation */
IndicesCollection::IndicesCollection(const IndicesCollectionImplementation & i)
  : TypedInterfaceObject<IndicesCollectionImplementation>(i.clone())
{
  // Nothing to do
}

/* Get the number of Indices */
UnsignedInteger IndicesCollection::getSize() const
{
  return getImplementation()->getSize();
}


/* String converter */
String IndicesCollection::__repr__() const
{
  return getImplementation()->__repr__();
}

String IndicesCollection::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

IndicesCollection::iterator IndicesCollection::begin_at(const UnsignedInteger index)
{
  // Arguments are checked in implementation
  return getImplementation()->begin_at(index);
}

IndicesCollection::iterator IndicesCollection::end_at(const UnsignedInteger index)
{
  return getImplementation()->end_at(index);
}

IndicesCollection::const_iterator IndicesCollection::cbegin_at(const UnsignedInteger index) const
{
  return getImplementation()->cbegin_at(index);
}

IndicesCollection::const_iterator IndicesCollection::cend_at(const UnsignedInteger index) const
{
  return getImplementation()->cend_at(index);
}

// Accessors to values_[index][pos]
const UnsignedInteger & IndicesCollection::operator()(const UnsignedInteger index, const UnsignedInteger pos) const
{
  return getImplementation()->operator()(index, pos);
}

UnsignedInteger & IndicesCollection::operator()(const UnsignedInteger index, const UnsignedInteger pos)
{
  return getImplementation()->operator()(index, pos);
}

END_NAMESPACE_OPENTURNS
