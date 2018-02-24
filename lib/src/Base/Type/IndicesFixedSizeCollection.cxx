//                                               -*- C++ -*-
/**
 *  @brief IndicesFixedSizeCollection stores a Collection of Indices contiguously.
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
#include "openturns/IndicesFixedSizeCollection.hxx"
#include "openturns/IndicesFixedSizeCollectionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicesFixedSizeCollection)

/* Default constructor */
IndicesFixedSizeCollection::IndicesFixedSizeCollection()
  : TypedInterfaceObject<IndicesFixedSizeCollectionImplementation>(new IndicesFixedSizeCollectionImplementation())
{
  // Nothing to do
}

/* Constructor from size and stride */
IndicesFixedSizeCollection::IndicesFixedSizeCollection(const UnsignedInteger size, const UnsignedInteger stride)
  : TypedInterfaceObject<IndicesFixedSizeCollectionImplementation>(new IndicesFixedSizeCollectionImplementation(size, stride))
{
  // Nothing to do
}

/* Constructor from size, stride and values */
IndicesFixedSizeCollection::IndicesFixedSizeCollection(const UnsignedInteger size,
   const UnsignedInteger stride, const Indices & values)
  : TypedInterfaceObject<IndicesFixedSizeCollectionImplementation>(new IndicesFixedSizeCollectionImplementation(size, stride, values))
{
  // Nothing to do
}

/* Constructor from external collection */
IndicesFixedSizeCollection::IndicesFixedSizeCollection(const Collection<Indices> & values)
  : TypedInterfaceObject<IndicesFixedSizeCollectionImplementation>(new IndicesFixedSizeCollectionImplementation(values))
{
  // Nothing to do
}

/* Constructor with implementation */
IndicesFixedSizeCollection::IndicesFixedSizeCollection(const Implementation & i)
  : TypedInterfaceObject<IndicesFixedSizeCollectionImplementation>(i)
{
  // Nothing to do
}

/* Constructor with implementation */
IndicesFixedSizeCollection::IndicesFixedSizeCollection(const IndicesFixedSizeCollectionImplementation & i)
  : TypedInterfaceObject<IndicesFixedSizeCollectionImplementation>(i.clone())
{
  // Nothing to do
}

/* String converter */
String IndicesFixedSizeCollection::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}

/* Get the number of Indices */
UnsignedInteger IndicesFixedSizeCollection::getSize() const
{
  return getImplementation()->getSize();
}


String IndicesFixedSizeCollection::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

IndicesFixedSizeCollection::iterator IndicesFixedSizeCollection::begin_at(const UnsignedInteger index)
{
  // Arguments are checked in implementation
  return getImplementation()->begin_at(index);
}

IndicesFixedSizeCollection::iterator IndicesFixedSizeCollection::end_at(const UnsignedInteger index)
{
  return getImplementation()->end_at(index);
}

IndicesFixedSizeCollection::const_iterator IndicesFixedSizeCollection::cbegin_at(const UnsignedInteger index) const
{
  return getImplementation()->cbegin_at(index);
}

IndicesFixedSizeCollection::const_iterator IndicesFixedSizeCollection::cend_at(const UnsignedInteger index) const
{
  return getImplementation()->cend_at(index);
}

// Accessors to values_[index][pos]
const UnsignedInteger & IndicesFixedSizeCollection::operator()(const UnsignedInteger index, const UnsignedInteger pos) const
{
  return getImplementation()->operator()(index, pos);
}

UnsignedInteger & IndicesFixedSizeCollection::operator()(const UnsignedInteger index, const UnsignedInteger pos)
{
  return getImplementation()->operator()(index, pos);
}

END_NAMESPACE_OPENTURNS
