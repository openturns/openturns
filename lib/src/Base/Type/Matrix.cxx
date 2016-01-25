//                                               -*- C++ -*-
/**
 *  @brief Matrix implements the classical mathematical matrix
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
#include "Matrix.hxx"
#include "MatrixImplementation.hxx"
#include "SymmetricMatrix.hxx"
#include "IdentityMatrix.hxx"
#include "ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(Matrix);

/* Default constructor */
Matrix::Matrix()
  : TypedInterfaceObject<MatrixImplementation>(new MatrixImplementation())
{
  // Nothing to do
}

/* Constructor with size (rowDim and colDim) */
/* The matrix is made up of a collection of rowDim*colDim elements */
/* The matrix is viewed as a set of column vectors read one after another */
Matrix::Matrix(const UnsignedInteger rowDim,
               const UnsignedInteger colDim)
  : TypedInterfaceObject<MatrixImplementation>(new MatrixImplementation(rowDim, colDim))
{
  // Nothing to do
}

/* Constructor from external collection */
/* If the dimensions of the matrix and of the collection */
/* do not match, either the collection is truncated */
/* or the rest of the matrix is filled with zeros */
Matrix::Matrix(const UnsignedInteger rowDim,
               const UnsignedInteger colDim,
               const Collection<NumericalScalar> & elementsValues)
  : TypedInterfaceObject<MatrixImplementation>(new MatrixImplementation(rowDim, colDim, elementsValues))
{
  // Nothing to do
}

/* Constructor with implementation */
Matrix::Matrix(const Implementation & i)
  : TypedInterfaceObject<MatrixImplementation>(i)
{
  // Nothing to do
}

/* Constructor with implementation */
Matrix::Matrix(const MatrixImplementation & i)
  : TypedInterfaceObject<MatrixImplementation>(i.clone())
{
  // Nothing to do
}


/* String converter */
String Matrix::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}


String Matrix::__str__(const String & offset) const
{
  return getImplementation()->__str__(offset);
}

/* Set small elements to zero */
Matrix Matrix::clean(const NumericalScalar threshold) const
{
  return Implementation(getImplementation()->clean(threshold).clone());
}

/* Get the dimensions of the matrix : number of rows */
UnsignedInteger Matrix::getNbRows() const
{
  return getImplementation()->getNbRows();
}

/* Get the dimensions of the matrix : number of columns */
UnsignedInteger Matrix::getNbColumns() const
{
  return getImplementation()->getNbColumns();
}

/* Operator () gives access to the elements of the matrix (to modify these elements) */
/* The element of the matrix is designated by its row number i and its column number j */
/* the first element of the matrix is m(0,0) */
NumericalScalar & Matrix::operator() (const UnsignedInteger i,
                                      const UnsignedInteger j)
{
  copyOnWrite();
  return (*getImplementation())(i, j);
}

/* Operator () gives access to the elements of the matrix (read only) */
/* The element of the matrix is designated by its row number i and its column number j */
const NumericalScalar & Matrix::operator() (const UnsignedInteger i,
    const UnsignedInteger j) const
{
  return (*getImplementation())(i, j);
}


/* Matrix transpose */
Matrix Matrix::transpose () const
{
  return Implementation(getImplementation()->transpose().clone());
}

/* Row extraction */
const Matrix Matrix::getRow(const UnsignedInteger rowIndex) const
{
  return Implementation(getImplementation()->getRow(rowIndex).clone());
}

/* Column extration */
const Matrix Matrix::getColumn(const UnsignedInteger columnIndex) const
{
  return Implementation(getImplementation()->getColumn(columnIndex).clone());
}

/* Matrix additions (must have the same dimensions) */
Matrix Matrix::operator+ (const Matrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation())).clone());
}

Matrix Matrix::operator+ (const SymmetricMatrix & m) const
{
  m.getImplementation()->symmetrize();
  return Implementation((*getImplementation() + * (m.getImplementation())).clone());
}

/* Matrix substractions (must have the same dimensions) */
Matrix Matrix::operator- (const Matrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation())).clone());
}

Matrix Matrix::operator- (const SymmetricMatrix & m) const
{
  m.getImplementation()->symmetrize();
  return Implementation((*getImplementation() - * (m.getImplementation())).clone());
}

/* Matrix multiplications (must have consistent dimensions) */
Matrix Matrix::operator* (const Matrix & m) const
{
  return Implementation(getImplementation()->genProd(*(m.getImplementation())).clone());
}

Matrix Matrix::operator* (const SymmetricMatrix & m) const
{
  return Implementation(m.getImplementation()->symProd(*getImplementation(), 'R').clone());
}

Matrix Matrix::operator* (const IdentityMatrix & m) const
{
  return *this;
}

/* Multiplication with a NumericalPoint (must have consistent dimensions) */
NumericalPoint Matrix::operator* (const NumericalPoint & pt) const
{
  return getImplementation()->genVectProd(pt) ;
}

/* Multiplication with a NumericalScalar */
Matrix Matrix::operator* (const NumericalScalar s) const
{
  return Implementation((*getImplementation() * s).clone());
}

/* Division by a NumericalScalar*/
Matrix Matrix::operator/ (const NumericalScalar s) const
{
  return Implementation((*getImplementation() / s).clone());
}

/* Resolution of a linear system */
NumericalPoint Matrix::solveLinearSystem(const NumericalPoint & b,
    const Bool keepIntact)
{
  return getImplementation()->solveLinearSystemRect(b, keepIntact);
}

Matrix Matrix::solveLinearSystem(const Matrix & b,
                                 const Bool keepIntact)
{
  return Implementation(getImplementation()->solveLinearSystemRect(*(b.getImplementation()), keepIntact).clone());
}

/* Compute singular values */
NumericalPoint Matrix::computeSingularValues(const Bool keepIntact)
{
  return getImplementation()->computeSingularValues(keepIntact);
}

/* Compute singular values */
NumericalPoint Matrix::computeSVD(Matrix & u,
                                  Matrix & vT,
                                  const Bool fullSVD,
                                  const Bool keepIntact)
{
  return getImplementation()->computeSVD(*(u.getImplementation()), *(vT.getImplementation()), fullSVD, keepIntact);
}


/* Build the QR factorization of the matrix */
Matrix Matrix::computeQR(Matrix & R,
                         const Bool fullQR,
                         const Bool keepIntact)
{
  return getImplementation()->computeQR(*(R.getImplementation()), fullQR, keepIntact);
}


/* Empty returns true if there is no element in the matrix */
Bool Matrix::isEmpty() const
{
  return getImplementation()->isEmpty() ;
}

/* Compute the associated Gram matrix */
CovarianceMatrix Matrix::computeGram(const Bool transposed) const
{
  return getImplementation()->computeGram(transposed) ;
}


/* Comparison operator */
Bool Matrix::operator == (const Matrix & rhs) const
{
  const Matrix &lhs(*this);
  return (*(lhs.getImplementation()) == *(rhs.getImplementation()) );
}


const NumericalScalar* Matrix::__baseaddress__() const
{
  return getImplementation()->__baseaddress__();
}


UnsignedInteger Matrix::__elementsize__() const
{
  return getImplementation()->__elementsize__();
}


UnsignedInteger Matrix::__stride__(UnsignedInteger dim) const
{
  return getImplementation()->__stride__(dim);
}

END_NAMESPACE_OPENTURNS
