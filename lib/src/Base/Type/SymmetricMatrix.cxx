//                                               -*- C++ -*-
/**
 *  @brief SymmetricMatrix implements the classical mathematical symmetric matrix
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/SymmetricMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SymmetricMatrix)

/* Default constructor */
SymmetricMatrix::SymmetricMatrix()
  : SquareMatrix(0)
  , hasBeenSymmetrized_(false)
{
  // Nothing to do
}


/* Constructor with implementation */
SymmetricMatrix::SymmetricMatrix(const Implementation & i)
  : SquareMatrix(i)
  , hasBeenSymmetrized_(false)
{
  // Nothing to do
}


/* Constructor with implementation */
SymmetricMatrix::SymmetricMatrix(const MatrixImplementation & i)
  : SquareMatrix(i)
  , hasBeenSymmetrized_(false)
{
  // Nothing to do
}

/* Copy constructor, added to solve glitches with inheritance */
SymmetricMatrix::SymmetricMatrix(const SymmetricMatrix & other)
  : SquareMatrix(static_cast<const SquareMatrix &>(other))
  , hasBeenSymmetrized_(false)
{
  // Nothing to do
}

/* Assignment operator */
SymmetricMatrix & SymmetricMatrix::operator=(const SymmetricMatrix & rhs)
{
  if (this != &rhs)
  {
    SquareMatrix::operator=(rhs);
    hasBeenSymmetrized_ = rhs.hasBeenSymmetrized_;
  }
  return *this;
}

/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
SymmetricMatrix::SymmetricMatrix(const UnsignedInteger dim)
  : SquareMatrix(dim)
  , hasBeenSymmetrized_(false)
{
  // Nothing to do
}

/* Constructor from external collection */
/* If the dimensions of the matrix and of the collection */
/* do not match, either the collection is truncated */
/* or the rest of the matrix is filled with zeros */
SymmetricMatrix::SymmetricMatrix(const UnsignedInteger dim,
                                 const Collection<Scalar> & elementsValues)
  : SquareMatrix(dim, elementsValues)
  , hasBeenSymmetrized_(false)
{
  // Nothing to do
}


/* Check if the internal representation is actually symmetric */
void SymmetricMatrix::checkSymmetry() const
{
  if (!hasBeenSymmetrized_)
  {
    getImplementation()->symmetrize();
    hasBeenSymmetrized_ = true;
  }
}

/* Test if the matrix is diagonal */
Bool SymmetricMatrix::isDiagonal() const
{
  // The loops must be done in this order in order to use the quick accessor
  // of the underlying implementation without symmetrization of the matrix.
  // We know that the storage is made column-wise, using the upper triangle
  for (UnsignedInteger j = 0; j < getDimension(); ++j)
    for (UnsignedInteger i = j + 1; i < getDimension(); ++i)
      if ((*getImplementation())(i, j) != 0.0) return false;
  return true;
}

/* Row extraction */
const Matrix SymmetricMatrix::getRow(const UnsignedInteger rowIndex) const
{
  return Implementation(getImplementation()->getRowSym(rowIndex).clone());
}

/* Column extration */
const Matrix SymmetricMatrix::getColumn(const UnsignedInteger columnIndex) const
{
  return Implementation(getImplementation()->getColumnSym(columnIndex).clone());
}

/* String converter */
String SymmetricMatrix::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

String SymmetricMatrix::__str__(const String & offset) const
{
  checkSymmetry();
  return SquareMatrix::clean(0.0).__str__(offset);
}

/* Operator () gives access to the elements of the matrix (to modify these elements) */
/* The element of the matrix is designated by its row number i and its column number j */
/* the first element of the matrix is m(0,0) */
Scalar & SymmetricMatrix::operator() (const UnsignedInteger i,
                                      const UnsignedInteger j)
{
  copyOnWrite();
  hasBeenSymmetrized_ = false;

  return (i > j) ? (*getImplementation())(i, j) : (*getImplementation())(j, i) ;
}

/* Operator () gives access to the elements of the matrix (read only) */
/* The element of the matrix is designated by its row number i and its column number j */
const Scalar & SymmetricMatrix::operator() (const UnsignedInteger i,
    const UnsignedInteger j)  const
{
  return (i > j) ? (*getImplementation())(i, j) : (*getImplementation())(j, i) ;
}

/* SymmetricMatrix transpose */
SymmetricMatrix SymmetricMatrix::transpose () const
{
  return *this;
}

/* SymmetricMatrix additions (must have the same dimensions) */
Matrix SymmetricMatrix::operator + (const Matrix & m) const
{
  checkSymmetry();
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

SquareMatrix SymmetricMatrix::operator + (const SquareMatrix & m) const
{
  checkSymmetry();
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

SymmetricMatrix SymmetricMatrix::operator + (const SymmetricMatrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* SymmetricMatrix subtractions (must have the same dimensions) */
Matrix SymmetricMatrix::operator - (const Matrix & m) const
{
  checkSymmetry();
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

SquareMatrix SymmetricMatrix::operator - (const SquareMatrix & m) const
{
  checkSymmetry();
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

SymmetricMatrix SymmetricMatrix::operator - (const SymmetricMatrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* SymmetricMatrix multiplications (must have consistent dimensions) */
Matrix SymmetricMatrix::operator * (const Matrix & m) const
{
  return Implementation((getImplementation()->symProd(*(m.getImplementation()), 'L') ).clone());
}

SquareMatrix SymmetricMatrix::operator * (const SquareMatrix & m) const
{
  return Implementation((getImplementation()->symProd(*(m.getImplementation()), 'L') ).clone());
}

SquareMatrix SymmetricMatrix::operator * (const SymmetricMatrix & m) const
{
  // Must check symmetry of the right-hand factor as it is seen as a square matrix
  m.checkSymmetry();
  return Implementation((getImplementation()->symProd(*(m.getImplementation()), 'L') ).clone());
}

SymmetricMatrix SymmetricMatrix::operator * (const IdentityMatrix & ) const
{
  return *this;
}


/* Multiplication with a Point (must have consistent dimensions) */
Point SymmetricMatrix::operator * (const Point & pt) const
{
  return getImplementation()->symVectProd(pt) ;
}


/* Multiplication with a Scalar */
SymmetricMatrix SymmetricMatrix::operator * (const Scalar & s) const
{
  return Implementation((*getImplementation() * s ).clone());
}

/* Division by a Scalar*/
SymmetricMatrix SymmetricMatrix::operator / (const Scalar & s) const
{
  return Implementation((*getImplementation() / s ).clone());
}

/* SquareMatrix integer power */
SymmetricMatrix SymmetricMatrix::power(const UnsignedInteger n) const
{
  return Implementation((getImplementation()->symPower(n)).clone());
}

/* Resolution of a linear system */
Point SymmetricMatrix::solveLinearSystem(const Point & b,
    const Bool keepIntact)
{
  return getImplementation()->solveLinearSystemSym(b, keepIntact);
}

Matrix SymmetricMatrix::solveLinearSystem(const Matrix & b,
    const Bool keepIntact)
{
  return getImplementation()->solveLinearSystemSym(*b.getImplementation(), keepIntact);
}

/* Compute determinant */
Scalar SymmetricMatrix::computeLogAbsoluteDeterminant(Scalar & sign,
    const Bool keepIntact)
{
  return getImplementation()->computeLogAbsoluteDeterminantSym(sign, keepIntact);
}

Scalar SymmetricMatrix::computeDeterminant(const Bool keepIntact)
{
  return getImplementation()->computeDeterminantSym(keepIntact);
}

/* Compute eigenvalues */
Point SymmetricMatrix::computeEigenValues(const Bool keepIntact)
{
  return getImplementation()->computeEigenValuesSym(keepIntact);
}
Point SymmetricMatrix::computeEV(SquareMatrix & v,
                                 const Bool keepIntact)
{
  return getImplementation()->computeEVSym(*(v.getImplementation()), keepIntact);
}

/* Compute the largest eigenvalue module using power iterations, symmetric matrix */
Scalar SymmetricMatrix::computeLargestEigenValueModule(const UnsignedInteger maximumIterations,
    const Scalar epsilon) const
{
  Scalar maximumModule = 0.0;
  const Bool found = getImplementation()->computeLargestEigenValueModuleSym(maximumModule, maximumIterations, epsilon);
  if (!found) throw InternalException(HERE) << "Could not reached a precision=" << epsilon << " using " << maximumIterations << " iterations. The approximation obtained is " << maximumModule;
  return maximumModule;
}

/* Comparison operator */
Bool SymmetricMatrix::operator == (const Matrix & rhs) const
{
  // Compare the references
  if (this == &rhs) return true;
  // Compare the dimensions
  const UnsignedInteger dimension = getDimension();
  if (!(dimension == rhs.getNbRows() && dimension == rhs.getNbColumns())) return false;
  // Compare the content
  for (UnsignedInteger i = 0; i < dimension; ++i)
    for (UnsignedInteger j = 0; j <= i; ++j)
      if ((*this)(i, j) != rhs(i, j)) return false;
  return true;
}

END_NAMESPACE_OPENTURNS
