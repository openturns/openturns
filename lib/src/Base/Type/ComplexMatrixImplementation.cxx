//                                               -*- C++ -*-
/**
 *  @brief ComplexMatrixImplementation implements the classical mathematicalComplexMatrixImplementation
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
#include <cstdlib>

#include "openturns/ComplexMatrixImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComplexMatrixImplementation)


static const Factory<ComplexMatrixImplementation> Factory_ComplexMatrixImplementation;

/* Default constructor */
ComplexMatrixImplementation::ComplexMatrixImplementation()
  : PersistentCollection<Complex>()
  , nbRows_(0)
  , nbColumns_(0)
{
  // Nothing to do
}

/* Constructor with size (rowDim and colDim) */
/* The ComplexMatrixImplementation is made up of a collection of rowDim*colDim elements */
/* The ComplexMatrixImplementation is viewed as a set of column vectors read one after another */
ComplexMatrixImplementation::ComplexMatrixImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim)
  : PersistentCollection<Complex>(rowDim * colDim, Complex(0.0, 0.0))
  , nbRows_(rowDim)
  , nbColumns_(colDim)
{
  // Nothing to do
}

/* Constructor from external collection */
ComplexMatrixImplementation::ComplexMatrixImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const Collection<Complex> & elementsValues)
  : PersistentCollection<Complex>(rowDim * colDim, Complex(0.0, 0.0))
  , nbRows_(rowDim)
  , nbColumns_(colDim)
{
  const UnsignedInteger matrixSize = std::min(rowDim * colDim, elementsValues.getSize());
  for(UnsignedInteger i = 0; i < matrixSize; ++i) operator[](i) = elementsValues[i];
}

/* Constructor from external collection */
ComplexMatrixImplementation::ComplexMatrixImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const Collection<Scalar> & elementsValues)
  : PersistentCollection<Complex>(rowDim * colDim, Complex(0.0, 0.0))
  , nbRows_(rowDim)
  , nbColumns_(colDim)
{
  const UnsignedInteger matrixSize = std::min(rowDim * colDim, elementsValues.getSize());
  //  Implicit cast from Scalar into Complex
  for(UnsignedInteger i = 0; i < matrixSize; ++i) operator[](i) = elementsValues[i];
}


ComplexMatrixImplementation::ComplexMatrixImplementation(const MatrixImplementation & matrix)
  : PersistentCollection<Complex>(matrix.getNbRows() * matrix.getNbColumns(), 0.0)
  , nbRows_(matrix.getNbRows())
  , nbColumns_(matrix.getNbColumns())
{
  // Copy from matrix
  const UnsignedInteger matrixSize = nbRows_ * nbColumns_;
  for(UnsignedInteger i = 0; i < matrixSize; ++i) operator[](i) = matrix[i];
}

/* Virtual constructor */
ComplexMatrixImplementation * ComplexMatrixImplementation::clone() const
{
  return new ComplexMatrixImplementation(*this);
}

/* Resolution of a linear system : rectangular matrix
 * MX = b, M is an mxn matrix, b is an mxq matrix and
 * X is an nxq matrix */
ComplexMatrixImplementation ComplexMatrixImplementation::solveLinearSystemRect (const ComplexMatrixImplementation & b,
    const Bool keepIntact)
{
  if (nbRows_ != b.nbRows_) throw InvalidDimensionException(HERE) << "The right-hand side has row dimension=" << b.nbRows_ << ", expected " << nbRows_;
  if ((nbRows_ == 0) || (nbColumns_ == 0) || (b.nbColumns_ == 0)) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix or empty right-hand side";
  int m(nbRows_);
  int n(nbColumns_);
  // B is an extended copy of b, it must be large enought to store the solution, see LAPACK documentation
  int p(std::max(m, n));
  int q(b.nbColumns_);
  ComplexMatrixImplementation B(p, q);
  for(UnsignedInteger j = 0; j < static_cast<UnsignedInteger>(q); ++j)
    for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(m); ++i)
      B(i, j) = b(i, j);
  int nrhs(q);
  int lwork(-1);
  ComplexCollection work(1);
  Point rwork(2 * n);
  int info;
  std::vector<int> jpiv(n);
  double rcond(ResourceMap::GetAsScalar("Matrix-DefaultSmallPivot"));
  int rank;

  ComplexMatrixImplementation Q;
  if (keepIntact) Q = ComplexMatrixImplementation(*this);
  ComplexMatrixImplementation & A = keepIntact ? Q : *this;

  // (int *m, int *n, int *nrhs, std::complex<double> *A, int *lda, std::complex<double> *B, int *ldb, int *jpvt, double *rcond, int *rank, std::complex<double> *work, int *lwork, double *rwork, int *info)
  zgelsy_(&m, &n, &nrhs, &A[0], &m, &B[0], &p, &jpiv[0], &rcond, &rank, &work[0], &lwork, &rwork[0], &info);
  lwork = static_cast<int>(std::real(work[0]));
  work = ComplexCollection(lwork);
  zgelsy_(&m, &n, &nrhs, &A[0], &m, &B[0], &p, &jpiv[0], &rcond, &rank, &work[0], &lwork, &rwork[0], &info);

  ComplexMatrixImplementation result(n, q);
  for(UnsignedInteger j = 0; j < static_cast<UnsignedInteger>(q); ++j)
    for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(n); ++i)
      result(i, j) = B(i, j);
  return result;
}

/* Resolution of a linear system : rectangular matrix
 * Mx = b, M is an mxn matrix, b is an m-dimensional
 * vector and x is an n-dimensional vector */
ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::solveLinearSystemRect (const ComplexCollection & b,
    const Bool keepIntact)
{
  const UnsignedInteger m = b.getSize();
  if (nbRows_ != m) throw InvalidDimensionException(HERE) << "The right-hand side dimension is " << m << ", expected " << nbRows_;
  if (nbRows_ == 0) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix";
  // Solve the matrix linear system
  // A ComplexMatrixImplementation is also a collection of Complex, so it is automatically converted into a ComplexCollection
  return solveLinearSystemRect(ComplexMatrixImplementation(m, 1, b), keepIntact);
}



/* Set small elements to zero */
ComplexMatrixImplementation ComplexMatrixImplementation::clean(const Scalar threshold) const
{
  // Nothing to do for nonpositive threshold
  if (threshold <= 0.0) return *this;
  ComplexMatrixImplementation result(nbRows_, nbColumns_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < nbRows_; ++i)
    {
      const Complex value((*this)[convertPosition(i, j)]);
      Scalar realPart = std::real(value);
      Scalar imagPart = std::imag(value);
      if (std::abs(realPart) < 0.5 * threshold) realPart = 0.0;
      else realPart = threshold * round(realPart / threshold);
      if (std::abs(imagPart) < 0.5 * threshold) imagPart = 0.0;
      else imagPart = threshold * round(imagPart / threshold);
      result(i, j) = Complex(realPart, imagPart);
    }
  return result;
}

/* Set small elements to zero */
ComplexMatrixImplementation ComplexMatrixImplementation::cleanHerm(const Scalar threshold) const
{
  hermitianize();
  return clean(threshold);
}

/* String converter */
String ComplexMatrixImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " name=" << getName()
         << " rows=" << nbRows_
         << " columns=" << nbColumns_
         << " values=" << PersistentCollection<Complex>::__repr__();
}

String ComplexMatrixImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  // Special case for empty matrix
  if (nbRows_ == 0 || nbColumns_ == 0) return oss;
  // Special case for matrix with 1 row
  if (nbRows_ == 1)
  {
    oss << "(";
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      oss << (j > 0 ? " " : "") << operator()(0, j);
    oss << ")" << Os::GetEndOfLine();
    return oss;
  }
  // Array for the widths of the different components
  Collection<UnsignedInteger> widthColumns(nbColumns_, 0);
  // First loop across the values to determine the optimal width for printing
  for (UnsignedInteger i = 0; i < nbRows_; ++i)
  {
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      widthColumns[j] = std::max(widthColumns[j], static_cast<UnsignedInteger>(String(OSS() << operator()(i, j)).length()));
  }
  // Second loop
  for (UnsignedInteger i = 0; i < nbRows_; ++i)
  {
    oss << (i == 0 ? "/" : (i == nbRows_ - 1 ? "\\" : "|"));
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      oss << (j > 0 ? " " : "") << std::setw(widthColumns[j]) << operator()(i, j);
    if (i == 0) oss << "\\" << Os::GetEndOfLine() << offset;
    else if (i == nbRows_ - 1) oss << "/";
    else oss << "|" << Os::GetEndOfLine() << offset;
  }
  return oss;
}

/* Operator () gives access to the elements of theComplexMatrixImplementation (to modify these elements) */
/* The element of theComplexMatrixImplementation is designated by its row number i and its column number j */
/* the first element of theComplexMatrixImplementation is m(0,0) */
Complex & ComplexMatrixImplementation::operator () (const UnsignedInteger i,
    const UnsignedInteger j)
{
  if (i >= nbRows_) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (j >= nbColumns_) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  return operator[](convertPosition(i, j));
}

/* Operator () gives access to the elements of theComplexMatrixImplementation (read only) */
/* The element of theComplexMatrixImplementation is designated by its row number i and its column number j */
const Complex & ComplexMatrixImplementation::operator () (const UnsignedInteger i,
    const UnsignedInteger j)  const
{
  if (i >= nbRows_) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (j >= nbColumns_) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  return operator[](convertPosition(i, j));
}


/* Get the dimensions of the ComplexMatrixImplementation : number of rows */
UnsignedInteger ComplexMatrixImplementation::getNbRows() const
{
  return nbRows_;
}

/* Get the dimensions of the ComplexMatrixImplementation : number of columns */
UnsignedInteger ComplexMatrixImplementation::getNbColumns() const
{
  return nbColumns_;
}

/* Get the dimensions of the ComplexMatrixImplementation : dimension (square matrix : nbRows_) */
UnsignedInteger ComplexMatrixImplementation::getDimension() const
{
  return nbRows_;
}

/* ComplexMatrixImplementation transpose */
ComplexMatrixImplementation ComplexMatrixImplementation::transpose () const
{
  ComplexMatrixImplementation trans(nbColumns_, nbRows_);
  // The source matrix is accessed columnwise in the natural order
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < nbRows_; ++i)
      trans(j, i) = operator()(i, j);
  return trans;
}

/* ComplexMatrixImplementation transpose
 * for hermitian matrices. It reduces to copy the upper triangle into
 * the lower triangle, which means to conjugate the lower triangle */
ComplexMatrixImplementation ComplexMatrixImplementation::transposeHerm () const
{
  ComplexMatrixImplementation result(nbColumns_, nbRows_);
  // The lower triangle of the source matrix is accessed columnwise in the natural order
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = j; i < nbRows_; ++i)
      result(i, j) = std::conj(operator()(i, j));
  return result;
}

/* ComplexMatrixImplementation conjugate */
ComplexMatrixImplementation ComplexMatrixImplementation::conjugate () const
{
  ComplexMatrixImplementation result(nbRows_, nbColumns_);
  // The source matrix is accessed columnwise in the natural order
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < nbRows_; ++i)
      result(i, j) = std::conj(operator()(i, j));
  return result;
}

/* ComplexMatrixImplementation conjugate triangular */
ComplexMatrixImplementation ComplexMatrixImplementation::conjugateHerm () const
{
  return transposeHerm();
}

/* ComplexMatrixImplementation conjugateTranspose */
ComplexMatrixImplementation ComplexMatrixImplementation::conjugateTranspose () const
{
  ComplexMatrixImplementation result(nbColumns_, nbRows_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < nbRows_; ++i)
      result(j, i) = std::conj(operator()(i, j));
  return result;
}

/* Hermitianize ComplexMatrixImplementation in case it is an hermitian matrix (stored as a triangular matrix) */
void ComplexMatrixImplementation::hermitianize() const
{
  ComplexMatrixImplementation *refThis(const_cast<ComplexMatrixImplementation *>(this));
  // The lower triangle of the source matrix is accessed columnwise in the natural order
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = j + 1; i < nbRows_; ++i)
    {
      const Complex value(operator[](convertPosition(i, j)));
      if (std::abs(value.imag()) == 0.0) refThis->operator[](convertPosition(j, i)) = value.real();
      else refThis->operator[](convertPosition(j, i)) = std::conj(value);
    }
}


/* Get the real part of the matrix */
MatrixImplementation ComplexMatrixImplementation::realRect() const
{
  MatrixImplementation result(nbRows_, nbColumns_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < nbRows_; ++i)
      result(i, j) = (*this)(i, j).real();
  return result;
}

MatrixImplementation ComplexMatrixImplementation::realSym() const
{
  MatrixImplementation result(nbRows_, nbColumns_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = j; i < nbRows_; ++i)
      result(i, j) = (*this)(i, j).real();
  return result;
}

/* Get the imaginary part of the matrix */
MatrixImplementation ComplexMatrixImplementation::imagRect() const
{
  MatrixImplementation result(nbRows_, nbColumns_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < nbRows_; ++i)
      result(i, j) = (*this)(i, j).imag();
  return result;
}

MatrixImplementation ComplexMatrixImplementation::imagSym() const
{
  MatrixImplementation result(nbRows_, nbColumns_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = j; i < nbRows_; ++i)
      result(i, j) = (*this)(i, j).imag();
  return result;
}


/* Empty returns true if there is no element in theComplexMatrixImplementation */
Bool ComplexMatrixImplementation::isEmpty() const
{
  return ((nbRows_ == 0)  || (nbColumns_ == 0) || (PersistentCollection<Complex>::isEmpty()));
}

/* Returns true if triangular lower or upper */
Bool ComplexMatrixImplementation::isTriangular(Bool lower) const
{
  if ( nbRows_ == nbColumns_ )
  {
    for ( UnsignedInteger j = 1; j < nbColumns_; ++ j )
      for ( UnsignedInteger i = 0; i < j; ++ i )
        if ( std::abs( (*this)[lower ?  convertPosition(i, j) : convertPosition(j, i)] ) > 0.0 )
          return false;
    return true;
  }
  else
    return false;
}

/* ComplexMatrixImplementation addition (must have the same dimensions) */
ComplexMatrixImplementation ComplexMatrixImplementation::operator + (const ComplexMatrixImplementation & matrix) const
{
  if ((nbRows_ != matrix.nbRows_ ) || (nbColumns_ != matrix.nbColumns_ )) throw InvalidDimensionException(HERE) << "Cannot add matrices with incompatible dimensions";
  // Must copy as add will be overwritten by the operation
  ComplexMatrixImplementation result(matrix);
  int size(nbRows_ * nbColumns_);
  Complex alpha(1.0, 0.0);
  int one = 1;
  // Lapack routine
  zaxpy_(&size, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &one, &result[0], &one);

  return result;
}

/* ComplexMatrixImplementation addition (must have the same dimensions) */
ComplexMatrixImplementation ComplexMatrixImplementation::operator + (const MatrixImplementation & matrix) const
{
  if ((nbRows_ != matrix.getNbRows() ) || (nbColumns_ != matrix.getNbColumns() )) throw InvalidDimensionException(HERE) << "Cannot add matrices with incompatible dimensions";
  // Must copy as add will be overwritten by the operation
  ComplexMatrixImplementation result(matrix);
  int size(nbRows_ * nbColumns_);
  Complex alpha(1.0, 0.0);
  int one = 1;
  // Lapack routine
  zaxpy_(&size, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &one, &result[0], &one);

  return result;
}

/* ComplexMatrixImplementation subtraction (must have the same dimensions) */
ComplexMatrixImplementation ComplexMatrixImplementation::operator - (const ComplexMatrixImplementation & matrix) const
{
  if ((nbRows_ != matrix.nbRows_ ) || (nbColumns_ != matrix.nbColumns_ )) throw InvalidDimensionException(HERE) << "Cannot subtract matrices with incompatible dimensions";
  // Must copy as add will be overwritten by the operation
  ComplexMatrixImplementation result(*this);
  int size(nbRows_ * nbColumns_);
  Complex alpha(-1.0, 0.0);
  int one = 1;

  zaxpy_(&size, &alpha, const_cast<std::complex<double>*>(&(matrix)[0]), &one, &result[0], &one);

  return result;
}

/* ComplexMatrixImplementation subtraction (must have the same dimensions) */
ComplexMatrixImplementation ComplexMatrixImplementation::operator - (const MatrixImplementation & matrix) const
{
  return operator - (ComplexMatrixImplementation(matrix));
}

/* Multiplication with a Complex */
ComplexMatrixImplementation ComplexMatrixImplementation::operator * (const Complex s) const
{
  // Check if s is null
  if (std::abs(s) == 0.0 ) return ComplexMatrixImplementation(nbRows_, nbColumns_);
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return *this;
  ComplexMatrixImplementation scalprod(*this);

  //
  Complex alpha(s);
  int one(1);
  int n_(nbRows_ * nbColumns_);
  // Lapack routine
  zscal_(&n_, &alpha, &scalprod[0], &one);

  return scalprod;
}

/* Division by a Scalar*/
ComplexMatrixImplementation ComplexMatrixImplementation::operator / (const Complex s) const
{
  if (std::abs(s) == 0) throw InvalidArgumentException(HERE) ;

  return operator * (1.0 / s);
}

/* ComplexMatrixImplementation multiplications (must have consistent dimensions) */
ComplexMatrixImplementation ComplexMatrixImplementation::genProd(const ComplexMatrixImplementation & matrix) const
{
  if (nbColumns_ != matrix.nbRows_) throw InvalidDimensionException(HERE) << "Invalid dimensions in complex matrix/ complex matrix product left="
        << nbRows_ << "x" << nbColumns_
        << " right=" << matrix.nbRows_ << "x"
        << matrix.nbColumns_;

  ComplexMatrixImplementation mult(nbRows_, matrix.nbColumns_);
  if ((nbRows_ == 0) || (nbColumns_ == 0) || (matrix.nbColumns_ == 0)) return mult;
  char transa('N');
  char transb('N');
  int m(nbRows_);
  int k(nbColumns_);
  int n(matrix.nbColumns_);
  Complex alpha(1.0, 0.0);
  Complex beta(0.0, 0.0);
  int ltransa(1);
  int ltransb(1);

  //Lapack routine
  zgemm_(&transa, &transb, &m, &n, &k, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &m, const_cast<std::complex<double>*>(&(matrix[0])), &k, &beta, &mult[0], &m, &ltransa, &ltransb);
  return mult;
}

ComplexMatrixImplementation ComplexMatrixImplementation::symProd (const ComplexMatrixImplementation & matrix,
    const char hermSide) const
{
  if (nbColumns_ != matrix.nbRows_) throw InvalidDimensionException(HERE) << "Invalid dimensions in matrix/matrix product left="
        << nbRows_ << "x" << nbColumns_
        << " right=" << matrix.nbRows_ << "x"
        << matrix.nbColumns_;

  ComplexMatrixImplementation mult(nbRows_, matrix.nbColumns_);
  if ((nbRows_ == 0) || (nbColumns_ == 0) || (matrix.nbColumns_ == 0)) return mult;
  char side(hermSide);
  char uplo('L');
  int m(nbRows_);
  int k(nbColumns_);
  int n(matrix.nbColumns_);
  Complex alpha(1.0);
  Complex beta(0.0);
  int lside(1);
  int luplo(1);

  // Lapack routine
  zsymm_(&side, &uplo, &m, &n, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &m, const_cast<std::complex<double>*>(&(matrix[0])), &k, &beta, &mult[0], &m, &lside, &luplo);

  return mult;
}

ComplexMatrixImplementation ComplexMatrixImplementation::hermProd(const ComplexMatrixImplementation & matrix,
    const char hermSide) const
{
  if (nbColumns_ != matrix.nbRows_) throw InvalidDimensionException(HERE) << "Invalid dimensions in matrix/matrix product left="
        << nbRows_ << "x" << nbColumns_
        << " right=" << matrix.nbRows_ << "x"
        << matrix.nbColumns_;

  ComplexMatrixImplementation mult(nbRows_, matrix.nbColumns_);
  if ((nbRows_ == 0) || (nbColumns_ == 0) || (matrix.nbColumns_ == 0)) return mult;
  char side(hermSide);
  char uplo('L');
  int m(nbRows_);
  int k(nbColumns_);
  int n(matrix.nbColumns_);
  Complex alpha(1.0);
  Complex beta(0.0);
  int lside(1);
  int luplo(1);

  // Lapack routine
  if (hermSide == 'L') zhemm_(&side, &uplo, &m, &n, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &m, const_cast<std::complex<double>*>(&(matrix[0])), &k, &beta, &mult[0], &m, &lside, &luplo);
  else zhemm_(&side, &uplo, &m, &n, &alpha, const_cast<std::complex<double>*>(&(matrix[0])), &k, const_cast<std::complex<double>*>(&((*this)[0])), &m, &beta, &mult[0], &m, &lside, &luplo);

  return mult;
}

ComplexMatrixImplementation ComplexMatrixImplementation::triangularProd(const ComplexMatrixImplementation & matrix,
    const char triangularSide,
    const char upperLower) const
{
  if (nbColumns_ != matrix.nbRows_) throw InvalidDimensionException(HERE) << "Invalid dimensions in matrix/matrix product left="
        << nbRows_ << "x" << nbColumns_
        << " right=" << matrix.nbRows_ << "x"
        << matrix.nbColumns_;

  ComplexMatrixImplementation mult(matrix);
  if ((nbRows_ == 0) || (nbColumns_ == 0) || (matrix.nbColumns_ == 0)) return mult;
  char side(triangularSide);
  int lside(1);

  char uplo(upperLower);
  int luplo(1);

  char trans('N');
  int ltrans(1);

  char diag('N');
  int ldiag(1);

  int m(nbRows_);
  int n(matrix.nbColumns_);

  Complex alpha(1.0);

  // Lapack routine
  ztrmm_(&side, &uplo, &trans, &diag, &m, &n, &alpha, const_cast<std::complex<double>*>(&((*this)[0])),  &m, const_cast<std::complex<double>*>(&(mult[0])), &m, &lside, &luplo, &ltrans,  &ldiag);
  return mult;
}

/* Integer power, general matrix */
ComplexMatrixImplementation ComplexMatrixImplementation::genPower(const UnsignedInteger n) const
{
  Bool first = true;
  UnsignedInteger exponent = n;
  ComplexMatrixImplementation y;
  ComplexMatrixImplementation z(*this);
  while (exponent > 0)
  {
    // t is the right bit of exponent
    const UnsignedInteger t = exponent % 2;
    // remove last bit from exponent
    exponent /= 2;
    // if right bit is 1
    if (t != 0)
    {
      // if it is the rightmost bit equals to 1
      if (first)
      {
        first = false;
        y = z;
      }
      else y = y.genProd(z);
      // if no more bit to consider
      if (exponent == 0) return y;
    }
    // Square the contribution
    z = z.genProd(z);
  }
  return y;
}

/* Integer power, symmetric matrix */
ComplexMatrixImplementation ComplexMatrixImplementation::symPower(const UnsignedInteger n) const
{
  Bool first = true;
  UnsignedInteger exponent = n;
  ComplexMatrixImplementation y;
  ComplexMatrixImplementation z(*this);
  while (exponent > 0)
  {
    // t is the right bit of exponent
    const UnsignedInteger t = exponent % 2;
    // remove last bit from exponent
    exponent /= 2;
    // if right bit is 1
    if (t != 0)
    {
      // if it is the rightmost bit equals to 1
      if (first)
      {
        first = false;
        y = z;
      }
      else y = y.symProd(z, 'L');
      // if no more bit to consider
      if (exponent == 0) return y;
    }
    // Square the contribution
    z = z.symProd(z, 'L');
  }
  return y;
}

/* Integer power, hermitian matrix */
ComplexMatrixImplementation ComplexMatrixImplementation::hermPower(const UnsignedInteger n) const
{
  Bool first = true;
  UnsignedInteger exponent = n;
  ComplexMatrixImplementation y;
  ComplexMatrixImplementation z(*this);
  while (exponent > 0)
  {
    // t is the right bit of exponent
    const UnsignedInteger t = exponent % 2;
    // remove last bit from exponent
    exponent /= 2;
    // if right bit is 1
    if (t != 0)
    {
      // if it is the rightmost bit equals to 1
      if (first)
      {
        first = false;
        y = z;
      }
      else y = y.hermProd(z, 'L');
      // if no more bit to consider
      if (exponent == 0) return y;
    }
    // Square the contribution
    z = z.hermProd(z, 'L');
  }
  return y;
}

/* Multiplications with a ComplexCollection */
ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::genVectProd(const ComplexCollection & pt) const
{
  if (nbColumns_ != pt.getSize() ) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix/vector product";

  ComplexCollection prod(nbRows_);
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return prod;
  char trans('N');
  int m_(nbRows_);
  int n_(nbColumns_);
  int one(1);
  Complex alpha(1.0, 0.0);
  Complex beta(0.0, 0.0);
  int ltrans(1);

  zgemv_(&trans, &m_, &n_, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &m_, const_cast<std::complex<double>*>(&(pt[0])), &one, &beta, &prod[0], &one, &ltrans);

  return prod;
}

/* Multiplications with a ScalarCollection */
ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::genVectProd(const ScalarCollection & pt) const
{
  if (nbColumns_ != pt.getSize() ) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix/vector product";

  ComplexCollection prod(nbRows_);
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return prod;
  ComplexCollection copyPoint(nbRows_);
  for (UnsignedInteger i = 0; i < pt.getSize(); ++i) copyPoint[i] = pt[i];
  char trans('N');
  int m_(nbRows_);
  int n_(nbColumns_);
  int one(1);
  Complex alpha(1.0, 0.0);
  Complex beta(0.0, 0.0);
  int ltrans(1);

  zgemv_(&trans, &m_, &n_, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &m_, const_cast<std::complex<double>*>(&(copyPoint[0])), &one, &beta, &prod[0], &one, &ltrans);

  return prod;
}

/* Multiplications with a Point */
ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::genVectProd(const Point & pt) const
{
  if (nbColumns_ != pt.getSize() ) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix/vector product";

  ComplexCollection prod(nbRows_);
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return prod;
  ComplexCollection copyPoint(nbRows_);
  for (UnsignedInteger i = 0; i < pt.getSize(); ++i) copyPoint[i] = pt[i];
  char trans('N');
  int m_(nbRows_);
  int n_(nbColumns_);
  int one(1);
  Complex alpha(1.0, 0.0);
  Complex beta(0.0, 0.0);
  int ltrans(1);

  zgemv_(&trans, &m_, &n_, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &m_, const_cast<std::complex<double>*>(&(copyPoint[0])), &one, &beta, &prod[0], &one, &ltrans);

  return prod;
}

ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::hermVectProd(const ComplexCollection & pt) const
{
  if (nbColumns_ != pt.getSize() ) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix/vector product";

  ComplexCollection prod(nbRows_);
  // In this case, nbRows_ == nbColumns_
  if (nbRows_ == 0) return prod;
  char uplo('L');
  int n(nbRows_);
  int one(1);
  Complex alpha(1.0, 0.0);
  Complex beta(0.0, 0.0);
  int luplo(1);
  zhemv_(&uplo, &n, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &n, const_cast<std::complex<double>*>(&(pt[0])), &one, &beta, &prod[0], &one, &luplo);

  return prod;
}

ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::hermVectProd(const ScalarCollection & pt) const
{
  if (nbColumns_ != pt.getSize() ) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix/vector product";

  ComplexCollection prod(nbRows_);
  // In this case, nbRows_ == nbColumns_
  if (nbRows_ == 0) return prod;
  ComplexCollection copyPoint(nbRows_);
  for (UnsignedInteger i = 0; i < pt.getSize(); ++i) copyPoint[i] = pt[i];
  char uplo('L');
  int n(nbRows_);
  int one(1);
  Complex alpha(1.0, 0.0);
  Complex beta(0.0, 0.0);
  int luplo(1);
  zhemv_(&uplo, &n, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &n, const_cast<std::complex<double>*>(&(copyPoint[0])), &one, &beta, &prod[0], &one, &luplo);

  return prod;
}

ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::hermVectProd(const Point & pt) const
{
  if (nbColumns_ != pt.getSize() ) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix/vector product";

  ComplexCollection prod(nbRows_);
  // In this case, nbRows_ == nbColumns_
  if (nbRows_ == 0) return prod;
  ComplexCollection copyPoint(nbRows_);
  for (UnsignedInteger i = 0; i < pt.getSize(); ++i) copyPoint[i] = pt[i];
  char uplo('L');
  int n(nbRows_);
  int one(1);
  Complex alpha(1.0, 0.0);
  Complex beta(0.0, 0.0);
  int luplo(1);
  zhemv_(&uplo, &n, &alpha, const_cast<std::complex<double>*>(&((*this)[0])), &n, const_cast<std::complex<double>*>(&(copyPoint[0])), &one, &beta, &prod[0], &one, &luplo);

  return prod;
}

ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::triangularVectProd(const ComplexCollection & pt,
    const char side) const
{
  // Product matrix vector using Blas
  // side tells if the triangular matrix is "Lower" or "Upper"
  char uplo(side);
  int luplo(1);

  // trans tells if the matrix is transposed or not
  char trans('N');
  int ltrans(1);

  // diag tells if M is unit diagonal or not
  char diag('N');
  int ldiag(1);

  // the dimension of the matrix
  int n(nbRows_);

  // leading dimension of M
  int lda(nbRows_);
  int one(1);

  ComplexCollection x(pt);
  ztrmv_(&uplo, &trans, &diag, &n, const_cast<std::complex<double>*>(&((*this)[0])), &lda, const_cast<std::complex<double>*>(&(x[0])), &one, &luplo, &ltrans, &ldiag);
  return x;
}

ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::triangularVectProd(const ScalarCollection & pt,
    const char side) const
{
  char uplo(side);
  int luplo(1);

  // trans tells if the matrix is transposed or not
  char trans('N');
  int ltrans(1);

  // diag tells if M is unit diagonal or not
  char diag('N');
  int ldiag(1);

  // the dimension of the matrix
  int n(nbRows_);

  // leading dimension of M
  int lda(nbRows_);
  int one(1);

  ComplexCollection x(nbRows_);
  for (UnsignedInteger i = 0; i < pt.getSize(); ++i) x[i] = pt[i];

  ztrmv_(&uplo, &trans, &diag, &n, const_cast<std::complex<double>*>(&((*this)[0])), &lda, const_cast<std::complex<double>*>(&(x[0])), &one, &luplo, &ltrans, &ldiag);
  return x;
}

ComplexMatrixImplementation::ComplexCollection ComplexMatrixImplementation::triangularVectProd(const Point & pt,
    const char side) const
{
  char uplo(side);
  int luplo(1);

  // trans tells if the matrix is transposed or not
  char trans('N');
  int ltrans(1);

  // diag tells if M is unit diagonal or not
  char diag('N');
  int ldiag(1);

  // the dimension of the matrix
  int n(nbRows_);

  // leading dimension of M
  int lda(nbRows_);
  int one(1);

  ComplexCollection x(nbRows_);
  for (UnsignedInteger i = 0; i < pt.getSize(); ++i) x[i] = pt[i];

  ztrmv_(&uplo, &trans, &diag, &n, const_cast<std::complex<double>*>(&((*this)[0])), &lda, const_cast<std::complex<double>*>(&(x[0])), &one, &luplo, &ltrans, &ldiag);
  return x;
}


Bool ComplexMatrixImplementation::isHermitian() const
{
  if ( nbRows_ == nbColumns_ )
  {
    for ( UnsignedInteger i = 1; i < nbRows_; ++ i )
      for ( UnsignedInteger j = 0; j < i; ++ j )
        if ( operator[](convertPosition(i, j)) != std::conj(operator[](convertPosition(j, i))) )
          return false;
    return true;
  }
  else
    return false;
}


/* Check if the matrix is HPD */
Bool ComplexMatrixImplementation::isHermitianPositiveDefinite(const Bool keepIntact)
{
  // Exception for null dimension
  if (getDimension() == 0) throw InvalidDimensionException(HERE) << "Cannot check the hermitian definite positiveness of an empty matrix";
  int info;
  int n(nbRows_);
  char uplo('L');
  int luplo(1);
  if (keepIntact)
  {
    ComplexMatrixImplementation A(*this);
    zpotrf_(&uplo, &n, &A[0], &n, &info, &luplo);
    if (info != 0) throw InternalException(HERE) << "Lapack ZPOTRF: error code=" << info;
  }
  else
  {
    zpotrf_(&uplo, &n, &(*this)[0], &n, &info, &luplo);
    if (info != 0) throw InternalException(HERE) << "Lapack ZPOTRF: error code=" << info;
  }
  return (info == 0) ;
}

/* Build the Cholesky factorization of the hermitian matrix */
ComplexMatrixImplementation ComplexMatrixImplementation::computeCholesky(const Bool keepIntact)
{
  // Exception for null dimension
  if (getDimension() == 0) throw InvalidDimensionException(HERE) << "Cannot compute the Cholesky decomposition of an empty matrix";
  int info;
  int n(nbRows_);
  char uplo('L');
  int luplo(1);

  ComplexMatrixImplementation Q;
  if (keepIntact) Q = ComplexMatrixImplementation(*this);
  ComplexMatrixImplementation & A = keepIntact ? Q : *this;

  zpotrf_(&uplo, &n, &A[0], &n, &info, &luplo);
  if (info != 0) throw InternalException(HERE) << "Lapack ZPOTRF: error code=" << info;
  for (UnsignedInteger j = 0; j < (UnsignedInteger)(n); ++j)
    for (UnsignedInteger i = 0; i < (UnsignedInteger)(j); ++i)
      A(i, j) = 0.0;//Complex(0.0, 0.0);
  // Check return code from Lapack
  if(info != 0)
    throw InvalidArgumentException(HERE) << " Error - Matrix is not positive definite" ;
  return A;
}

/* Comparison operator */
Bool ComplexMatrixImplementation::operator == (const ComplexMatrixImplementation & rhs) const
{
  const ComplexMatrixImplementation &lhs(*this);
  Bool equality = true;

  if (&lhs != &rhs)   // Not the same object
  {
    const Collection<Complex> & refLhs(static_cast<const Collection<Complex> >(lhs));
    const Collection<Complex> & refRhs(static_cast<const Collection<Complex> >(rhs));
    equality = ( lhs.nbRows_ == rhs.nbRows_ && lhs.nbColumns_ == rhs.nbColumns_ && refLhs == refRhs);
  }

  return equality;
}



/* Method save() stores the object through the StorageManager */
void ComplexMatrixImplementation::save(Advocate & adv) const
{
  PersistentCollection<Complex>::save(adv);
  adv.saveAttribute( "nbRows_",    nbRows_);
  adv.saveAttribute( "nbColumns_", nbColumns_);
}

/* Method load() reloads the object from the StorageManager */
void ComplexMatrixImplementation::load(Advocate & adv)
{
  PersistentCollection<Complex>::load(adv);

  adv.loadAttribute( "nbRows_",    nbRows_);
  adv.loadAttribute( "nbColumns_", nbColumns_);
}

UnsignedInteger ComplexMatrixImplementation::stride(const UnsignedInteger dim) const
{
  UnsignedInteger stride = elementSize();
  if (dim > 0)
    stride *= nbRows_;
  return stride;
}


END_NAMESPACE_OPENTURNS
