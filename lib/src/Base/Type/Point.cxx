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
  : InternalType() //,
    // p_description_()
{
  // Nothing to do
}

/* Constructor with size */
Point::Point(const UnsignedInteger size,
             const Scalar value)
  : InternalType(size, value)
{
  // Nothing to do
}


/* Constructor from a collection */
Point::Point(const Collection<Scalar> & coll)
  : InternalType(coll)
{
  // Nothing to do
}

Point::Point(std::initializer_list<Scalar> initList)
  : InternalType(initList)
{
  // Nothing to do
}

/* Virtual constructor */
Point * Point::clone() const
{
  return new Point(*this);
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

/* Check if the components are in increasing order */
Bool Point::isIncreasing() const
{
  const UnsignedInteger size = getSize();
  Bool isIncreasing = true;
  for (UnsignedInteger i = 1; isIncreasing && i < size; ++i)
    isIncreasing = isIncreasing && (*this)[i - 1] < (*this)[i];
  return isIncreasing;
}

/* Check if the components are in nondecreasing order */
Bool Point::isNonDecreasing() const
{
  const UnsignedInteger size = getSize();
  Bool isNonDecreasing = true;
  for (UnsignedInteger i = 1; isNonDecreasing && i < size; ++i)
    isNonDecreasing = isNonDecreasing && (*this)[i - 1] <= (*this)[i];
  return isNonDecreasing;
}

/* Check if the components are in decreasing order */
Bool Point::isDecreasing() const
{
  const UnsignedInteger size = getSize();
  Bool isDecreasing = true;
  for (UnsignedInteger i = 1; isDecreasing && i < size; ++i)
    isDecreasing = isDecreasing && (*this)[i - 1] > (*this)[i];
  return isDecreasing;
}

/* Check if the components are in nonincreasing order */
Bool Point::isNonIncreasing() const
{
  const UnsignedInteger size = getSize();
  Bool isNonIncreasing = true;
  for (UnsignedInteger i = 1; isNonIncreasing && i < size; ++i)
    isNonIncreasing = isNonIncreasing && (*this)[i - 1] >= (*this)[i];
  return isNonIncreasing;
}

/* Check if the components are in increasing or decreasing order */
Bool Point::isMonotonic() const
{
  // This order assumes that the nondecreasing case is the most frequent case
  return isNonDecreasing() || isNonIncreasing();
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
  if (n > 0)
    daxpy_(&n, &alpha, const_cast<double*>(&other[0]), &one, &(*this)[0], &one);
  return *this;
}



/* Subtraction operator */
Point operator - (const Point & lhs, const Point & rhs)
{
  int n = lhs.getDimension();
  if (n != (int)rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be subtracted (LHS dimension = "
        << n
        << "; RHS dimension = "
        << rhs.getDimension();

  // We create a Point of the same dimension as both points for holding the result
  Point result(lhs);
  double alpha = -1.0;
  int one = 1;
  if (n > 0)
    daxpy_(&n, &alpha, const_cast<double*>(&rhs[0]), &one, &result[0], &one);
  return result;
}


/* Unary minus operator */
Point operator - (const Point & lhs)
{
  double alpha = -1.0;
  Point result(lhs * alpha);
  return result;
}


/* In-place subtraction operator */
Point & Point::operator -=(const Point & other)
{
  int n = getDimension();
  if (n != (int)other.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be subtracted (LHS dimension = "
        << n
        << "; RHS dimension = " <<
        other.getDimension();

  double alpha = -1.0;
  int one = 1;
  if (n > 0)
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
  if (n > 0)
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
  if (n > 0)
    dscal_(&n, &alpha, &result[0], &one);
  return result;
}

/*  In-place product operator */
Point & Point::operator *=(const Scalar scalar)
{
  int n = getDimension();
  double alpha = scalar;
  int one = 1;
  if (n > 0)
    dscal_(&n, &alpha, &(*this)[0], &one);
  return *this;
}

/* Division operator */
Point operator /(const Point & point,
                 const Scalar scalar)
{
  if (!(scalar > 0.0 || scalar < 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot divide by 0.";
  int n = point.getDimension();
  double alpha = 1.0 / scalar;
  Point result(point);
  int one = 1;
  if (n > 0)
    dscal_(&n, &alpha, &result[0], &one);
  return result;
}

/*  In-place division operator */
Point & Point::operator /=(const Scalar scalar)
{
  if (!(scalar > 0.0 || scalar < 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot divide by 0.";
  int n = getDimension();
  double alpha = 1.0 / scalar;
  int one = 1;
  if (n > 0)
    dscal_(&n, &alpha, &(*this)[0], &one);
  return *this;
}


/* Dot product operator */
Scalar Point::dot(const Point & rhs) const
{
  int n = getDimension();
  if (n != (int)rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "Points of different dimensions cannot be added (LHS dimension = "
        << n
        << "; RHS dimension = "
        << rhs.getDimension();

  int one = 1;
  if (n > 0)
    return ddot_(&n, const_cast<double*>(&(*this)[0]), &one, const_cast<double*>(&rhs[0]), &one);
  else
    return 0.0;
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
  int n = getDimension();
  int one(1);
  if (n > 0)
    return dnrm2_(&n, const_cast<double*>(&(*this)[0]), &one);
  else
    return 0.0;
}

/* l1-norm */
Scalar Point::norm1() const
{
  int n = getDimension();
  int one(1);
  if (n > 0)
    return dasum_(&n, const_cast<double*>(&(*this)[0]), &one);
  else
    return 0.0;
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
  return dot(*this);
}


/*  Normalize */
Point Point::normalize() const
{
  const Scalar theNorm = norm();
  if (!(theNorm > 0.0)) throw InternalException(HERE) << "Error: cannot normalize a null vector";
  return (*this) / theNorm;
}

/*  Square normalize */
Point Point::normalizeSquare() const
{
  const Scalar theNormSquare = normSquare();
  if (!(theNormSquare > 0.0)) throw InternalException(HERE) << "Error: cannot square normalize a null vector";
  Point result(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) result[i] = pow((*this)[i], 2) / theNormSquare;
  return result;
}

/* Method save() stores the object through the StorageManager */
void Point::save(Advocate & adv) const
{
  InternalType::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void Point::load(Advocate & adv)
{
  InternalType::load(adv);
}


END_NAMESPACE_OPENTURNS
