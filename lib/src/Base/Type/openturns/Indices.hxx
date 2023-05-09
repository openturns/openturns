//                                               -*- C++ -*-
/**
 *  @brief Set of indices defined as a Collection of UnsignedInteger class
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
#ifndef OPENTURNS_INDICES_HXX
#define OPENTURNS_INDICES_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/PersistentCollection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Indices
 *
 * A class that holds a collection of indices
 */
class OT_API Indices :
  public PersistentCollection<UnsignedInteger>
{
  CLASSNAME

public:
  typedef PersistentCollection<UnsignedInteger>   InternalType;

  /** Default constructor */
  Indices()
    : InternalType()
  {
    // Nothing to do
  }

  /** Constructor that pre-allocate size elements */
  explicit Indices(const UnsignedInteger size) : InternalType(size)
  {
    // Nothing to do
  }

  /** Constructor that pre-allocate size elements with value */
  Indices(const UnsignedInteger size,
          const UnsignedInteger value) : InternalType(size, value)
  {
    // Nothing to do
  }

  /** Initializer list constructor */
  Indices(std::initializer_list<UnsignedInteger> initList)
    : InternalType(initList)
  {
    // Nothing to do
  }

  /** Constructor from a range of elements */
  template <typename InputIterator>
  Indices(const InputIterator first,
          const InputIterator last)
    : InternalType(first, last)
  {
    // Nothing to do
  }

  /** Check that no value is repeated and no value exceed the given bound */
  Bool check(const UnsignedInteger bound) const;

  /** Check if the indices are increasing */
  Bool isIncreasing() const;

  /** Fill the indices with a linear progression, starting from start value by step stepsize */
  void fill(const UnsignedInteger initialValue = 0,
            const UnsignedInteger stepSize = 1);

  /** Compute the complement between the indices and {0,...,n-1} for a given n */
  Indices complement(const UnsignedInteger n) const;

  /** Compute infinite-norm */
  UnsignedInteger normInf() const;

  /** Compute 1-norm */
  UnsignedInteger norm1() const;

#ifdef SWIG
  /** @copydoc Object::__repr__() const */
  String __repr__() const override;

  /** @copydoc Object::__str__() const */
  String __str__(const String & offset = "") const override;
#endif

}; /* class Indices */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INDICES_HXX */
