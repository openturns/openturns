//                                               -*- C++ -*-
/**
 *  @brief TriangularComplexMatrix implements the classical mathematical triangluar matrix
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
#include "openturns/TriangularComplexMatrix.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/HermitianMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TriangularComplexMatrix)

/* Default constructor */
TriangularComplexMatrix::TriangularComplexMatrix()
  : SquareComplexMatrix(0)
  , isLowerTriangular_(true)
{
  // Nothing to do
}


/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
TriangularComplexMatrix::TriangularComplexMatrix(const UnsignedInteger dimension,
    Bool isLower)
  : SquareComplexMatrix(dimension)
  , isLowerTriangular_(isLower)
{
  // Nothing to do
}


/* Constructor with implementation */
TriangularComplexMatrix::TriangularComplexMatrix(const Implementation & i,
    Bool isLower)
  : SquareComplexMatrix(i)
  , isLowerTriangular_(isLower)
{
  // Nothing to do
}


/* String converter */
String TriangularComplexMatrix::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

String TriangularComplexMatrix::__str__(const String & offset) const
{
  return SquareComplexMatrix::__str__(offset);
}

/* Get the dimension of the matrix */
UnsignedInteger TriangularComplexMatrix::getDimension() const
{
  return getImplementation()->getDimension();
}

/* TriangularComplexMatrix transpose */
TriangularComplexMatrix TriangularComplexMatrix::transpose () const
{
  // Quick return for empty or scalar TriangularComplexMatrix
  if (getDimension() <= 1) return (*this);
  return TriangularComplexMatrix(Implementation(getImplementation()->transpose().clone()), !isLowerTriangular_);
}

/* TriangularComplexMatrix conjugate */
TriangularComplexMatrix TriangularComplexMatrix::conjugate () const
{
  // Quick return for empty or scalar TriangularComplexMatrix
  if (getDimension() <= 1) return (*this);
  return TriangularComplexMatrix(Implementation(getImplementation()->conjugate().clone()), isLowerTriangular_);
}

/* TriangularComplexMatrix conjugate */
TriangularComplexMatrix TriangularComplexMatrix::conjugateTranspose () const
{
  // Quick return for empty or scalar TriangularComplexMatrix
  if (getDimension() <= 1) return (*this);
  return TriangularComplexMatrix(Implementation(getImplementation()->conjugateTranspose().clone()), !isLowerTriangular_);
}

/* Check if the matrix is lower or upper */
Bool TriangularComplexMatrix::isLowerTriangular() const
{
  return isLowerTriangular_;
}


/* Operator () gives access to the elements of the matrix (to modify these elements) */
/* The element of the matrix is designated by its row number i and its column number j */
/* the first element of the matrix is m(0,0) */
Complex & TriangularComplexMatrix::operator() (const UnsignedInteger i,
    const UnsignedInteger j)
{
  if (isLowerTriangular() && (i < j))
    throw InvalidArgumentException(HERE) << "Error; The triangular matrix is lower. "
                                         << "The indices are not valid" ;

  if (!isLowerTriangular() && (i > j))
    throw InvalidArgumentException(HERE) << "Error; The triangular matrix is upper. "
                                         << "The indices are not valid" ;

  copyOnWrite();
  return  (*getImplementation())(i, j) ;
}

/* Operator () gives access to the elements of the matrix (read only) */
/* The element of the matrix is designated by its row number i and its column number j */
const Complex & TriangularComplexMatrix::operator() (const UnsignedInteger i,
    const UnsignedInteger j)  const
{
  return (*getImplementation())(i, j) ;
}


/* TriangularComplexMatrix additions */
SquareComplexMatrix TriangularComplexMatrix::operator+ (const TriangularComplexMatrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* ComplexMatrix additions */
SquareComplexMatrix TriangularComplexMatrix::operator+ (const SquareComplexMatrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* ComplexMatrix subtractions */
SquareComplexMatrix TriangularComplexMatrix::operator- (const SquareComplexMatrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* TriangularComplexMatrix subtractions */
SquareComplexMatrix TriangularComplexMatrix::operator- (const TriangularComplexMatrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* ComplexMatrix multiplications */
SquareComplexMatrix TriangularComplexMatrix::operator * (const SquareComplexMatrix & m) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return Implementation((getImplementation()->triangularProd(*(m.getImplementation()), 'L',  uplo ) ).clone());
}

/* ComplexMatrix multiplications */
ComplexMatrix TriangularComplexMatrix::operator * (const ComplexMatrix & m) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return Implementation((getImplementation()->triangularProd(*(m.getImplementation()), 'L',  uplo ) ).clone());
}

/* HermitianMatrix multiplications */
SquareComplexMatrix TriangularComplexMatrix::operator * (const HermitianMatrix & m) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return Implementation((getImplementation()->triangularProd(*(m.getImplementation()), 'L', uplo ) ).clone());
}

/* TriangularComplexMatrix multiplications */
SquareComplexMatrix TriangularComplexMatrix::operator * (const TriangularComplexMatrix & m) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return Implementation((getImplementation()->triangularProd(*(m.getImplementation()), 'L', uplo ) ).clone());
}

/* Real SquareMatrix multiplications */
SquareComplexMatrix  TriangularComplexMatrix::operator * (const SquareMatrix & m) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return Implementation((getImplementation()->triangularProd(*(m.getImplementation()), 'L', uplo ) ).clone());
}

/* Matrix multiplications */
ComplexMatrix  TriangularComplexMatrix::operator * (const Matrix & m) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return Implementation((getImplementation()->triangularProd(*(m.getImplementation()), 'L', uplo ) ).clone());
}

/* SymmetricMatrix multiplications */
SquareComplexMatrix TriangularComplexMatrix::operator * (const SymmetricMatrix & m) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return Implementation((getImplementation()->triangularProd(*(m.getImplementation()), 'L', uplo ) ).clone());
}

/*  IdentityMatrix multiplications */
TriangularComplexMatrix TriangularComplexMatrix::operator * (const IdentityMatrix & ) const
{
  return (*this);
}

/* Multiplication with a ComplexCollection (must have consistent dimensions) */
TriangularComplexMatrix::ComplexCollection TriangularComplexMatrix::operator * (const ComplexCollection & pt) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return getImplementation()->triangularVectProd(pt, uplo) ;
}

/* Multiplication with a ScalarCollection (must have consistent dimensions) */
TriangularComplexMatrix::ComplexCollection TriangularComplexMatrix::operator * (const ScalarCollection & pt) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return getImplementation()->triangularVectProd(pt, uplo) ;
}

/* Multiplication with a Point (must have consistent dimensions) */
TriangularComplexMatrix::ComplexCollection TriangularComplexMatrix::operator * (const Point & pt) const
{
  char uplo('L');
  if (!isLowerTriangular()) uplo = 'R';
  return getImplementation()->triangularVectProd(pt, uplo) ;
}

/* Multiplication with a Complex */
TriangularComplexMatrix TriangularComplexMatrix::operator* (const Complex s) const
{
  return TriangularComplexMatrix(Implementation((*getImplementation() * s ).clone()), isLowerTriangular_);
}

/* Division by a Complex*/
TriangularComplexMatrix TriangularComplexMatrix::operator / (const Complex s) const
{
  return TriangularComplexMatrix(Implementation((*getImplementation() / s ).clone()), isLowerTriangular_);
}

END_NAMESPACE_OPENTURNS
