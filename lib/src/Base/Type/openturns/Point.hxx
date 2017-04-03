//                                               -*- C++ -*-
/**
 *  @brief Point implements the classical mathematical point
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
  : public PersistentCollection<NumericalScalar>
{
  CLASSNAME;

public:

  typedef PersistentCollection<NumericalScalar> InternalType;
  typedef Collection<NumericalScalar>           NumericalScalarCollection;
  typedef Point                        ImplementationType;

  /** Default constructor */
  Point();

  /** Constructor with size */
  explicit Point(const UnsignedInteger size,
                          const NumericalScalar value = 0.0);

  /** Constructor from a collection */
  Point(const Collection<NumericalScalar> & coll);

  /** Virtual constructor */
  virtual Point * clone() const;

  /** Description Accessor */
  virtual void setDescription(const Description & description);
  virtual Description getDescription() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Set small elements to zero */
  Point clean(const NumericalScalar threshold) const;

  /** Dimension accessor */
  inline UnsignedInteger getDimension() const
  {
    return PersistentCollection<NumericalScalar>::getSize();
  }
  inline UnsignedInteger getSize() const
  {
    return PersistentCollection<NumericalScalar>::getSize();
  }

  /** Collection accessor */
  inline const NumericalScalarCollection & getCollection() const
  {
    return *this;
  }

#ifndef SWIG
  /** Erase the elements between first and last */
  iterator erase(const iterator first, const iterator last);

  /** Erase the element pointed by position */
  iterator erase(iterator position);

  /** Erase the element pointed by position */
  iterator erase(UnsignedInteger position);

  /** In-place addition operator */
  Point & operator +=(const Point & other);

  /** In-place substraction operator */
  Point & operator -=(const Point & other);
#endif

  /**  In-place product operator */
  Point & operator *=(const NumericalScalar scalar);

  /**  In-place division operator */
  Point & operator /=(const NumericalScalar scalar);

  /**  Norm */
  NumericalScalar norm() const;

  /** l1-norm */
  NumericalScalar norm1() const;

  /** linf-norm */
  NumericalScalar normInf() const;

  /**  Norm^2 */
  NumericalScalar normSquare() const;

  /** Normalize the vector */
  Point normalize() const;

  /** Square normalize the vector */
  Point normalizeSquare() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

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
OT_API Point operator * (const NumericalScalar scalar,
                                  const Point & point);

/** Product operator */
OT_API Point operator * (const Point & point,
                                  const NumericalScalar scalar);

/** Division operator */
OT_API Point operator / (const Point & point,
                                  const NumericalScalar scalar);

/** Addition operator */
OT_API Point operator + (const Point & lhs,
                                  const Point & rhs);

/** Substraction operator */
OT_API Point operator - (const Point & lhs,
                                  const Point & rhs);

#endif

/** Dot product operator */
OT_API NumericalScalar dot(const Point & lhs,
                           const Point & rhs);



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POINT_HXX */
