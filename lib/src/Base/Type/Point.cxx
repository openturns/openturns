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
#include <cmath>
#include "openturns/Point.hxx"
#include "openturns/Exception.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Point)

static const Factory<PersistentCollection<Scalar> > Factory_PersistentCollection_Scalar;

static const Factory<Point> Factory_Point;

/* Default constructor */
Point::Point()
  : PersistentCollection<Scalar>() //,
    // p_description_()
{
  // Nothing to do
}

/* Constructor with size */
Point::Point(const UnsignedInteger size,
             const Scalar value)
  : PersistentCollection<Scalar>(size, value)
{
  // Nothing to do
}


/* Constructor from a collection */
Point::Point(const Collection<Scalar> & coll)
  : PersistentCollection<Scalar>(coll)
{
  // Nothing to do
}


/* Virtual constructor */
Point * Point::clone() const
{
  return new Point(*this);
}

void Point::setDescription(const Description & description)
{
  // Nothing to do
}

Description Point::getDescription() const
{
  return Description( getDimension() );
}

/* Set small elements to zero */
Point Point::clean(const Scalar threshold) const
{
  // Nothing to do for nonpositive threshold
  if (threshold <= 0.0) return *this;
  const UnsignedInteger size = getSize();
  Point result(size, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar value = (*this)[i];
    // Things are done this way to prevent spurious -0.0
    if (std::abs(value) < 0.5 * threshold) result[i] = 0.0;
    else result[i] = threshold * round(value / threshold);
  }
  return result;
}


/* String converter */
String Point::__repr__() const
{
  return OSS(true) << "class=" << Point::GetClassName()
         << " name=" << getName()
         << " dimension=" << getDimension()
         << " values=" << PersistentCollection<Scalar>::__repr__();
}

String Point::__str__(const String & offset) const
{
  return PersistentCollection<Scalar>::__str__(offset);
}

/* Erase the elements between first and last */
Point::iterator Point::erase(const iterator first, const iterator last)
{
  return PersistentCollection<Scalar>::erase(first, last);
}

/* Erase the element pointed by position */
Point::iterator Point::erase(iterator position)
{
  return PersistentCollection<Scalar>::erase(position);
}

/* Erase the element pointed by position */
Point::iterator Point::erase(UnsignedInteger position)
{
  return PersistentCollection<Scalar>::erase(begin() + position);
}



/* Addition operator */
Point operator + (const Point & lhs,
                  const Point & rhs)
{
  int n = lhs.getDimension();
  if (n != (int)rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be added (LHS dimension = "
        << n
        << "; RHS dimension = "
        << rhs.getDimension();

  // We create a Point of the same dimension as both points for holding the result
  Point result(lhs);
  double alpha = 1.0;
  int one = 1;
  daxpy_(&n, &alpha, const_cast<double*>(&rhs[0]), &one, &result[0], &one);
  return result;
}



/* In-place addition operator */
Point & Point::operator +=(const Point & other)
{
  int n = getDimension();
  if (n != (int)other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be added (LHS dimension = "
        << n
        << "; RHS dimension = "
        << other.getDimension();
  double alpha = 1.0;
  int one = 1;
  daxpy_(&n, &alpha, const_cast<double*>(&other[0]), &one, &(*this)[0], &one);

  return *this;
}



/* Substraction operator */
Point operator - (const Point & lhs, const Point & rhs)
{
  int n = lhs.getDimension();
  if (n != (int)rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be substracted (LHS dimension = "
        << n
        << "; RHS dimension = "
        << rhs.getDimension();

  // We create a Point of the same dimension as both points for holding the result
  Point result(lhs);
  double alpha = -1.0;
  int one = 1;
  daxpy_(&n, &alpha, const_cast<double*>(&rhs[0]), &one, &result[0], &one);
  return result;
}



/* In-place substraction operator */
Point & Point::operator -=(const Point & other)
{
  int n = getDimension();
  if (n != (int)other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be substracted (LHS dimension = "
        << n
        << "; RHS dimension = " <<
        other.getDimension();

  double alpha = -1.0;
  int one = 1;
  daxpy_(&n, &alpha, const_cast<double*>(&other[0]), &one, &(*this)[0], &one);

  return *this;
}



/* Product operator */
Point operator *(const Point & point,
                 const Scalar scalar)
{
  int n = point.getDimension();
  double alpha = scalar;
  Point result(point);
  int one = 1;
  dscal_(&n, &alpha, &result[0], &one);
  return result;
}

Point operator *(const Scalar scalar,
                 const Point & point)
{
  int n = point.getDimension();
  double alpha = scalar;
  Point result(point);
  int one = 1;
  dscal_(&n, &alpha, &result[0], &one);
  return result;
}

/*  In-place product operator */
Point & Point::operator *=(const Scalar scalar)
{
  int n = getDimension();
  double alpha = scalar;
  int one = 1;
  dscal_(&n, &alpha, &(*this)[0], &one);
  return *this;
}

/* Division operator */
Point operator /(const Point & point,
                 const Scalar scalar)
{
  if (scalar == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot divide by 0.";
  int n = point.getDimension();
  double alpha = 1.0 / scalar;
  Point result(point);
  int one = 1;
  dscal_(&n, &alpha, &result[0], &one);
  return result;
}

/*  In-place division operator */
Point & Point::operator /=(const Scalar scalar)
{
  if (scalar == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot divide by 0.";
  int n = getDimension();
  double alpha = 1.0 / scalar;
  int one = 1;
  dscal_(&n, &alpha, &(*this)[0], &one);
  return *this;
}


/* Dot product operator */
Scalar dot(const Point & lhs,
           const Point & rhs)
{
  int n = lhs.getDimension();
  if (n != (int)rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be added (LHS dimension = "
        << n
        << "; RHS dimension = "
        << rhs.getDimension();

  int one = 1;
  return ddot_(&n, const_cast<double*>(&lhs[0]), &one, const_cast<double*>(&rhs[0]), &one);
}


/* Comparison operator */
Bool operator ==(const Point & lhs,
                 const Point & rhs)
{
  return static_cast<const PersistentCollection<Scalar> >(lhs) == static_cast<const PersistentCollection<Scalar> >(rhs);
}



/* Ordering operator */
Bool operator <(const Point & lhs,
                const Point & rhs)
{
  return static_cast<const PersistentCollection<Scalar> >(lhs) < static_cast<const PersistentCollection<Scalar> >(rhs);
}



/*  Norm */
Scalar Point::norm() const
{
  int n(getDimension());
  int one(1);
  return dnrm2_(&n, const_cast<double*>(&(*this)[0]), &one);
}

/* l1-norm */
Scalar Point::norm1() const
{
  int n(getDimension());
  int one(1);
  return dasum_(&n, const_cast<double*>(&(*this)[0]), &one);
}

/* linf-norm */
Scalar Point::normInf() const
{
  const UnsignedInteger dimension = getDimension();
  if (dimension == 0) return 0.0;
  const Scalar minValue = *std::min_element(begin(), end());
  const Scalar maxValue = *std::max_element(begin(), end());
  return std::max(maxValue, -minValue);
}

/*  Norm^2 */
Scalar Point::normSquare() const
{
  return dot(*this, *this);
}


/*  Normalize */
Point Point::normalize() const
{
  const Scalar theNorm = norm();
  if (theNorm == 0.0) throw InternalException(HERE) << "Error: cannot normalize a null vector";
  return (*this) / theNorm;
}

/*  Square normalize */
Point Point::normalizeSquare() const
{
  const Scalar theNormSquare = normSquare();
  if (theNormSquare == 0.0) throw InternalException(HERE) << "Error: cannot square normalize a null vector";
  Point result(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) result[i] = pow((*this)[i], 2) / theNormSquare;
  return result;
}

/* Method save() stores the object through the StorageManager */
void Point::save(Advocate & adv) const
{
  PersistentCollection<Scalar>::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void Point::load(Advocate & adv)
{
  PersistentCollection<Scalar>::load(adv);
}


END_NAMESPACE_OPENTURNS
