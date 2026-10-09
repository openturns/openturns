//                                               -*- C++ -*-
/**
 *  @brief Point implements the classical mathematical point
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_POINT_HXX
#define OPENTURNS_POINT_HXX

#include <vector>
#include <algorithm>
#include <functional>
#include "openturns/PersistentObject.hxx"
#include "openturns/Description.hxx"
#include "openturns/Collection.hxx"
#include "openturns/DataContainer.hxx"
#include "openturns/AlgebraEngine.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Point
 *
 * Point implements the classical mathematical point
 */

class OT_API OT_WARN_UNUSED Point
  : public PersistentObject
{
  CLASSNAME

public:

  typedef Scalar*             iterator;
  typedef const Scalar*       const_iterator;
  typedef std::reverse_iterator<iterator>       reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef Collection<Scalar>           ScalarCollection;
  typedef Point                        ImplementationType;

  /** Default constructor */
  Point();

  /** Constructor with size */
  explicit Point(const UnsignedInteger size,
                 const Scalar value = 0.0);

  /** Constructor from a collection */
  Point(const Collection<Scalar> & coll);

  /** Constructor from a DataContainer (1D) */
  explicit Point(const DataContainer & dc);

  /** Initializer list constructor */
  Point(std::initializer_list<Scalar> initList);

  /** Constructor from a range of elements */
  template <typename InputIterator,
            typename std::enable_if<!std::is_integral<InputIterator>::value, bool>::type = true>
  Point(InputIterator first, InputIterator last)
    : PersistentObject()
    , data_()
  {
    assign(first, last);
  }

  /** Virtual constructor */
  Point * clone() const override;

#ifndef SWIG
  /** Copy constructor */
  Point(const Point & other);

  /** Assignment operator */
  Point & operator = (const Point & other);
#endif

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Check if the components are in increasing order */
  Bool isIncreasing() const;

  /** Check if the components are in nondecreasing order */
  Bool isNonDecreasing() const;

  /** Check if the components are in decreasing order */
  Bool isDecreasing() const;

  /** Check if the components are in nonincreasing order */
  Bool isNonIncreasing() const;

  /** Check if the components are in nonincreasing or nondecreasing order */
  Bool isMonotonic() const;

  /** Dimension accessor */
  inline UnsignedInteger getDimension() const
  {
    return data_.getSize();
  }
  inline UnsignedInteger getSize() const
  {
    return data_.getSize();
  }

#ifndef SWIG
  /** Operator[]() gives access to the elements of the point */
  inline Scalar & operator [] (const UnsignedInteger i)
  {
    return data_[i];
  }

  /** Operator[]() gives access to the elements of the const point */
  inline const Scalar & operator [] (const UnsignedInteger i) const
  {
    return data_[i];
  }
#endif

  /** At() gives access to the elements of the point but throws an exception if bounds are overcome */
  Scalar & at(const UnsignedInteger i);
  const Scalar & at(const UnsignedInteger i) const;

  /** Method __len__() is for Python */
  inline UnsignedInteger __len__() const
  {
    return getSize();
  }

  /* Method __eq__() is for Python */
  Bool __eq__(const Point & rhs) const
  {
    return *this == rhs;
  }

  /* Method __getitem__() is for Python */
  Scalar __getitem__(SignedInteger i) const;

  /* Method __setitem__() is for Python */
  void __setitem__(SignedInteger i,
                   const Scalar & val);

  /* Whether the list contains the value val */
  Bool contains(Scalar val) const;

  /** find returns the index of the first occurrence of the value */
  UnsignedInteger find(const Scalar & val) const;

  /** Method add() appends an element to the point */
  void add(const Scalar & elt);

  /** Method add() appends a collection to the point */
  void add(const Collection<Scalar> & coll);
  void add(const Point & coll);

  /** Select elements designated by their indices */
  ScalarCollection select(const Collection<UnsignedInteger> & marginalIndices) const;

  /** Method getSize() returns the number of elements of the point */

  /** Method resize() changes the size of the point */
  void resize(const UnsignedInteger newSize);

  /** Clear all elements of the point */
  void clear();

  /** Assign elements to the point */
  template <typename InputIterator>
  void assign(InputIterator first,
              InputIterator last)
  {
    /* Delegate to std::vector so that (count, value) calls with
       non-iterator arithmetic types keep their historical meaning */
    const std::vector<Scalar> tmp(first, last);
    data_.resize(tmp.size());
    std::copy(tmp.begin(), tmp.end(), data_.data());
  }

  /** Whether the point is empty */
  Bool isEmpty() const;

#ifndef SWIG
  /** Erase the elements between first and last */
  iterator erase(const iterator first, const iterator last);

  /** Erase the element pointed by position */
  iterator erase(iterator position);

  /** Erase the element pointed by position */
  iterator erase(UnsignedInteger position);
#endif

  /** Method begin() points to the first element of the point */
  inline iterator begin()
  {
    return data_.data();
  }
  inline const_iterator begin() const
  {
    return data_.data();
  }

  /** Method end() points beyond the last element of the point */
  inline iterator end()
  {
    return data_.data() + getSize();
  }
  inline const_iterator end() const
  {
    return data_.data() + getSize();
  }

  /** Method rbegin() points to the last element of the point */
  inline reverse_iterator rbegin()
  {
    return reverse_iterator(end());
  }
  inline const_reverse_iterator rbegin() const
  {
    return const_reverse_iterator(end());
  }

  /** Method rend() points before the first element of the point */
  inline reverse_iterator rend()
  {
    return reverse_iterator(begin());
  }
  inline const_reverse_iterator rend() const
  {
    return const_reverse_iterator(begin());
  }

  /** Returns a pointer to the block of memory */
  inline const Scalar * data() const
  {
    return data_.data();
  }
  inline Scalar * data()
  {
    return data_.data();
  }

  /** Give access to the underlying storage as a STL vector */
  std::vector<Scalar> toStdVector() const;

  /** Collection accessor */
  ScalarCollection getCollection() const;

#ifndef SWIG
  /** Implicit conversion to a scalar collection (compatibility bridge) */
  operator ScalarCollection() const;
#endif

#ifndef SWIG
  /**  In-place addition operator */
  Point & operator +=(const Point & other);

  /** In-place subtraction operator */
  Point & operator -=(const Point & other);
#endif

  /**  In-place product operator */
  Point & operator *=(const Scalar scalar);

  /**  In-place division operator */
  Point & operator /=(const Scalar scalar);

  /**  Norm */
  Scalar norm() const;

  /** l1-norm */
  Scalar norm1() const;

  /** linf-norm */
  Scalar normInf() const;

  /**  Norm^2 */
  Scalar normSquare() const;

  /** Normalize the vector */
  Point normalize() const;

  /** Square normalize the vector */
  Point normalizeSquare() const;

  /**  Dot product operator */
  Scalar dot(const Point & rhs) const;

  /** Convert to DataContainer (1D) */
  DataContainer toDataContainer() const;

  /** Construct a Point from a DataContainer (1D) */
  static Point FromDataContainer(const DataContainer & dc);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The flat storage of the coordinates */
  DataContainer data_;

}; /* class Point */


#ifndef SWIG
/** Comparison operator */
OT_API Bool operator == (const Point & lhs,
                         const Point & rhs);

/** Difference operator */
inline Bool operator != (const Point & lhs,
                         const Point & rhs)
{
  return !(lhs == rhs);
}

/** Ordering operator */
OT_API Bool operator < (const Point & lhs,
                        const Point & rhs);

/** Product operator */
OT_API Point operator * (const Scalar scalar,
                         const Point & point);

/** Product operator */
OT_API Point operator * (const Point & point,
                         const Scalar scalar);

/** Division operator */
OT_API Point operator / (const Point & point,
                         const Scalar scalar);

/** Addition operator */
OT_API Point operator + (const Point & lhs,
                         const Point & rhs);

/** Subtraction operator */
OT_API Point operator - (const Point & lhs,
                         const Point & rhs);

/** Unary minus operator */
OT_API Point operator - (const Point & lhs);

#endif



END_NAMESPACE_OPENTURNS

#ifndef SWIG

namespace std
{
template<> struct hash<OT::Point >
{
  size_t operator()(const OT::Point & coll) const noexcept
  {
    constexpr std::size_t adder = 0x9e3779b9;
    size_t seed = 0;
    for (OT::UnsignedInteger i = 0; i < coll.getSize(); ++ i)
    {
      size_t hi = hash<OT::Scalar> {}(coll[i]);
      seed = seed ^ (hi + adder + (seed << 6) + (seed >> 2));
    }
    return seed;
  }
};
}

#endif

#endif /* OPENTURNS_POINT_HXX */
