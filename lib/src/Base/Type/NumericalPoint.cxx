//                                               -*- C++ -*-
/**
 *  @brief NumericalPoint implements the classical mathematical point
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/NumericalPoint.hxx"
#include "openturns/Exception.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Lapack.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(NumericalPoint);

static const Factory<PersistentCollection<NumericalScalar> > RegisteredFactory_alt1;

static const Factory<NumericalPoint> RegisteredFactory;

/* Default constructor */
NumericalPoint::NumericalPoint()
  : PersistentCollection<NumericalScalar>() //,
  // p_description_()
{
  // Nothing to do
}

/* Constructor with size */
NumericalPoint::NumericalPoint(const UnsignedInteger size,
                               const NumericalScalar value)
  : PersistentCollection<NumericalScalar>(size, value)
{
  // Nothing to do
}


/* Constructor from a collection */
NumericalPoint::NumericalPoint(const Collection<NumericalScalar> & coll)
  : PersistentCollection<NumericalScalar>(coll)
{
  // Nothing to do
}


/* Virtual constructor */
NumericalPoint * NumericalPoint::clone() const
{
  return new NumericalPoint(*this);
}

void NumericalPoint::setDescription(const Description & description)
{
  // Nothing to do
}

Description NumericalPoint::getDescription() const
{
  return Description( getDimension() );
}

/* Set small elements to zero */
NumericalPoint NumericalPoint::clean(const NumericalScalar threshold) const
{
  // Nothing to do for nonpositive threshold
  if (threshold <= 0.0) return *this;
  const UnsignedInteger size(getSize());
  NumericalPoint result(size, 0.0);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const NumericalScalar value((*this)[i]);
    // Things are done this way to prevent spurious -0.0
    if (std::abs(value) < 0.5 * threshold) result[i] = 0.0;
    else result[i] = threshold * round(value / threshold);
  }
  return result;
}


/* String converter */
String NumericalPoint::__repr__() const
{
  return OSS(true) << "class=" << NumericalPoint::GetClassName()
         << " name=" << getName()
         << " dimension=" << getDimension()
         << " values=" << PersistentCollection<NumericalScalar>::__repr__();
}

String NumericalPoint::__str__(const String & offset) const
{
  return PersistentCollection<NumericalScalar>::__str__(offset);
}

/* Erase the elements between first and last */
NumericalPoint::iterator NumericalPoint::erase(const iterator first, const iterator last)
{
  return PersistentCollection<NumericalScalar>::erase(first, last);
}

/* Erase the element pointed by position */
NumericalPoint::iterator NumericalPoint::erase(iterator position)
{
  return PersistentCollection<NumericalScalar>::erase(position);
}

/* Erase the element pointed by position */
NumericalPoint::iterator NumericalPoint::erase(UnsignedInteger position)
{
  return PersistentCollection<NumericalScalar>::erase(begin() + position);
}



/* Addition operator */
NumericalPoint operator + (const NumericalPoint & lhs,
                           const NumericalPoint & rhs)
{
  int n = lhs.getDimension();
  if (n != (int)rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "NumericalPoints of different dimensions cannot be added (LHS dimension = "
        << n
        << "; RHS dimension = "
        << rhs.getDimension();

  // We create a NumericalPoint of the same dimension as both points for holding the result
  NumericalPoint result(lhs);
  double alpha = 1.0;
  int one = 1;
  daxpy_(&n, &alpha, const_cast<double*>(&rhs[0]), &one, &result[0], &one);
  return result;
}



/* In-place addition operator */
NumericalPoint & NumericalPoint::operator +=(const NumericalPoint & other)
{
  int n = getDimension();
  if (n != (int)other.getDimension())
    throw InvalidArgumentException(HERE)
        << "NumericalPoints of different dimensions cannot be added (LHS dimension = "
        << n
        << "; RHS dimension = "
        << other.getDimension();
  double alpha = 1.0;
  int one = 1;
  daxpy_(&n, &alpha, const_cast<double*>(&other[0]), &one, &(*this)[0], &one);

  return *this;
}



/* Substraction operator */
NumericalPoint operator - (const NumericalPoint & lhs, const NumericalPoint & rhs)
{
  int n = lhs.getDimension();
  if (n != (int)rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "NumericalPoints of different dimensions cannot be substracted (LHS dimension = "
        << n
        << "; RHS dimension = "
        << rhs.getDimension();

  // We create a NumericalPoint of the same dimension as both points for holding the result
  NumericalPoint result(lhs);
  double alpha = -1.0;
  int one = 1;
  daxpy_(&n, &alpha, const_cast<double*>(&rhs[0]), &one, &result[0], &one);
  return result;
}



/* In-place substraction operator */
NumericalPoint & NumericalPoint::operator -=(const NumericalPoint & other)
{
  int n = getDimension();
  if (n != (int)other.getDimension())
    throw InvalidArgumentException(HERE)
        << "NumericalPoints of different dimensions cannot be substracted (LHS dimension = "
        << n
        << "; RHS dimension = " <<
        other.getDimension();

  double alpha = -1.0;
  int one = 1;
  daxpy_(&n, &alpha, const_cast<double*>(&other[0]), &one, &(*this)[0], &one);

  return *this;
}



/* Product operator */
NumericalPoint operator *(const NumericalPoint & point,
                          const NumericalScalar scalar)
{
  int n = point.getDimension();
  double alpha = scalar;
  NumericalPoint result(point);
  int one = 1;
  dscal_(&n, &alpha, &result[0], &one);
  return result;
}

NumericalPoint operator *(const NumericalScalar scalar,
                          const NumericalPoint & point)
{
  int n = point.getDimension();
  double alpha = scalar;
  NumericalPoint result(point);
  int one = 1;
  dscal_(&n, &alpha, &result[0], &one);
  return result;
}

/*  In-place product operator */
NumericalPoint & NumericalPoint::operator *=(const NumericalScalar scalar)
{
  int n = getDimension();
  double alpha = scalar;
  int one = 1;
  dscal_(&n, &alpha, &(*this)[0], &one);
  return *this;
}

/* Division operator */
NumericalPoint operator /(const NumericalPoint & point,
                          const NumericalScalar scalar)
{
  if (scalar == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot divide by 0.";
  int n = point.getDimension();
  double alpha = 1.0 / scalar;
  NumericalPoint result(point);
  int one = 1;
  dscal_(&n, &alpha, &result[0], &one);
  return result;
}

/*  In-place division operator */
NumericalPoint & NumericalPoint::operator /=(const NumericalScalar scalar)
{
  if (scalar == 0.0) throw InvalidArgumentException(HERE) << "Error: cannot divide by 0.";
  int n = getDimension();
  double alpha = 1.0 / scalar;
  int one = 1;
  dscal_(&n, &alpha, &(*this)[0], &one);
  return *this;
}


/* Dot product operator */
NumericalScalar dot(const NumericalPoint & lhs,
                    const NumericalPoint & rhs)
{
  int n = lhs.getDimension();
  if (n != (int)rhs.getDimension())
    throw InvalidArgumentException(HERE)
        << "NumericalPoints of different dimensions cannot be added (LHS dimension = "
        << n
        << "; RHS dimension = "
        << rhs.getDimension();

  int one = 1;
  return ddot_(&n, const_cast<double*>(&lhs[0]), &one, const_cast<double*>(&rhs[0]), &one);
}


/* Comparison operator */
Bool operator ==(const NumericalPoint & lhs,
                 const NumericalPoint & rhs)
{
  return static_cast<const PersistentCollection<NumericalScalar> >(lhs) == static_cast<const PersistentCollection<NumericalScalar> >(rhs);
}



/* Ordering operator */
Bool operator <(const NumericalPoint & lhs,
                const NumericalPoint & rhs)
{
  return static_cast<const PersistentCollection<NumericalScalar> >(lhs) < static_cast<const PersistentCollection<NumericalScalar> >(rhs);
}



/*  Norm */
NumericalScalar NumericalPoint::norm() const
{
  int n = getDimension();
  int one = 1;
  return dnrm2_(&n, const_cast<double*>(&(*this)[0]), &one);
}

/* l1-norm */
NumericalScalar NumericalPoint::norm1() const
{
  int n = getDimension();
  int one = 1;
  return dasum_(&n, const_cast<double*>(&(*this)[0]), &one);
}

/*  Norm^2 */
NumericalScalar NumericalPoint::normSquare() const
{
  return dot(*this, *this);
}


/*  Normalize */
NumericalPoint NumericalPoint::normalize() const
{
  const NumericalScalar theNorm(norm());
  if (theNorm == 0.0) throw InternalException(HERE) << "Error: cannot normalize a null vector";
  return (*this) / theNorm;
}

/*  Square normalize */
NumericalPoint NumericalPoint::normalizeSquare() const
{
  const NumericalScalar theNormSquare(normSquare());
  if (theNormSquare == 0.0) throw InternalException(HERE) << "Error: cannot square normalize a null vector";
  NumericalPoint result(getDimension());
  for (UnsignedInteger i = 0; i < getDimension(); ++i) result[i] = pow((*this)[i], 2) / theNormSquare;
  return result;
}

/* Method save() stores the object through the StorageManager */
void NumericalPoint::save(Advocate & adv) const
{
  PersistentCollection<NumericalScalar>::save(adv);
}


/* Method load() reloads the object from the StorageManager */
void NumericalPoint::load(Advocate & adv)
{
  PersistentCollection<NumericalScalar>::load(adv);
}


END_NAMESPACE_OPENTURNS
