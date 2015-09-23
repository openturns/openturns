//                                               -*- C++ -*-
/**
 *  @brief ARMACoefficients class enables to stock coefficients of an ARMA process
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#include "PersistentObjectFactory.hxx"
#include "ARMACoefficients.hxx"
#include "Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

TEMPLATE_CLASSNAMEINIT(PersistentCollection< ARMACoefficients >);
TEMPLATE_CLASSNAMEINIT(PersistentCollection< SquareMatrix >);
static const Factory<PersistentCollection<SquareMatrix> > RegisteredFactory1;

CLASSNAMEINIT(ARMACoefficients);
static const Factory<ARMACoefficients> RegisteredFactory;

/* Default constructor */
ARMACoefficients::ARMACoefficients(const UnsignedInteger & size,
                                   const UnsignedInteger & dimension)
  : PersistentCollection<SquareMatrix>(size, SquareMatrix(dimension) )
  , dimension_(dimension)
{
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: dimension should be at least 1";
}


/* Standard constructor */
ARMACoefficients::ARMACoefficients(const SquareMatrixCollection & collection)
  : PersistentCollection<SquareMatrix>(0)
{
  // Adding elements one by one and checking coherance of dimension
  const UnsignedInteger collectionSize(collection.getSize());
  if (collectionSize == 0) throw InvalidArgumentException(HERE) << "Error: cannot build an ARMACoefficients object based on an empty collection of matrices.";
  dimension_ = collection[0].getDimension();
  add(collection[0]);
  for (UnsignedInteger i = 1; i < collectionSize; ++i)
    if (collection[i].getDimension() == dimension_)
      add(collection[i]);
}

/* Default constructor */
ARMACoefficients::ARMACoefficients(const NumericalPoint & scalarCoefficients)
  : PersistentCollection<SquareMatrix>(scalarCoefficients.getSize(), SquareMatrix(1))
{
  dimension_ = 1;
  for (UnsignedInteger i = 0 ; i < getSize() ; ++i ) (*this)[i](0, 0) = scalarCoefficients[i];
}

/* constructor using polynomial */
ARMACoefficients::ARMACoefficients(const UniVariatePolynomial & polynomial)
  : PersistentCollection<SquareMatrix>(polynomial.getDegree() + 1, SquareMatrix(1))
{
  dimension_ = 1;
  const NumericalPoint coefficients(polynomial.getCoefficients());
  for (UnsignedInteger i = 0 ; i < getSize() ; ++i ) (*this)[i](0, 0) = coefficients[i];
}

/* Virtual constructor  - clone*/
ARMACoefficients * ARMACoefficients::clone() const
{
  return new ARMACoefficients(*this);
}

/* String converter */
String ARMACoefficients::__repr__() const
{
  OSS oss(true);
  oss << "class=" << ARMACoefficients::GetClassName();
  for (UnsignedInteger i = 0; i < getSize(); ++i) oss << ", shift=" << i << ", value=" << (*this)[i];
  return oss;
}

String ARMACoefficients::__str__(const String & offset) const
{
  OSS oss(false);
  for (UnsignedInteger i = 0; i < getSize(); ++i) oss << offset << "shift = " << i << "\n" << offset << (*this)[i].__str__(offset) << "\n";
  return oss;
}

/* Dimension accessor */
UnsignedInteger  ARMACoefficients::getDimension() const
{
  return dimension_;
}

/** Redefinition of add method : control of SquareMatrix sizes */
void ARMACoefficients::add(const SquareMatrix & matrix)
{
  if (matrix.getDimension() != dimension_)
    throw InvalidArgumentException(HERE) << "Could not add the coefficient. Incompatible dimension with the elements of collection";
  PersistentCollection<SquareMatrix>::add(matrix);
}

void ARMACoefficients::add(const NumericalScalar scalar)
{
  if (dimension_ != 1)
    throw InvalidArgumentException(HERE) << "Could not add the coefficient. The dimension is greater than 1.";
  PersistentCollection<SquareMatrix>::add(SquareMatrix(1, NumericalPoint(1, scalar)));
}


/* Method save() stores the object through the StorageManager */
void ARMACoefficients::save(Advocate & adv) const
{
  PersistentCollection<SquareMatrix>::save( adv );
}

/* Method load() reloads the object from the StorageManager */
void ARMACoefficients::load(Advocate & adv)
{
  PersistentCollection<SquareMatrix>::load( adv );
}

END_NAMESPACE_OPENTURNS

