//                                               -*- C++ -*-
/**
 *  @brief SquareMatrix implements the classical mathematical square matrix
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
#include "SquareMatrix.hxx"
#include "SquareComplexMatrix.hxx"
#include "SymmetricMatrix.hxx"
#include "IdentityMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SquareMatrix);

/* Default constructor */
SquareMatrix::SquareMatrix()
  : Matrix(0, 0)
{
  // Nothing to do
}


/* Constructor with implementation */
SquareMatrix::SquareMatrix(const Implementation & i)
  : Matrix(i)
{
  if (getNbRows() != getNbColumns())
    throw InvalidDimensionException(HERE) << "The matrix provided is not square : rows=" << getNbRows() << " columns=" << getNbColumns();
}


/* Constructor with implementation */
SquareMatrix::SquareMatrix(const MatrixImplementation & i)
  : Matrix(i)
{
  if (getNbRows() != getNbColumns())
    throw InvalidDimensionException(HERE) << "The matrix provided is not square : rows=" << getNbRows() << " columns=" << getNbColumns();
}


/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
SquareMatrix::SquareMatrix(const UnsignedInteger dim)
  : Matrix(dim, dim)
{
  // Nothing to do
}

/* Constructor from external collection */
/* If the dimensions of the matrix and of the collection */
/* do not match, either the collection is truncated */
/* or the rest of the matrix is filled with zeros */
SquareMatrix::SquareMatrix(const UnsignedInteger dim,
                           const NumericalScalarCollection & elementsValues)
  : Matrix(dim, dim, elementsValues)
{
  // Nothing to do
}


/* String converter */
String SquareMatrix::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

/* Get the dimension of the matrix */
UnsignedInteger SquareMatrix::getDimension() const
{
  return getImplementation()->getDimension();
}

/* SquareMatrix transpose */
SquareMatrix SquareMatrix::transpose () const
{
  // Quick return for empty or scalar SquareMatrix
  if (getDimension() <= 1) return (*this);
  return Implementation(getImplementation()->transpose().clone());
}

/* Matrix additions (must have the same dimensions) */
SquareMatrix SquareMatrix::operator + (const SquareMatrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

SquareMatrix SquareMatrix::operator + (const SymmetricMatrix & m) const
{
  m.getImplementation()->symmetrize();
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* Matrix substractions (must have the same dimensions) */
SquareMatrix SquareMatrix::operator - (const SquareMatrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

SquareMatrix SquareMatrix::operator - (const SymmetricMatrix & m) const
{
  m.getImplementation()->symmetrize();
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* Matrix multiplications (must have consistent dimensions) */
SquareMatrix SquareMatrix::operator * (const SquareMatrix & m) const
{
  return Implementation((getImplementation()->genProd(*(m.getImplementation())) ).clone());
}

SquareMatrix SquareMatrix::operator * (const SymmetricMatrix & m) const
{
  return Implementation(m.getImplementation()->symProd(*getImplementation(), 'R').clone());
}

SquareMatrix SquareMatrix::operator * (const IdentityMatrix & m) const
{
  return *this;
}

/* Multiplication with a NumericalPoint (must have consistent dimensions) */
NumericalPoint SquareMatrix::operator * (const NumericalPoint & pt) const
{
  return getImplementation()->genVectProd(pt) ;
}

/* Multiplication with a NumericalScalar */
SquareMatrix SquareMatrix::operator * (const NumericalScalar s) const
{
  return Implementation((*getImplementation() * s ).clone());
}

/* Division by a NumericalScalar*/
SquareMatrix SquareMatrix::operator / (const NumericalScalar s) const
{
  return Implementation((*getImplementation() / s ).clone());
}

/* SquareMatrix integer power */
SquareMatrix SquareMatrix::power(const UnsignedInteger n) const
{
  return Implementation(getImplementation()->genPower(n).clone());
}

/* Resolution of a linear system */
NumericalPoint SquareMatrix::solveLinearSystem(const NumericalPoint & b,
    const Bool keepIntact)
{
  return getImplementation()->solveLinearSystemSquare(b, keepIntact);
}

Matrix SquareMatrix::solveLinearSystem(const Matrix & b,
                                       const Bool keepIntact)
{
  return Implementation(getImplementation()->solveLinearSystemSquare(*b.getImplementation(), keepIntact).clone());
}

/* Compute determinant */
NumericalScalar SquareMatrix::computeLogAbsoluteDeterminant(NumericalScalar & sign,
    const Bool keepIntact)
{
  return getImplementation()->computeLogAbsoluteDeterminant(sign, keepIntact);
}

NumericalScalar SquareMatrix::computeDeterminant(const Bool keepIntact)
{
  return getImplementation()->computeDeterminant(keepIntact);
}

/* Compute trace */
NumericalScalar SquareMatrix::computeTrace() const
{
  return getImplementation()->computeTrace();
}

/* Compute eigenvalues */
SquareMatrix::NumericalComplexCollection SquareMatrix::computeEigenValues(const Bool keepIntact)
{
  return getImplementation()->computeEigenValuesSquare(keepIntact);
}

SquareMatrix::NumericalComplexCollection SquareMatrix::computeEV(SquareComplexMatrix & v,
    const Bool keepIntact)
{
  return getImplementation()->computeEVSquare(*(v.getImplementation()), keepIntact);
}

/* Check if the matrix is diagonal */
Bool SquareMatrix::isDiagonal() const
{
  // The loops must be done in this order in order to use the quick accessor
  // of the underlying implementation.
  for (UnsignedInteger j = 0; j < getDimension(); ++j)
  {
    for (UnsignedInteger i = 0; i < j; ++i)
      if ((*getImplementation())(i, j) != 0.0) return false;
    for (UnsignedInteger i = j + 1; i < getDimension(); ++i)
      if ((*getImplementation())(i, j) != 0.0) return false;
  }
  return true;
}

END_NAMESPACE_OPENTURNS
