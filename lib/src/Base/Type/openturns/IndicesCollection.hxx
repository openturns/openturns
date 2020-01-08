//                                               -*- C++ -*-
/**
 *  @brief IndicesCollectionImplementation stores a Collection of Indices contiguously.
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
#ifndef OPENTURNS_INDICESCOLLECTION_HXX
#define OPENTURNS_INDICESCOLLECTION_HXX

#include "openturns/IndicesCollectionImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/OStream.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class IndicesCollection
 *
 * IndicesCollection stores a Collection of Indices contiguously.
 */

class OT_API IndicesCollection :
  public TypedInterfaceObject<IndicesCollectionImplementation>
{
  CLASSNAME

public:

#ifndef SWIG
  typedef IndicesCollectionImplementation::iterator        iterator;
  typedef IndicesCollectionImplementation::const_iterator  const_iterator;
#endif

  /** Default constructor */
  IndicesCollection();

  /** Constructor from size and stride */
  IndicesCollection(const UnsignedInteger size, const UnsignedInteger stride);

  /** Constructor from size, stride and values */
  IndicesCollection(const UnsignedInteger size, const UnsignedInteger stride, const Indices & values);

#ifndef SWIG
  /** Constructor from external collection */
  explicit IndicesCollection(const Collection<Indices> & coll);

  /** Constructor with implementation */
  IndicesCollection(const Implementation & i);

  /** Constructor with implementation */
  IndicesCollection(const IndicesCollectionImplementation & i);
#endif

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

}; /* class IndicesCollection */

#ifndef SWIG
/** Stream operator, to mimic Collection<UnsignedInteger> */
inline
std::ostream & operator << (std::ostream & os, const IndicesCollection & collection)
{
  return os << collection.__repr__();
}

inline
OStream & operator << (OStream & OS, const IndicesCollection & collection)
{
  return OS << collection.__str__();
}
#endif



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDICESCOLLECTION_HXX */
