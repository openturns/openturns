//                                               -*- C++ -*-
/**
 *  @brief TriangularMatrix implements the classical mathematical triangluar matrix
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/TriangularMatrix.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/HermitianMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(TriangularMatrix)

/* Default constructor */
TriangularMatrix::TriangularMatrix()
  : SquareMatrix(0)
  , isLowerTriangular_(true)
  , hasBeenTriangularized_(false)
{
  // Nothing to do
}


/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
TriangularMatrix::TriangularMatrix(const UnsignedInteger dimension,
                                   Bool isLower)
  : SquareMatrix(dimension)
  , isLowerTriangular_(isLower)
  , hasBeenTriangularized_(false)
{
  // Nothing to do
}


/* Constructor with implementation */
TriangularMatrix::TriangularMatrix(const Implementation & i,
                                   Bool isLower)

  : SquareMatrix(i)
  , isLowerTriangular_(isLower)
  , hasBeenTriangularized_(false)
{
  // Nothing to do
}


/* Constructor with matrix implementation */
TriangularMatrix::TriangularMatrix(const MatrixImplementation & i,
                                   Bool isLower)

  : SquareMatrix(i)
  , isLowerTriangular_(isLower)
  , hasBeenTriangularized_(false)
{
  // Nothing to do
}


/* Check if the internal representation is actually symmetric */
void TriangularMatrix::checkTriangularity() const
{
  if (!hasBeenTriangularized_)
  {
    getImplementation()->triangularize(isLowerTriangular_);
    hasBeenTriangularized_ = true;
  }
}

/* Test if the matrix is diagonal */
Bool TriangularMatrix::isDiagonal() const
{
  if (isLowerTriangular_)
  {
    for (UnsignedInteger j = 0; j < getDimension(); ++j)
      for (UnsignedInteger i = j + 1; i < getDimension(); ++i)
        if ((*getImplementation())(i, j) != 0.0) return false;
  }
  else
  {
    for (UnsignedInteger j = 0; j < getDimension(); ++j)
      for (UnsignedInteger i = 0; i < j; ++i)
        if ((*getImplementation())(i, j) != 0.0) return false;
  }
  return true;
}

/* String converter */
String TriangularMatrix::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

String TriangularMatrix::__str__(const String & offset) const
{
  checkTriangularity();
  return SquareMatrix::__str__(offset);
}

/* Get the dimension of the matrix */
UnsignedInteger TriangularMatrix::getDimension() const
{
  return getImplementation()->getDimension();
}

/* TriangularMatrix transpose */
TriangularMatrix TriangularMatrix::transpose () const
{
  // Quick return for empty or scalar TriangularMatrix
  if (getDimension() <= 1) return (*this);
  return TriangularMatrix(getImplementation()->transpose(), !isLowerTriangular_);
}


/* Check if the matrix is lower or upper */
Bool TriangularMatrix::isLowerTriangular() const
{
  return isLowerTriangular_;
}


/* Operator () gives access to the elements of the matrix (to modify these elements) */
/* The element of the matrix is designated by its row number i and its column number j */
/* the first element of the matrix is m(0,0) */
Scalar & TriangularMatrix::operator() (const UnsignedInteger i,
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
const Scalar & TriangularMatrix::operator() (const UnsignedInteger i,
    const UnsignedInteger j)  const
{
  return (*getImplementation())(i, j) ;
}


/* TriangularMatrix additions */
SquareMatrix TriangularMatrix::operator+ (const TriangularMatrix & m) const
{
  return *getImplementation() + *m.getImplementation();
}

/* Matrix additions */
SquareMatrix TriangularMatrix::operator+ (const SquareMatrix & m) const
{
  return *getImplementation() + *m.getImplementation();
}

/* Matrix subtractions */
SquareMatrix TriangularMatrix::operator- (const SquareMatrix & m) const
{
  return *getImplementation() - *m.getImplementation();
}

/* TriangularMatrix subtractions */
SquareMatrix TriangularMatrix::operator- (const TriangularMatrix & m) const
{
  return *getImplementation() - *m.getImplementation();
}

/* Matrix multiplications */
SquareMatrix TriangularMatrix::operator * (const SquareMatrix & m) const
{
  char uplo(isLowerTriangular() ? 'L' : 'U');
  return getImplementation()->triangularProd(*m.getImplementation(), 'L',  uplo );
}

/* Matrix multiplications */
Matrix TriangularMatrix::operator * (const Matrix & m) const
{
  char uplo(isLowerTriangular() ? 'L' : 'U');
  return getImplementation()->triangularProd(*m.getImplementation(), 'L',  uplo );
}

/* TriangularMatrix multiplications */
SquareMatrix TriangularMatrix::operator * (const TriangularMatrix & m) const
{
  char uplo(isLowerTriangular() ? 'L' : 'U');
  return getImplementation()->triangularProd(*m.getImplementation(), 'L', uplo );
}

/* SymmetricMatrix multiplications */
SquareMatrix TriangularMatrix::operator * (const SymmetricMatrix & m) const
{
  char uplo(isLowerTriangular() ? 'L' : 'U');
  return getImplementation()->triangularProd(*m.getImplementation(), 'L', uplo );
}

/*  IdentityMatrix multiplications */
TriangularMatrix TriangularMatrix::operator * (const IdentityMatrix & ) const
{
  return (*this);
}

/* Multiplication with a ScalarCollection (must have consistent dimensions) */
TriangularMatrix::ScalarCollection TriangularMatrix::operator * (const ScalarCollection & pt) const
{
  char uplo(isLowerTriangular() ? 'L' : 'U');
  return getImplementation()->triangularVectProd(pt, uplo) ;
}

/* Multiplication with a Point (must have consistent dimensions) */
Point TriangularMatrix::operator * (const Point & pt) const
{
  char uplo(isLowerTriangular() ? 'L' : 'U');
  return getImplementation()->triangularVectProd(pt, uplo) ;
}

/* Multiplication with a Numerical */
TriangularMatrix TriangularMatrix::operator* (const Scalar s) const
{
  return TriangularMatrix(*getImplementation() * s, isLowerTriangular_);
}

/* Division by a Numerical*/
TriangularMatrix TriangularMatrix::operator / (const Scalar s) const
{
  return TriangularMatrix(*getImplementation() / s, isLowerTriangular_);
}


/* Resolution of a linear system */
Point TriangularMatrix::solveLinearSystem (const Point & b,
    const Bool keepIntact)
{
  return getImplementation()->solveLinearSystemTri(b, keepIntact, isLowerTriangular_);
}

Matrix TriangularMatrix::solveLinearSystem (const Matrix & b,
    const Bool keepIntact)
{
  return getImplementation()->solveLinearSystemTri(*b.getImplementation(), keepIntact, isLowerTriangular_);
}

END_NAMESPACE_OPENTURNS
