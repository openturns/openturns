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
#include <cmath>
#include "openturns/Point.hxx"
#include "openturns/Exception.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/AlgebraEngine.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Point)

static const Factory<PersistentCollection<Scalar> > Factory_PersistentCollection_Scalar;

static const Factory<Point> Factory_Point;

/* Default constructor */
Point::Point()
  : PersistentObject()
  , data_()
{
  // Nothing to do
}

/* Constructor with size */
Point::Point(const UnsignedInteger size,
             const Scalar value)
  : PersistentObject()
  , data_(size, value)
{
  // Nothing to do
}


/* Constructor from a collection */
Point::Point(const Collection<Scalar> & coll)
  : PersistentObject()
  , data_()
{
  assign(coll.begin(), coll.end());
}

/* Constructor from a DataContainer (1D) */
Point::Point(const DataContainer & dc)
  : PersistentObject()
  , data_()
{
  if (dc.isEmpty()) return;
  if (dc.getDimension() > 1)
    throw InvalidArgumentException(HERE)
        << "Cannot construct a 1D Point from a DataContainer with dimension " << dc.getDimension();
  const UnsignedInteger n = dc.getSize();
  data_.resize(n);
  std::copy(dc.data(), dc.data() + n, data_.data());
}

Point::Point(std::initializer_list<Scalar> initList)
  : PersistentObject()
  , data_()
{
  assign(initList.begin(), initList.end());
}

#ifndef SWIG
/* Copy constructor */
Point::Point(const Point & other)
  : PersistentObject(other)
  , data_(other.data_)
{
}

/* Assignment operator */
Point & Point::operator = (const Point & other)
{
  if (this != &other)
  {
    PersistentObject::operator = (other);
    data_ = other.data_;
  }
  return *this;
}
#endif

/* Virtual constructor */
Point * Point::clone() const
{
  return new Point(*this);
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
  OSS ossValues(true);
  ossValues << "[";
  std::copy( begin(), end(), OSS_iterator<Scalar>(ossValues, ",") );
  ossValues << "]";
  return OSS(true) << "class=" << Point::GetClassName()
         << " name=" << getName()
         << " dimension=" << getDimension()
         << " values=" << String(ossValues);
}

namespace
{
String pointValuesToString(const Point & point, const String & offset)
{
  OSS oss(false);
  oss << offset << "[";
  std::copy( point.begin(), point.end(), OSS_iterator<Scalar>(oss, ",") );
  oss << "]";
  if (point.getSize() >= ResourceMap::GetAsUnsignedInteger("Collection-size-visible-in-str-from"))
    oss << "#" << point.getSize();
  return oss;
}
}

String Point::__str__(const String & offset) const
{
  return pointValuesToString(*this, offset);
}

/* At() gives access to the elements of the point but throws an exception if bounds are overcome */
Scalar & Point::at(const UnsignedInteger i)
{
  if (!(i < getSize())) throw OutOfBoundException(HERE) << "Index i is out of range. Got " << i << " (size=" << getSize() << ")";
  return (*this)[i];
}

const Scalar & Point::at(const UnsignedInteger i) const
{
  if (!(i < getSize())) throw OutOfBoundException(HERE) << "Index i is out of range. Got " << i << " (size=" << getSize() << ")";
  return (*this)[i];
}

/* Method __getitem__() is for Python */
Scalar Point::__getitem__(SignedInteger i) const
{
  if (i < 0)
  {
    i += getSize();
  }
  return at(i);
}

/* Method __setitem__() is for Python */
void Point::__setitem__(SignedInteger i,
                        const Scalar & val)
{
  if (i < 0)
  {
    i += getSize();
  }
  at(i) = val;
}

/* Whether the list contains the value val */
Bool Point::contains(Scalar val) const
{
  return (std::find(begin(), end(), val) != end());
}

/** find returns the index of the first occurrence of the value */
UnsignedInteger Point::find(const Scalar & val) const
{
  return std::find(begin(), end(), val) - begin();
}

/** Method add() appends an element to the point */
void Point::add(const Scalar & elt)
{
  const UnsignedInteger oldSize = getSize();
  data_.resize(oldSize + 1);
  data_[oldSize] = elt;
}

/** Method add() appends a collection to the point */
void Point::add(const Collection<Scalar> & coll)
{
  const UnsignedInteger oldSize = getSize();
  data_.resize(oldSize + coll.getSize());
  std::copy(coll.begin(), coll.end(), data_.data() + oldSize);
}

void Point::add(const Point & coll)
{
  add(static_cast<const Collection<Scalar> &>(coll));
}

/** Select elements designated by their indices */
Point::ScalarCollection Point::select(const Collection<UnsignedInteger> & marginalIndices) const
{
  ScalarCollection marginalCollection(marginalIndices.getSize());
  for (UnsignedInteger i = 0; i < marginalIndices.getSize(); ++ i)
  {
    const UnsignedInteger index = marginalIndices[i];
    if (index >= getSize())
      throw OutOfBoundException(HERE) << "Selection index is out of range (" << index << ") as size=" << getSize();
    marginalCollection[i] = (*this)[index];
  }
  return marginalCollection;
}

/** Method resize() changes the size of the point */
void Point::resize(const UnsignedInteger newSize)
{
  data_.resize(newSize);
}

/** Clear all elements of the point */
void Point::clear()
{
  data_.clear();
}

/** Whether the point is empty */
Bool Point::isEmpty() const
{
  return getSize() == 0;
}

/* Erase the elements between first and last */
Point::iterator Point::erase(const iterator first, const iterator last)
{
  const UnsignedInteger offsetFirst = first - begin();
  const UnsignedInteger offsetLast = last - begin();
  data_.erase(offsetFirst, offsetLast);
  return begin() + offsetFirst;
}

/* Erase the element pointed by position */
Point::iterator Point::erase(iterator position)
{
  return erase(position, position + 1);
}

/* Erase the element pointed by position */
Point::iterator Point::erase(UnsignedInteger position)
{
  return erase(begin() + position);
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
  if (&lhs == &rhs) return true;
  return (lhs.getSize() == rhs.getSize()) &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}



/* Ordering operator */
Bool operator <(const Point & lhs,
                const Point & rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
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

/* Give access to the underlying storage as a STL vector */
std::vector<Scalar> Point::toStdVector() const
{
  return std::vector<Scalar>(begin(), end());
}

/* Collection accessor */
Point::ScalarCollection Point::getCollection() const
{
  return ScalarCollection(begin(), end());
}

/* Implicit conversion to a scalar collection (compatibility bridge) */
Point::operator ScalarCollection() const
{
  return getCollection();
}

/* Convert to DataContainer (1D) */
DataContainer Point::toDataContainer() const
{
  const UnsignedInteger n = getDimension();
  if (n == 0) return DataContainer();
  DataContainer dc(n);
  std::copy(begin(), end(), dc.data());
  return dc;
}

/* Construct a Point from a DataContainer (1D) */
Point Point::FromDataContainer(const DataContainer & dc)
{
  if (dc.isEmpty()) return Point();
  if (dc.getDimension() > 1)
    throw InvalidArgumentException(HERE)
        << "Cannot construct a 1D Point from a DataContainer with dimension " << dc.getDimension();
  return Point(dc);
}

/* Method save() stores the object through the StorageManager */
void Point::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  // Same layout as the former PersistentCollection<Scalar>: a size attribute
  // followed by the values, so existing studies remain readable.
  adv.saveAttribute("size", getSize());
  AdvocateIterator<Scalar> adv_it(adv);
  const Scalar * pData = data_.data();
  const UnsignedInteger totalSize = getSize();
  for (UnsignedInteger i = 0; i < totalSize; ++i, ++adv_it)
  {
    *adv_it = pData[i];
  }
}


/* Method load() reloads the object from the StorageManager */
void Point::load(Advocate & adv)
{
  PersistentObject::load(adv);
  UnsignedInteger size = 0;
  adv.loadAttribute("size", size);
  data_.resize(size);
  AdvocateIterator<Scalar> adv_it(adv);
  Scalar * pData = data_.data();
  for (UnsignedInteger i = 0; i < size; ++i, ++adv_it)
  {
    pData[i] = adv_it();
  }
}


END_NAMESPACE_OPENTURNS
