//                                               -*- C++ -*-
/**
 *  @brief Point implements the classical mathematical point
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/PersistentCollection.hxx"
#include "openturns/Description.hxx"
#include "openturns/Indices.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Point
 *
 * Point implements the classical mathematical point
 */

class OT_API Point
  : public PersistentCollection<Scalar>
{
  CLASSNAME

public:

  typedef PersistentCollection<Scalar> InternalType;
  typedef Collection<Scalar>           ScalarCollection;
  typedef Point                        ImplementationType;

  /** Default constructor */
  Point();

  /** Constructor with size */
  explicit Point(const UnsignedInteger size,
                 const Scalar value = 0.0);

  /** Constructor from a collection */
  Point(const Collection<Scalar> & coll);

  /** Initializer list constructor */
  Point(std::initializer_list<Scalar> initList);

  /** Constructor from a range of elements */
  template <typename InputIterator>
  Point(InputIterator first, InputIterator last)
  : InternalType(first, last) {}

  /** Virtual constructor */
  Point * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Set small elements to zero */
  Point clean(const Scalar threshold) const;

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
    return PersistentCollection<Scalar>::getSize();
  }
  inline UnsignedInteger getSize() const
  {
    return PersistentCollection<Scalar>::getSize();
  }

  /** Collection accessor */
  inline const ScalarCollection & getCollection() const
  {
    return *this;
  }

#ifndef SWIG
  /** Erase the elements between first and last */
  iterator erase(const iterator first, const iterator last) override;

  /** Erase the element pointed by position */
  iterator erase(iterator position) override;

  /** Erase the element pointed by position */
  iterator erase(UnsignedInteger position);

  /** In-place addition operator */
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

  /** Dot product operator */
  Scalar dot(const Point & rhs) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  //      /** The description of all components */
  //      Description:: p_description_;

}; /* class Point */


#ifndef SWIG
/** Comparison operator */
OT_API Bool operator == (const Point & lhs,
                         const Point & rhs);

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

#endif /* OPENTURNS_POINT_HXX */
