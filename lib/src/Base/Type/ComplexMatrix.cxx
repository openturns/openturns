//                                               -*- C++ -*-
/**
 *  @brief ComplexMatrix implements the classical mathematical matrix
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

#include "openturns/ComplexMatrix.hxx"
#include "openturns/ComplexMatrixImplementation.hxx"
#include "openturns/PersistentObject.hxx"
#include "openturns/HermitianMatrix.hxx"
#include "openturns/TriangularComplexMatrix.hxx"
#include "openturns/Os.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComplexMatrix)

/* Default constructor */
ComplexMatrix::ComplexMatrix()
  : TypedInterfaceObject<ComplexMatrixImplementation>(new ComplexMatrixImplementation())
{
  // Nothing to do
}

/* Constructor with size (rowDim and colDim) */
/* The matrix is made up of a collection of rowDim*colDim elements */
/* The matrix is viewed as a set of column vectors read one after another */
ComplexMatrix::ComplexMatrix(const UnsignedInteger rowDim,
                             const UnsignedInteger colDim)
  : TypedInterfaceObject<ComplexMatrixImplementation>(new ComplexMatrixImplementation(rowDim, colDim))
{
  // Nothing to do
}

/* Constructor from external collection */
/* If the dimensions of the matrix and of the collection */
/* do not match, either the collection is truncated */
/* or the rest of the matrix is filled with zeros */
ComplexMatrix::ComplexMatrix(const UnsignedInteger rowDim,
                             const UnsignedInteger colDim,
                             const ComplexCollection & elementsValues)
  : TypedInterfaceObject<ComplexMatrixImplementation>(new ComplexMatrixImplementation(rowDim, colDim, elementsValues))
{
  // Nothing to do
}

/* Constructor from external collection */
/* If the dimensions of the matrix and of the collection */
/* do not match, either the collection is truncated */
/* or the rest of the matrix is filled with zeros */
ComplexMatrix::ComplexMatrix(const UnsignedInteger rowDim,
                             const UnsignedInteger colDim,
                             const ScalarCollection & elementsValues)
  : TypedInterfaceObject<ComplexMatrixImplementation>(new ComplexMatrixImplementation(rowDim, colDim, elementsValues))
{
  // Nothing to do
}

/* Constructor with implementation */
ComplexMatrix::ComplexMatrix(const Implementation & i)
  : TypedInterfaceObject<ComplexMatrixImplementation>(i)
{
  // Nothing to do
}

/* Constructor with implementation */
ComplexMatrix::ComplexMatrix(const ComplexMatrixImplementation & i)
  : TypedInterfaceObject<ComplexMatrixImplementation>(i.clone())
{
  // Nothing to do
}

/* Constructor from hermitian matrix */
ComplexMatrix::ComplexMatrix(const HermitianMatrix & hermitian)
  : TypedInterfaceObject<ComplexMatrixImplementation>(hermitian.getImplementation())
{
  getImplementation()->hermitianize();
}


/* Resolution of a linear system */
ComplexMatrix::ComplexCollection ComplexMatrix::solveLinearSystem(const ComplexCollection & b,
    const Bool keepIntact)
{
  return getImplementation()->solveLinearSystemRect(b, keepIntact);
}

ComplexMatrix ComplexMatrix::solveLinearSystem(const ComplexMatrix & b,
    const Bool keepIntact)
{
  return Implementation(getImplementation()->solveLinearSystemRect(*(b.getImplementation()), keepIntact).clone());
}

/* Set small elements to zero */
ComplexMatrix ComplexMatrix::clean(const Scalar threshold) const
{
  return Implementation(getImplementation()->clean(threshold).clone());
}

/* String converter */
String ComplexMatrix::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " implementation=" << getImplementation()->__repr__();
}


String ComplexMatrix::__str__(const String & offset) const
{
  OSS oss(false);
  const UnsignedInteger rows = getNbRows();
  const UnsignedInteger cols = getNbColumns();
  if ( (rows >= ResourceMap::GetAsUnsignedInteger("Matrix-size-visible-in-str-from")) ||
       (cols >= ResourceMap::GetAsUnsignedInteger("Matrix-size-visible-in-str-from")) )
    oss << rows << "x" << cols << Os::GetEndOfLine();

  size_t lwidth = 0;
  size_t rwidth = 0;
  for( UnsignedInteger i = 0; i < rows; ++i )
    for( UnsignedInteger j = 0; j < cols; ++j )
    {
      String st = OSS() << (*this)(i, j);
      size_t dotpos = st.find( '.' );
      lwidth = std::max( lwidth, (dotpos != String::npos) ? dotpos             : st.size() );
      rwidth = std::max( rwidth, (dotpos != String::npos) ? st.size() - dotpos : 0         );
    }

  const char * bracket = "[";
  const char * newline = "";
  for( UnsignedInteger i = 0; i < rows; ++i, newline = Os::GetEndOfLine(), bracket = " " )
  {
    oss << newline << offset << bracket << "[ ";
    const char * sep = "";
    for( UnsignedInteger j = 0; j < cols; ++j, sep = " " )
    {
      String st = OSS() << (*this)(i, j);
      size_t dotpos = st.find( '.' );
      oss << sep << String( lwidth - ((dotpos != String::npos) ? dotpos : st.size()), ' ' )
          << st
          << String( rwidth - ((dotpos != String::npos) ? st.size() - dotpos : 0), ' ' );
    }
    oss << " ]";
  }
  oss << "]";
  return oss;
}


/* Get the dimensions of the matrix : number of rows */
UnsignedInteger ComplexMatrix::getNbRows() const
{
  return getImplementation()->getNbRows();
}

/* Get the dimensions of the matrix : number of columns */
UnsignedInteger ComplexMatrix::getNbColumns() const
{
  return getImplementation()->getNbColumns();
}

/* Operator () gives access to the elements of the matrix (to modify these elements) */
/* The element of the matrix is designated by its row number i and its column number j */
/* the first element of the matrix is m(0,0) */
Complex & ComplexMatrix::operator() (const UnsignedInteger i,
                                     const UnsignedInteger j)
{
  copyOnWrite();
  return (*getImplementation())(i, j);
}

/* Operator () gives access to the elements of the matrix (read only) */
/* The element of the matrix is designated by its row number i and its column number j */
const Complex & ComplexMatrix::operator() (const UnsignedInteger i,
    const UnsignedInteger j)  const
{
  return (*getImplementation())(i, j);
}

/* ComplexMatrix transpose */
ComplexMatrix ComplexMatrix::transpose () const
{
  return Implementation(getImplementation()->transpose().clone());
}


/* ComplexMatrix conjugate */
ComplexMatrix ComplexMatrix::conjugate () const
{
  return Implementation(getImplementation()->conjugate().clone());
}

/* ComplexMatrix conjugate and transpose */
ComplexMatrix ComplexMatrix::conjugateTranspose () const
{
  return Implementation(getImplementation()->conjugateTranspose().clone());
}

/* Get the real part of the matrix */
Matrix ComplexMatrix::real() const
{
  return Matrix::Implementation(getImplementation()->realRect().clone());
}

/* Get the imaginary part of the matrix */
Matrix ComplexMatrix::imag() const
{
  return Matrix::Implementation(getImplementation()->imagRect().clone());
}

/* ComplexMatrix additions (must have the same dimensions) */
ComplexMatrix ComplexMatrix::operator+ (const ComplexMatrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* ComplexMatrix additions */
ComplexMatrix ComplexMatrix::operator+ (const Matrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* ComplexMatrix subtractions (must have the same dimensions) */
ComplexMatrix ComplexMatrix::operator- (const ComplexMatrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* ComplexMatrix subtractions */
ComplexMatrix ComplexMatrix::operator- (const Matrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* ComplexMatrix multiplications (must have consistent dimensions) */
ComplexMatrix ComplexMatrix::operator* (const ComplexMatrix & m) const
{
  return Implementation((getImplementation()->genProd(*(m.getImplementation())) ).clone());
}

/* ComplexMatrix multiplications (must have consistent dimensions) */
ComplexMatrix ComplexMatrix::operator* (const HermitianMatrix & m) const
{
  return Implementation((getImplementation()->hermProd(*(m.getImplementation()), 'R') ).conjugateTranspose().clone());
}

/* ComplexMatrix multiplications  */
ComplexMatrix ComplexMatrix::operator* (const TriangularComplexMatrix & m) const
{
  char uplo('L');
  if (!m.isLowerTriangular()) uplo = 'R';
  return Implementation(m.getImplementation()->triangularProd(*(getImplementation()), 'R', uplo).clone());
}

/* ComplexMatrix multiplication - Matrix on right */
ComplexMatrix ComplexMatrix::operator* (const Matrix & m) const
{
  return Implementation((getImplementation()->genProd(*(m.getImplementation())) ).clone());
}

/* ComplexMatrix multiplication - SymmetricMatrix on right */
ComplexMatrix ComplexMatrix::operator* (const SymmetricMatrix & m) const
{
  m.checkSymmetry();
  return Implementation((getImplementation()->genProd(*(m.getImplementation())) ).clone());
}

/* ComplexMatrix multiplication - SymmetricMatrix on right */
ComplexMatrix ComplexMatrix::operator* (const IdentityMatrix & ) const
{
  return (*this);
}

/* Multiplication with a ScalarCollection  */
ComplexMatrix::ComplexCollection ComplexMatrix::operator* (const ScalarCollection & collection) const
{
  return getImplementation()->genVectProd(collection) ;
}

/* Multiplication with a Point  */
ComplexMatrix::ComplexCollection ComplexMatrix::operator* (const Point & point) const
{
  return getImplementation()->genVectProd(point) ;
}

/* Multiplication with a ComplexCollection */
ComplexMatrix::ComplexCollection ComplexMatrix::operator* (const ComplexCollection & collection) const
{
  return getImplementation()->genVectProd(collection) ;
}

/* Multiplication with a Complex */
ComplexMatrix ComplexMatrix::operator* (const Complex s) const
{
  return Implementation((*getImplementation() * s ).clone());
}

/* Division by a Complex*/
ComplexMatrix ComplexMatrix::operator/ (const Complex s) const
{
  return Implementation((*getImplementation() / s ).clone());
}


/* Empty returns true if there is no element in the matrix */
Bool ComplexMatrix::isEmpty() const
{
  return getImplementation()->isEmpty() ;
}


/* Comparison operator */
Bool ComplexMatrix::operator == (const ComplexMatrix & rhs) const
{
  const ComplexMatrix &lhs(*this);
  return (*(lhs.getImplementation()) == *(rhs.getImplementation()) );
}


const Complex* ComplexMatrix::data() const
{
  return getImplementation()->data();
}


UnsignedInteger ComplexMatrix::elementSize() const
{
  return getImplementation()->elementSize();
}


UnsignedInteger ComplexMatrix::stride(const UnsignedInteger dim) const
{
  return getImplementation()->stride(dim);
}


END_NAMESPACE_OPENTURNS
