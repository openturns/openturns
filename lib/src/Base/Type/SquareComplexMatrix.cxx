//                                               -*- C++ -*-
/**
 *  @brief SquareComplexMatrix implements the classical mathematical square matrix
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
#include "openturns/SquareComplexMatrix.hxx"
#include "openturns/HermitianMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SquareComplexMatrix)

/* Default constructor */
SquareComplexMatrix::SquareComplexMatrix()
  : ComplexMatrix(0, 0)
{
  // Nothing to do
}


/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
SquareComplexMatrix::SquareComplexMatrix(const UnsignedInteger dimension)
  : ComplexMatrix(dimension, dimension)
{
  // Nothing to do
}

/* Constructor from external collection */
/* If the dimensions of the matrix and of the collection */
/* do not match, either the collection is truncated */
/* or the rest of the matrix is filled with zeros */
SquareComplexMatrix::SquareComplexMatrix(const UnsignedInteger dimension,
    const ComplexCollection & elementsValues)
  : ComplexMatrix(dimension, dimension, elementsValues)
{
  // Nothing to do
}


/* Constructor with implementation */
SquareComplexMatrix::SquareComplexMatrix(const Implementation & i)
  : ComplexMatrix(i)
{
  // Nothing to do
}


/* Constructor from hermitian matrix */
SquareComplexMatrix::SquareComplexMatrix(const HermitianMatrix & hermitian)
  : ComplexMatrix(hermitian)
{
  // Nothing to do
}


/* String converter */
String SquareComplexMatrix::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

/* Get the dimension of the matrix */
UnsignedInteger SquareComplexMatrix::getDimension() const
{
  return getImplementation()->getDimension();
}

/* SquareComplexMatrix transpose */
SquareComplexMatrix SquareComplexMatrix::transpose () const
{
  // Quick return for empty or scalar SquareComplexMatrix
  if (getDimension() <= 1) return (*this);
  return Implementation(getImplementation()->transpose().clone());
}

SquareComplexMatrix SquareComplexMatrix::conjugate () const
{
  // Quick return for empty or scalar SquareComplexMatrix
  if (getDimension() <= 1) return (*this);
  return Implementation(getImplementation()->conjugate().clone());
}

SquareComplexMatrix SquareComplexMatrix::conjugateTranspose () const
{
  // Quick return for empty or scalar SquareComplexMatrix
  if (getDimension() <= 1) return (*this);
  return Implementation(getImplementation()->conjugateTranspose().clone());
}

/* Matrix additions (must have the same dimensions) */
SquareComplexMatrix SquareComplexMatrix::operator+ (const SquareComplexMatrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

SquareComplexMatrix SquareComplexMatrix::operator+ (const HermitianMatrix & m) const
{
  m.getImplementation()->hermitianize();
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* Matrix subtractions (must have the same dimensions) */
SquareComplexMatrix SquareComplexMatrix::operator- (const SquareComplexMatrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

SquareComplexMatrix SquareComplexMatrix::operator- (const HermitianMatrix & m) const
{
  m.getImplementation()->hermitianize();
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* Matrix multiplications (must have consistent dimensions) */
SquareComplexMatrix SquareComplexMatrix::operator* (const SquareComplexMatrix & m) const
{
  return Implementation((getImplementation()->genProd(*(m.getImplementation())) ).clone());
}

ComplexMatrix SquareComplexMatrix::operator* (const ComplexMatrix & m) const
{
  return Implementation((getImplementation()->genProd(*(m.getImplementation())) ).clone());

}
SquareComplexMatrix SquareComplexMatrix::operator* (const HermitianMatrix & m) const
{
  return Implementation(getImplementation()->hermProd(*(m.getImplementation()), 'R').conjugateTranspose().clone());
}

SquareComplexMatrix SquareComplexMatrix::operator* (const IdentityMatrix & ) const
{
  return *this;
}


/* Multiplication with a Point (must have consistent dimensions) */
SquareComplexMatrix::ComplexCollection SquareComplexMatrix::operator * (const Point & pt) const
{
  return getImplementation()->genVectProd(pt) ;
}

/* Multiplication with a Point (must have consistent dimensions) */
SquareComplexMatrix::ComplexCollection SquareComplexMatrix::operator * (const ComplexCollection & pt) const
{
  return getImplementation()->genVectProd(pt) ;
}

SquareComplexMatrix::ComplexCollection SquareComplexMatrix::operator * (const ScalarCollection & pt) const
{
  return getImplementation()->genVectProd(pt) ;
}



/* Multiplication with a Complex */
SquareComplexMatrix SquareComplexMatrix::operator * (const Complex s) const
{
  return Implementation((*getImplementation() * s ).clone());
}

/* Division by a Complex*/
SquareComplexMatrix SquareComplexMatrix::operator / (const Complex s) const
{
  return Implementation((*getImplementation() / s ).clone());
}

/* SquareComplexMatrix integer power */
SquareComplexMatrix SquareComplexMatrix::power(const UnsignedInteger n) const
{
  return Implementation(getImplementation()->genPower(n).clone());
}


END_NAMESPACE_OPENTURNS
