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
#ifndef OPENTURNS_INDICESFROZENCOLLECTIONIMPLEMENTATION_HXX
#define OPENTURNS_INDICESFROZENCOLLECTIONIMPLEMENTATION_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class IndicesFixedSizeCollectionImplementation
 *
 * IndicesFixedSizeCollectionImplementation stores a Collection of Indices contiguously.
 */

class OT_API IndicesFixedSizeCollectionImplementation
  : public PersistentObject
{
  CLASSNAME

public:
  typedef Collection<UnsignedInteger>::iterator        iterator;
  typedef Collection<UnsignedInteger>::const_iterator  const_iterator;

  /** Default constructor */
  IndicesFixedSizeCollectionImplementation();

  /** Constructor from size and stride */
  IndicesFixedSizeCollectionImplementation(const UnsignedInteger size, const UnsignedInteger stride);

  /** Constructor from size, stride and values */
  IndicesFixedSizeCollectionImplementation(const UnsignedInteger size, const UnsignedInteger stride, const Indices & values);

  /** Constructor from a Collection<Indices> */
  explicit IndicesFixedSizeCollectionImplementation(const Collection<Indices> & coll);

  /** Virtual constructor */
  virtual IndicesFixedSizeCollectionImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Number of Indices */
  virtual UnsignedInteger getSize() const;

  /** Comparison operators */
  Bool operator == (const IndicesFixedSizeCollectionImplementation & rhs) const;
  inline Bool operator != (const IndicesFixedSizeCollectionImplementation & rhs) const
  {
    return !((*this) == rhs);
  }

#ifndef SWIG
  /** Iterators on values at the index-th element */
  virtual iterator begin_at(const UnsignedInteger position);
  virtual iterator end_at(const UnsignedInteger position);

  /** Iterators on constant values at the index-th element */
  virtual const_iterator cbegin_at(const UnsignedInteger position) const;
  virtual const_iterator cend_at(const UnsignedInteger position) const;

  /** Accessors to values[index][pos] */
  virtual const UnsignedInteger & operator()(const UnsignedInteger index, const UnsignedInteger pos) const;
  virtual UnsignedInteger & operator()(const UnsignedInteger index, const UnsignedInteger pos);
#endif

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  /** Number of Indices */
  UnsignedInteger size_;

  /** List of values, stored contiguously */
  Indices values_;

  /** Gives for each element its first index in values_ */
  Indices offsets_;

};


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDICESFROZENCOLLECTIONIMPLEMENTATION_HXX */
