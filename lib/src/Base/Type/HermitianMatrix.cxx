//                                               -*- C++ -*-
/**
 *  @brief HermitianMatrix implements the classical mathematical hermitian matrix
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
#include "openturns/HermitianMatrix.hxx"
#include "openturns/TriangularComplexMatrix.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/SquareMatrix.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HermitianMatrix);

/* Default constructor */
HermitianMatrix::HermitianMatrix()
  : SquareComplexMatrix(0)
  , hasBeenHermitianized_(false)
{
  // Nothing to do
}


/* Constructor with size (dim, which is the same for nbRows_ and nbColumns_ )*/
HermitianMatrix::HermitianMatrix(const UnsignedInteger dimension)
  : SquareComplexMatrix(dimension, dimension)
  , hasBeenHermitianized_(false)
{
  // Nothing to do
}


/* Constructor with implementation */
HermitianMatrix::HermitianMatrix(const Implementation & i)
  : SquareComplexMatrix(i)
  , hasBeenHermitianized_(false)
{
  // Nothing to do
}

/* String converter */
String HermitianMatrix::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " dimension=" << this->getDimension()
         << " implementation=" << getImplementation()->__repr__();
}

String HermitianMatrix::__str__(const String & offset) const
{
  checkHermitian();
  return SquareComplexMatrix::clean(0.0).__str__(offset);
}

/* Get the dimension of the matrix */
UnsignedInteger HermitianMatrix::getDimension() const
{
  return getImplementation()->getDimension();
}

/* HermitianMatrix transpose */
HermitianMatrix HermitianMatrix::transpose () const
{
  // Quick return for empty or scalar HermitianMatrix
  if (getDimension() <= 1) return (*this);
  return Implementation(getImplementation()->transposeHerm().clone());
}

/* HermitianMatrix conjugate */
HermitianMatrix HermitianMatrix::conjugate () const
{
  // Quick return for empty or scalar HermitianMatrix
  if (getDimension() <= 1) return (*this);
  return Implementation(getImplementation()->conjugateHerm().clone());
}

/* HermitianMatrix conjugate */
HermitianMatrix HermitianMatrix::conjugateTranspose () const
{
  return (*this);
}

//* Get the real part of the matrix */
SymmetricMatrix HermitianMatrix::real() const
{
  return SymmetricMatrix::Implementation(getImplementation()->realSym().clone());
}

/* Get the imaginary part of the matrix */
SquareMatrix HermitianMatrix::imag() const
{
  return SquareMatrix::Implementation(getImplementation()->imagRect().clone());
}

/* Check if the internal representation is really symmetric */
void HermitianMatrix::checkHermitian() const
{
  if (!hasBeenHermitianized_)
  {
    getImplementation()->hermitianize();
    hasBeenHermitianized_ = true;
  }
}

/* Operator () gives access to the elements of the matrix (to modify these elements) */
/* The element of the matrix is designated by its row number i and its column number j */
/* the first element of the matrix is m(0,0) */
NumericalComplex & HermitianMatrix::operator () (const UnsignedInteger i,
    const UnsignedInteger j)
{
  if (i < j) throw InvalidArgumentException(HERE) << "Error: only the lower triangle of an Hermitian matrix can be filled directly.";
  copyOnWrite();
  hasBeenHermitianized_ = false;

  return (*getImplementation())(i, j);
}

/* Operator () gives access to the elements of the matrix (read only) */
/* The element of the matrix is designated by its row number i and its column number j */
const NumericalComplex HermitianMatrix::operator () (const UnsignedInteger i,
    const UnsignedInteger j)  const
{
  return (i >= j) ? (*getImplementation())(i, j) : std::conj((*getImplementation())(j, i)) ;
}


/* HermitianMatrix additions */
HermitianMatrix HermitianMatrix::operator + (const HermitianMatrix & m) const
{
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* complexMatrix additions  */
SquareComplexMatrix HermitianMatrix::operator + (const SquareComplexMatrix & m) const
{
  checkHermitian();
  return Implementation((*getImplementation() + * (m.getImplementation()) ).clone());
}

/* ComplexMatrix substractions  */
HermitianMatrix HermitianMatrix::operator - (const HermitianMatrix & m) const
{
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* HermitianMatrix substractions */
SquareComplexMatrix HermitianMatrix::operator - (const SquareComplexMatrix & m) const
{
  checkHermitian();
  return Implementation((*getImplementation() - * (m.getImplementation()) ).clone());
}

/* ComplexMatrix multiplications  */
ComplexMatrix HermitianMatrix::operator * (const ComplexMatrix & m) const
{
  return Implementation((getImplementation()->hermProd(*(m.getImplementation()), 'L') ).clone());
}

/* ComplexMatrix multiplications  */
SquareComplexMatrix HermitianMatrix::operator * (const SquareComplexMatrix & m) const
{
  return Implementation((getImplementation()->hermProd(*(m.getImplementation()), 'L') ).clone());
}

/* HermitianMatrix multiplications */
SquareComplexMatrix HermitianMatrix::operator * (const HermitianMatrix & m) const
{
  m.checkHermitian();
  return Implementation((getImplementation()->hermProd(*(m.getImplementation()), 'L') ).clone());
}

/* TriangularComplexMatrix multiplications  */
SquareComplexMatrix HermitianMatrix::operator * (const TriangularComplexMatrix & m) const
{
  char uplo('L');
  if (!m.isLowerTriangular()) uplo = 'R';
  return Implementation((m.getImplementation()->triangularProd(*(this->getImplementation()), 'R',  uplo ) ).clone());
}

/* Matrix multiplications */
ComplexMatrix HermitianMatrix::operator * (const Matrix & m) const
{
  return Implementation((getImplementation()->hermProd(*(m.getImplementation()), 'L') ).clone());
}

/* Real SquareMatrix multiplications  */
SquareComplexMatrix HermitianMatrix::operator * (const SquareMatrix & m) const
{
  return Implementation(getImplementation()->hermProd(*(m.getImplementation()), 'L').clone());
}

/* RealSymmetricMatrix multiplications */
SquareComplexMatrix HermitianMatrix::operator * (const SymmetricMatrix & m) const
{
  m.checkSymmetry();
  return Implementation((getImplementation()->hermProd(*(m.getImplementation()), 'L') ).clone());
}

/* IdentityMatrix multiplications  */
HermitianMatrix HermitianMatrix::operator * (const IdentityMatrix & m) const
{
  return (*this);
}

/* Multiplication with a NumericalComplexCollection (must have consistent dimensions) */
HermitianMatrix::NumericalComplexCollection HermitianMatrix::operator * (const NumericalComplexCollection & pt) const
{
  return getImplementation()->hermVectProd(pt) ;
}

/* Multiplication with a NumericalScalarCollection (must have consistent dimensions) */
HermitianMatrix::NumericalComplexCollection HermitianMatrix::operator * (const NumericalScalarCollection & pt) const
{
  return getImplementation()->hermVectProd(pt) ;
}

/* Multiplication with a NumericalPoint (must have consistent dimensions) */
HermitianMatrix::NumericalComplexCollection HermitianMatrix::operator * (const NumericalPoint & pt) const
{
  return getImplementation()->hermVectProd(pt) ;
}

/* Multiplication with a NumericalComplex */
HermitianMatrix HermitianMatrix::operator * (const NumericalComplex s) const
{
  return Implementation((*getImplementation() * s ).clone());
}

/* Division by a NumericalComplex*/
HermitianMatrix HermitianMatrix::operator / (const NumericalComplex s) const
{
  return Implementation((*getImplementation() / s ).clone());
}

/* HermitianMatrix integer power */
HermitianMatrix HermitianMatrix::power(const UnsignedInteger n) const
{
  return Implementation(getImplementation()->hermPower(n).clone());
}

/* Build the Cholesky factorization of the hermitian matrix */
TriangularComplexMatrix HermitianMatrix::computeCholesky(const Bool keepIntact)
{
  return TriangularComplexMatrix(Implementation(getImplementation()->computeCholesky(keepIntact).clone()), true);
}

END_NAMESPACE_OPENTURNS
