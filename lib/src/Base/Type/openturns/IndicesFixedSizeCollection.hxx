//                                               -*- C++ -*-
/**
 *  @brief IndicesFixedSizeCollectionImplementation stores a Collection of Indices contiguously.
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
#ifndef OPENTURNS_INDICESFROZENCOLLECTION_HXX
#define OPENTURNS_INDICESFROZENCOLLECTION_HXX

#include "openturns/IndicesFixedSizeCollectionImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class IndicesFixedSizeCollection
 *
 * IndicesFixedSizeCollection stores a Collection of Indices contiguously.
 */

class OT_API IndicesFixedSizeCollection :
  public TypedInterfaceObject<IndicesFixedSizeCollectionImplementation>
{
  CLASSNAME

public:

  typedef IndicesFixedSizeCollectionImplementation::iterator        iterator;
  typedef IndicesFixedSizeCollectionImplementation::const_iterator  const_iterator;

  /** Default constructor */
  IndicesFixedSizeCollection();

  /** Constructor from size and stride */
  IndicesFixedSizeCollection(const UnsignedInteger size, const UnsignedInteger stride);

  /** Constructor from size, stride and values */
  IndicesFixedSizeCollection(const UnsignedInteger size, const UnsignedInteger stride, const Indices & values);

  /** Constructor from external collection */
  explicit IndicesFixedSizeCollection(const Collection<Indices> & coll);

  /** Constructor with implementation */
  IndicesFixedSizeCollection(const Implementation & i);

  /** Constructor with implementation */
  IndicesFixedSizeCollection(const IndicesFixedSizeCollectionImplementation & i);

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Number of Indices */
  UnsignedInteger getSize() const;

#ifndef SWIG
  /** Iterators on values at the index-th element */
  iterator begin_at(const UnsignedInteger index);
  iterator end_at(const UnsignedInteger index);

  /** Iterators on constant values at the index-th element */
  const_iterator cbegin_at(const UnsignedInteger index) const;
  const_iterator cend_at(const UnsignedInteger index) const;

  /** Accessors to values[index][pos] */
  const UnsignedInteger & operator()(const UnsignedInteger index, const UnsignedInteger pos) const;
  UnsignedInteger & operator()(const UnsignedInteger index, const UnsignedInteger pos);
#endif

}; /* class IndicesFixedSizeCollection */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDICESFROZENCOLLECTION_HXX */
