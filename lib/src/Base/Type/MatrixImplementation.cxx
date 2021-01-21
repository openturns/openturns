//                                               -*- C++ -*-
/**
 *  @brief MatrixImplementation implements the classical mathematical MatrixImplementation
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
#include <cstdlib>
#include <functional>

#include "openturns/MatrixImplementation.hxx"
#include "openturns/ComplexMatrixImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Os.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(MatrixImplementation)


static const Factory<MatrixImplementation> Factory_MatrixImplementation;

// All the pivots with a magnitude less than this threshold are considered as zero
/* Default constructor */
MatrixImplementation::MatrixImplementation()
  : PersistentCollection<Scalar>()
  , nbRows_(0)
  , nbColumns_(0)
{
  // Nothing to do
}

/* Constructor with size (rowDim and colDim) */
/* The MatrixImplementation is made up of a collection of rowDim*colDim elements */
/* The MatrixImplementation is viewed as a set of column vectors read one after another */
MatrixImplementation::MatrixImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim)
  : PersistentCollection<Scalar>(rowDim * colDim, 0.0)
  , nbRows_(rowDim)
  , nbColumns_(colDim)
{
  // Nothing to do
}

/* Constructor from external collection */
MatrixImplementation::MatrixImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const Collection<Scalar> & elementsValues)
  : PersistentCollection<Scalar>(rowDim * colDim, 0.0)
  , nbRows_(rowDim)
  , nbColumns_(colDim)
{
  const UnsignedInteger matrixSize = std::min(rowDim * colDim, elementsValues.getSize());
  std::copy(elementsValues.begin(), elementsValues.begin() + matrixSize, begin());
}

/* Virtual constructor */
MatrixImplementation * MatrixImplementation::clone() const
{
  return new MatrixImplementation(*this);
}


/* String converter */
String MatrixImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " name=" << getName()
         << " rows=" << nbRows_
         << " columns=" << nbColumns_
         << " values=" << PersistentCollection<Scalar>::__repr__();
}

String MatrixImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  if (nbRows_ == 0 || nbColumns_ == 0) return "[]";

  if ( (nbRows_ >= ResourceMap::GetAsUnsignedInteger("Matrix-size-visible-in-str-from")) ||
       (nbColumns_ >= ResourceMap::GetAsUnsignedInteger("Matrix-size-visible-in-str-from")) )
    oss << nbRows_ << "x" << nbColumns_ << Os::GetEndOfLine();

  size_t lwidth(0);
  size_t rwidth(0);
  for ( UnsignedInteger i = 0; i < nbRows_; ++i )
    for ( UnsignedInteger j = 0; j < nbColumns_; ++j )
    {
      String st(OSS() << (*this)(i, j));
      size_t dotpos(st.find( '.' ));
      lwidth = std::max( lwidth, (dotpos != String::npos) ? dotpos             : st.size() );
      rwidth = std::max( rwidth, (dotpos != String::npos) ? st.size() - dotpos : 0         );
    }

  const char * bracket("[");
  const char * newline("");
  const char * noffset("");
  for ( UnsignedInteger i = 0; i < nbRows_; ++i, newline = Os::GetEndOfLine(), noffset = offset.c_str(), bracket = " " )
  {
    oss << newline << noffset << bracket << "[ ";
    const char * sep("");
    for ( UnsignedInteger j = 0; j < nbColumns_; ++j, sep = " " )
    {
      String st(OSS() << (*this)(i, j));
      size_t dotpos(st.find( '.' ));
      oss << sep << String( lwidth - ((dotpos != String::npos) ? dotpos : st.size()), ' ' )
          << st
          << String( rwidth - ((dotpos != String::npos) ? st.size() - dotpos : 0), ' ' );
    }
    oss << " ]";
  }
  oss << "]";
  return oss;
}

/* Operator () gives access to the elements of the MatrixImplementation (to modify these elements) */
/* The element of the MatrixImplementation is designated by its row number i and its column number j */
/* the first element of the MatrixImplementation is m(0,0) */
Scalar & MatrixImplementation::operator() (const UnsignedInteger i,
    const UnsignedInteger j)
{
  if (!(i < nbRows_)) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (!(j < nbColumns_)) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  return operator[](convertPosition(i, j));
}

/* Operator () gives access to the elements of the MatrixImplementation (read only) */
/* The element of the MatrixImplementation is designated by its row number i and its column number j */
const Scalar & MatrixImplementation::operator() (const UnsignedInteger i,
    const UnsignedInteger j)  const
{
  if (!(i < nbRows_)) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (!(j < nbColumns_)) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  return operator[](convertPosition(i, j));
}


/* Get the dimensions of the MatrixImplementation : number of rows */
UnsignedInteger MatrixImplementation::getNbRows() const
{
  return nbRows_;
}

/* Get the dimensions of the MatrixImplementation : number of columns */
UnsignedInteger MatrixImplementation::getNbColumns() const
{
  return nbColumns_;
}

/* Get the dimensions of the MatrixImplementation : dimension (square matrix : nbRows_) */
UnsignedInteger MatrixImplementation::getDimension() const
{
  return nbRows_;
}

/* MatrixImplementation transpose */
MatrixImplementation MatrixImplementation::transpose() const
{
  MatrixImplementation trans(nbColumns_, nbRows_);
  // The source matrix is accessed columnwise in the natural order
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < nbRows_; ++i)
      trans(j, i) = operator()(i, j);
  return trans;
}

/* MatrixImplementation reshape */
MatrixImplementation MatrixImplementation::reshape(const UnsignedInteger newRowDim,
    const UnsignedInteger newColDim) const
{
  return MatrixImplementation(newRowDim, newColDim, *this);
}

void MatrixImplementation::reshapeInPlace(const UnsignedInteger newRowDim,
    const UnsignedInteger newColDim)
{
  if (newRowDim * newColDim != getSize()) resize(newRowDim * newColDim);
  nbRows_ = newRowDim;
  nbColumns_ = newColDim;
}


/* Row extraction */
const MatrixImplementation MatrixImplementation::getRow(const UnsignedInteger rowIndex) const
{
  if (!(rowIndex < nbRows_)) throw OutOfBoundException(HERE) << "Error: the row index=" << rowIndex << " must be less than the row number=" << nbRows_;
  MatrixImplementation row(1, nbColumns_);
  for (UnsignedInteger i = 0; i < nbColumns_; ++i) row(0, i) = (*this)(rowIndex, i);
  return row;
}

const MatrixImplementation MatrixImplementation::getRowSym(const UnsignedInteger rowIndex) const
{
  if (!(rowIndex < nbRows_)) throw OutOfBoundException(HERE) << "Error: the row index=" << rowIndex << " must be less than the row number=" << nbRows_;
  MatrixImplementation row(1, nbColumns_);
  for (UnsignedInteger i = 0; i < rowIndex; ++i) row(0, i) = (*this)(rowIndex, i);
  for (UnsignedInteger i = rowIndex; i < nbColumns_; ++i) row(0, i) = (*this)(i, rowIndex);
  return row;
}

/* Column extration */
const MatrixImplementation MatrixImplementation::getColumn(const UnsignedInteger columnIndex) const
{
  if (!(columnIndex < nbColumns_)) throw OutOfBoundException(HERE) << "Error: the column index=" << columnIndex << " must be less than the column number=" << nbColumns_;
  MatrixImplementation column(nbRows_, 1);
  for (UnsignedInteger i = 0; i < nbRows_; ++i) column(i, 0) = (*this)(i, columnIndex);
  return column;
}

const MatrixImplementation MatrixImplementation::getColumnSym(const UnsignedInteger columnIndex) const
{
  if (!(columnIndex < nbColumns_)) throw OutOfBoundException(HERE) << "Error: the column index=" << columnIndex << " must be less than the column number=" << nbColumns_;
  MatrixImplementation column(nbRows_, 1);
  for (UnsignedInteger i = 0; i < columnIndex; ++i) column(i, 0) = (*this)(columnIndex, i);
  for (UnsignedInteger i = columnIndex; i < nbRows_; ++i) column(i, 0) = (*this)(i, columnIndex);
  return column;
}


/* MatrixImplementation addition (must have the same dimensions) */
MatrixImplementation MatrixImplementation::operator+ (const MatrixImplementation & matrix) const
{
  if ((nbRows_ != matrix.nbRows_ ) || (nbColumns_ != matrix.nbColumns_ )) throw InvalidDimensionException(HERE) << "Cannot add matrices with incompatible dimensions";
  // Must copy as add will be overwritten by the operation
  MatrixImplementation result(matrix);
  int size(nbRows_ * nbColumns_);
  double alpha(1.0);
  int one(1);
  daxpy_(&size, &alpha, const_cast<double*>(&((*this)[0])), &one, &result[0], &one);

  return result;
}

/* In-place MatrixImplementation addition (must have the same dimensions) */
MatrixImplementation & MatrixImplementation::operator+= (const MatrixImplementation & matrix)
{
  if ((nbRows_ != matrix.nbRows_ ) || (nbColumns_ != matrix.nbColumns_ )) throw InvalidDimensionException(HERE) << "Cannot add matrices with incompatible dimensions";
  // Must copy as add will be overwritten by the operation
  MatrixImplementation result(matrix);
  int size(nbRows_ * nbColumns_);
  double alpha(1.0);
  int one(1);
  daxpy_(&size, &alpha, const_cast<double*>(&(matrix[0])), &one, &(*this)[0], &one);

  return *this;
}

/* MatrixImplementation subtraction (must have the same dimensions) */
MatrixImplementation MatrixImplementation::operator- (const MatrixImplementation & matrix) const
{
  if ((nbRows_ != matrix.nbRows_ ) || (nbColumns_ != matrix.nbColumns_ )) throw InvalidDimensionException(HERE) << "Cannot subtract matrices with incompatible dimensions";
  // Must copy as add will be overwritten by the operation
  MatrixImplementation result(*this);
  int size(nbRows_ * nbColumns_);
  double alpha(-1.0);
  int one(1);
  daxpy_(&size, &alpha, const_cast<double*>(&(matrix[0])), &one, &result[0], &one);

  return result;
}

/* In-place MatrixImplementation subtraction (must have the same dimensions) */
MatrixImplementation & MatrixImplementation::operator-= (const MatrixImplementation & matrix)
{
  if ((nbRows_ != matrix.nbRows_ ) || (nbColumns_ != matrix.nbColumns_ )) throw InvalidDimensionException(HERE) << "Cannot subtract matrices with incompatible dimensions";
  // Must copy as add will be overwritten by the operation
  MatrixImplementation result(*this);
  int size(nbRows_ * nbColumns_);
  double alpha(-1.0);
  int one(1);
  daxpy_(&size, &alpha, const_cast<double*>(&(matrix[0])), &one, &(*this)[0], &one);

  return *this;
}

/* MatrixImplementation multiplications (must have consistent dimensions) */
MatrixImplementation MatrixImplementation::genProd (const MatrixImplementation & matrix,
    const Bool transposeLeft,
    const Bool transposeRight) const
{
  // Multiply an mxk matrix op(A) by a kxn matrix op(B),
  // where op(A) == A if transposeLeft == false, op(A) == A^t else
  // and op(B) == B if transposeRight == false, op(B) == B^t else
  int m(transposeLeft ? nbColumns_ : nbRows_);
  int k(transposeLeft ? nbRows_ : nbColumns_);
  int l(transposeRight ? matrix.nbColumns_ : matrix.nbRows_);
  int n(transposeRight ? matrix.nbRows_ : matrix.nbColumns_);
  if (k != l) throw InvalidDimensionException(HERE) << "Invalid dimensions in matrix/matrix product left="
        << nbRows_ << "x" << nbColumns_
        << " right=" << matrix.nbRows_ << "x"
        << matrix.nbColumns_ << ", left is transposed=" << (transposeLeft ? "true" : "false") << ", right is transposed=" << (transposeRight ? "true" : "false");

  MatrixImplementation mult(m, n);
  if ((m == 0) || (n == 0) || (k == 0)) return mult;
  char transa(transposeLeft ? 'T' : 'N');
  char transb(transposeRight ? 'T' : 'N');
  double alpha(1.0);
  double beta(0.0);
  int ltransa(1);
  int ltransb(1);
  int lda(nbRows_);
  int ldb(matrix.nbRows_);
  dgemm_(&transa, &transb, &m, &n, &k, &alpha, const_cast<double*>(&((*this)[0])), &lda, const_cast<double*>(&(matrix[0])), &ldb, &beta, &mult[0], &m, &ltransa, &ltransb);

  return mult;
}

MatrixImplementation MatrixImplementation::symProd (const MatrixImplementation & matrix,
    const char symSide) const
{
  const MatrixImplementation & left = (symSide == 'L') ? *this : matrix;
  const MatrixImplementation & right = (symSide == 'L') ? matrix : *this;

  if (left.nbColumns_ != right.nbRows_) throw InvalidDimensionException(HERE) << "Invalid dimensions in matrix/matrix product"
        << " left=" << left.nbRows_ << "x" << left.nbColumns_
        << " right=" << right.nbRows_ << "x" << right.nbColumns_;

  MatrixImplementation mult(left.nbRows_, right.nbColumns_);
  if ((left.nbRows_ == 0) || (left.nbColumns_ == 0) || (right.nbRows_ == 0) || (right.nbColumns_ == 0)) return mult;

  char side = symSide;
  char uplo = 'L';
  int m = left.nbRows_;
  int n = right.nbColumns_;
  int lda = nbRows_;
  int ldb = matrix.nbRows_;
  double alpha = 1.0;
  double beta = 0.0;
  int lside = 1;
  int luplo = 1;
  dsymm_(&side, &uplo, &m, &n, &alpha, const_cast<double*>(&((*this)[0])), &lda, const_cast<double*>(&(matrix[0])), &ldb, &beta, &mult[0], &m, &lside, &luplo);
  return mult;
}

/* Multiplications with a Point (must have consistent dimensions) */
Point MatrixImplementation::genVectProd (const Point & pt, const Bool transposed) const
{
  int k = transposed ? nbRows_ : nbColumns_;
  if (k != (int)pt.getDimension()) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix/vector product: columns=" << k << " / vector dimension=" << pt.getDimension() << ".";

  int l = transposed ? nbColumns_ : nbRows_;
  Point prod(l);
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return prod;
  char trans = transposed ? 'T' : 'N';
  int one = 1;
  double alpha = 1.0;
  double beta = 0.0;
  int ltrans = 1;
  int m = nbRows_;
  int n = nbColumns_;

  dgemv_(&trans, &m, &n, &alpha, const_cast<double*>(&((*this)[0])), &m, const_cast<double*>(&(pt[0])), &one, &beta, &prod[0], &one, &ltrans);

  return prod;
}

/* Multiplications with a SampleImplementation (must have consistent dimensions)
   If side=='L', we compute this * S, otherwise we compute S * this */
Sample MatrixImplementation::genSampleProd (const Sample & sample, const Bool transposeMatrix, const Bool transposeSample, const char side) const
{
  // Sample and matrices are stored in reverse order, thus instead of computing
  // P = this * S, we compute P^T = S^T * this^T and S^T has a column-major order
  // as expected by dgemm.
  // Likewise when side is 'R', we compute P^T = this^T * S^T instead of P = S * this.
  const UnsignedInteger matrixRows(transposeMatrix ? nbColumns_ : nbRows_);
  const UnsignedInteger matrixColumns(transposeMatrix ? nbRows_ : nbColumns_);
  const UnsignedInteger sampleRows(transposeSample ? sample.getDimension() : sample.getSize());
  const UnsignedInteger sampleColumns(transposeSample ? sample.getSize() : sample.getDimension());
  if (side == 'L' && matrixColumns != sampleRows) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix*sample product: columns=" << matrixColumns << " / sample rows=" << sampleRows << ".";
  if (side == 'R' && matrixRows != sampleColumns) throw InvalidDimensionException(HERE) << "Invalid dimension in sample*matrix product: rows=" << matrixRows << " / sample columns=" << sampleColumns << ".";

  const UnsignedInteger prodRows(side == 'L' ? matrixRows : sampleRows);
  const UnsignedInteger prodColumns(side == 'L' ? sampleColumns : matrixColumns);
  Sample prod(prodRows, prodColumns);
  if (nbRows_ == 0 || nbColumns_ == 0) return prod;
  char transa = transposeSample ? 'T' : 'N';
  char transb = transposeMatrix ? 'N' : 'T';
  double alpha = 1.0;
  double beta = 0.0;
  int ltransa = 1;
  int ltransb = 1;
  if (side == 'L')
  {
    int m = sampleColumns;
    int n = matrixRows;
    int k = matrixColumns;
    int lda = (transa == 'N' ? m : k);
    int ldb = (transb == 'N' ? k : n);
    dgemm_(&transa, &transb, &m, &n, &k, &alpha, const_cast<double*>(&(sample(0, 0))), &lda, const_cast<double*>(&((*this)[0])), &ldb, &beta, &prod(0, 0), &m, &ltransa, &ltransb);
  }
  else
  {
    int m = matrixColumns;
    int n = sampleRows;
    int k = matrixRows;
    int lda = (transb == 'N' ? m : k);
    int ldb = (transa == 'N' ? k : n);
    dgemm_(&transb, &transa, &m, &n, &k, &alpha, const_cast<double*>(&((*this)[0])), &lda, const_cast<double*>(&(sample(0, 0))), &ldb, &beta, &prod(0, 0), &m, &ltransb, &ltransa);
  }

  return prod;
}

Point MatrixImplementation::symVectProd (const Point & pt) const
{
  if (nbColumns_ != pt.getDimension() ) throw InvalidDimensionException(HERE) << "Invalid dimension in matrix/vector product";

  Point prod(nbRows_);
  // In this case, nbRows_ == nbColumns_
  if (nbRows_ == 0) return prod;
  char uplo('L');
  int n(nbRows_);
  int one(1);
  double alpha(1.0);
  double beta(0.0);
  int luplo(1);
  dsymv_(&uplo, &n, &alpha, const_cast<double*>(&((*this)[0])), &n, const_cast<double*>(&(pt[0])), &one, &beta, &prod[0], &one, &luplo);

  return prod;
}

/* Gram matrix. If transpose == true, compute M^T.M, else M.M^T. */
MatrixImplementation MatrixImplementation::computeGram (const Bool transposed) const
{
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return MatrixImplementation(0, 0);
  char uplo = 'L';
  char trans = transposed ? 'T' : 'N';
  int n = transposed ? nbColumns_ : nbRows_;
  int k = transposed ? nbRows_ : nbColumns_;
  double alpha = 1.0;
  int lda = transposed ? k : n;
  double beta =  0.0;
  MatrixImplementation C(n, n);
  int ldc = n;
  int one = 1;
  dsyrk_(&uplo, &trans, &n, &k, &alpha, const_cast<double*>(&((*this)[0])), &lda, &beta, &C[0], &ldc, &one, &one);

  return C;
}

/* Multiplication by a Scalar */
MatrixImplementation MatrixImplementation::operator* (const Scalar s) const
{
  if (s == 0.0) return MatrixImplementation(nbRows_, nbColumns_);
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return *this;
  MatrixImplementation scalprod(*this);
  double alpha(s);
  int one(1);
  int n_(nbRows_ * nbColumns_);
  dscal_(&n_, &alpha, &scalprod[0], &one);

  return scalprod;
}

/* In-place Multiplication by a Scalar */
MatrixImplementation & MatrixImplementation::operator*= (const Scalar s)
{
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return *this;
  double alpha(s);
  int one(1);
  int n_(nbRows_ * nbColumns_);
  dscal_(&n_, &alpha, &(*this)[0], &one);

  return *this;
}

/* Division by a Scalar*/
MatrixImplementation MatrixImplementation::operator/ (const Scalar s) const
{
  if (!(s < 0.0 || s > 0.0)) throw InvalidArgumentException(HERE);
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return *this;
  MatrixImplementation scalprod(*this);
  double alpha(1.0 / s);
  int one(1);
  int n_(nbRows_ * nbColumns_);
  dscal_(&n_, &alpha, &scalprod[0], &one);

  return scalprod;
}

/* In-place division by a Scalar */
MatrixImplementation & MatrixImplementation::operator/= (const Scalar s)
{
  if (!(s < 0.0 || s > 0.0)) throw InvalidArgumentException(HERE);
  if ((nbRows_ == 0) || (nbColumns_ == 0)) return *this;
  double alpha(1.0 / s);
  int one(1);
  int n_(nbRows_ * nbColumns_);
  dscal_(&n_, &alpha, &(*this)[0], &one);

  return *this;
}

MatrixImplementation MatrixImplementation::triangularProd(const MatrixImplementation & matrix,
    const char triangularSide,
    const char upperLower) const
{
  if (nbColumns_ != matrix.nbRows_) throw InvalidDimensionException(HERE) << "Invalid dimensions in matrix/matrix product left="
        << nbRows_ << "x" << nbColumns_
        << " right=" << matrix.nbRows_ << "x"
        << matrix.nbColumns_;

  MatrixImplementation mult(matrix);
  if ((nbRows_ == 0) || (nbColumns_ == 0) || (matrix.nbColumns_ == 0)) return mult;
  char side(triangularSide);
  int lside = 1;
  char uplo(upperLower);
  int luplo = 1;
  char trans('N');
  int ltrans = 1;
  char diag('N');
  int ldiag = 1;
  int m(nbRows_);
  int n(matrix.nbColumns_);

  Scalar alpha = 1.0;

  // Lapack routine
  dtrmm_(&side, &uplo, &trans, &diag, &m, &n, &alpha, const_cast<double*>(&((*this)[0])),  &m, const_cast<double*>(&(mult[0])), &m, &lside, &luplo, &ltrans, &ldiag);
  return mult;
}

/* Integer power, general matrix */
MatrixImplementation MatrixImplementation::genPower(const UnsignedInteger n) const
{
  Bool first = true;
  UnsignedInteger exponent = n;
  MatrixImplementation y;
  MatrixImplementation z(*this);
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
MatrixImplementation MatrixImplementation::symPower(const UnsignedInteger n) const
{
  Bool first = true;
  UnsignedInteger exponent = n;
  MatrixImplementation y;
  MatrixImplementation z(*this);
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

/* Empty returns true if there is no element in the MatrixImplementation */
Bool MatrixImplementation::isEmpty() const
{
  return ((nbRows_ == 0)  || (nbColumns_ == 0) || (PersistentCollection<Scalar>::isEmpty()));
}

/* Returns true if triangular lower or upper */
Bool MatrixImplementation::isTriangular(Bool lower) const
{
  if ( nbRows_ == nbColumns_ )
  {
    for ( UnsignedInteger j = 1; j < nbColumns_; ++ j )
      for ( UnsignedInteger i = 0; i < j; ++ i )
        if ( std::abs( (*this)[lower ?  convertPosition(i, j) : convertPosition(j, i)] ) > 0. )
          return false;
    return true;
  }
  else
    return false;
}

/* Comparison operator */
Bool MatrixImplementation::operator == (const MatrixImplementation & rhs) const
{
  const MatrixImplementation &lhs(*this);
  Bool equality = true;

  if (&lhs != &rhs)   // Not the same object
  {
    const Collection<Scalar> & refLhs(static_cast<const Collection<Scalar> >(lhs));
    const Collection<Scalar> & refRhs(static_cast<const Collection<Scalar> >(rhs));
    equality = ( lhs.nbRows_ == rhs.nbRows_ && lhs.nbColumns_ == rhs.nbColumns_ && refLhs == refRhs);
  }

  return equality;
}


Bool MatrixImplementation::isSymmetric() const
{
  const Scalar epsilon = ResourceMap::GetAsScalar("Matrix-SymmetryThreshold");
  if ( nbRows_ == nbColumns_ )
  {
    for ( UnsignedInteger i = 1; i < nbRows_; ++ i )
      for ( UnsignedInteger j = 0; j < i; ++ j )
        if ( std::abs(this->operator[](convertPosition(i, j)) - operator[](convertPosition(j, i))) > epsilon)
          return false;
    return true;
  }
  else
    return false;
}


/* Copy the lower triangle into the upper one, used by symmetric matrices */
void MatrixImplementation::symmetrize() const
{
  MatrixImplementation *refThis(const_cast<MatrixImplementation *>(this));
  // Loop over the upper triangle
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < j; ++i)
      refThis->operator[](convertPosition(i, j)) = operator[](convertPosition(j, i));
}

/* Fill the relevant triangle with zero */
void MatrixImplementation::triangularize(const Bool isLowerTriangular) const
{
  MatrixImplementation *refThis(const_cast<MatrixImplementation *>(this));
  if (isLowerTriangular)
  {
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      for (UnsignedInteger i = 0; i < j; ++i)
        refThis->operator[](convertPosition(i, j)) = 0.0;
  }
  else
  {
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
      for (UnsignedInteger i = j + 1; i < nbRows_; ++i)
        refThis->operator[](convertPosition(i, j)) = 0.0;
  }
}

/* Check if the matrix values belong to (-1;1) */
Bool MatrixImplementation::hasUnitRange() const
{
  bool unitRange = true;
  for (UnsignedInteger i = 0; i < nbRows_; ++i)
    for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    {
      if (std::abs(this->operator[](convertPosition(i, j))) > 1.0)
      {
        unitRange = false;
        break;
      }
    }
  return unitRange;
}

/* Set small elements to zero */
MatrixImplementation MatrixImplementation::clean(const Scalar threshold) const
{
  // Nothing to do for nonpositive threshold
  if (threshold <= 0.0) return *this;
  MatrixImplementation result(nbRows_, nbColumns_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++j)
    for (UnsignedInteger i = 0; i < nbRows_; ++i)
    {
      const Scalar value = (*this)(i, j);
      // Things are done this way to prevent spurious -0.0
      if (std::abs(value) < 0.5 * threshold) result(i, j) = 0.0;
      else result(i, j) = threshold * round(value / threshold);
    }
  return result;
}

/* Set small elements to zero */
MatrixImplementation MatrixImplementation::cleanSym(const Scalar threshold) const
{
  symmetrize();
  return clean(threshold);
}

/* Resolution of a linear system : rectangular matrix
 * MX = b, M is an mxn matrix, b is an mxq matrix and
 * X is an nxq matrix */
MatrixImplementation MatrixImplementation::solveLinearSystemRect (const MatrixImplementation & b,
    const Bool keepIntact)
{
  if (nbRows_ != b.nbRows_) throw InvalidDimensionException(HERE) << "The right-hand side has row dimension=" << b.nbRows_ << ", expected " << nbRows_;
  if (!(nbRows_ > 0 && nbColumns_ > 0 && b.nbColumns_ > 0)) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix or empty right-hand side";
  int m(nbRows_);
  int n(nbColumns_);
  // B is an extended copy of b, it must be large enough to store the solution, see LAPACK documentation
  int p(std::max(m, n));
  int q(b.nbColumns_);
  MatrixImplementation B(p, q);
  for(UnsignedInteger j = 0; j < static_cast<UnsignedInteger>(q); ++j)
    for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(m); ++i)
      B(i, j) = b(i, j);
  int nrhs(q);
  int lwork(-1);
  double lwork_d = -1.;
  int info = -1;
  std::vector<int> jpiv(n);
  double rcond(ResourceMap::GetAsScalar("Matrix-DefaultSmallPivot"));
  int rank = -1;

  MatrixImplementation Q;
  if (keepIntact) Q = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? Q : *this;

  dgelsy_(&m, &n, &nrhs, &A[0], &m, &B[0], &p, &jpiv[0], &rcond, &rank, &lwork_d, &lwork, &info);
  lwork = static_cast<int>(lwork_d);
  Point work(lwork);
  dgelsy_(&m, &n, &nrhs, &A[0], &m, &B[0], &p, &jpiv[0], &rcond, &rank, &work[0], &lwork, &info);

  MatrixImplementation result(n, q);
  for(UnsignedInteger j = 0; j < static_cast<UnsignedInteger>(q); ++j)
    for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(n); ++i)
      result(i, j) = B(i, j);
  return result;
}

/* Resolution of a linear system : rectangular matrix
 * Mx = b, M is an mxn matrix, b is an m-dimensional
 * vector and x is an n-dimensional vector */
Point MatrixImplementation::solveLinearSystemRect (const Point & b,
    const Bool keepIntact)
{
  const UnsignedInteger m = b.getDimension();
  if (nbRows_ != m) throw InvalidDimensionException(HERE) << "The right-hand side dimension is " << m << ", expected " << nbRows_;
  if (nbRows_ == 0) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix";
  // Solve the matrix linear system
  // A MatrixImplementation is also a collection of Scalar, so it is automatically converted into a Point
  return solveLinearSystemRect(MatrixImplementation(m, 1, b), keepIntact);
}


/* Resolution of a linear system : square matrix */
MatrixImplementation MatrixImplementation::solveLinearSystemTri (const MatrixImplementation & b,
    const Bool keepIntact,
    const Bool lower,
    const Bool transposed)
{
  if (nbRows_ != b.nbRows_ ) throw InvalidDimensionException(HERE) << "The right-hand side has row dimension=" << b.nbRows_ << ", expected " << nbRows_;
  if (!(nbRows_ > 0 && nbColumns_ > 0 && b.nbColumns_ > 0)) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix or empty right-hand side";

  // We must copy the right-hand side because it will be overwritten by the operation
  MatrixImplementation B(b);
  // side tells if we solve M.X = alpha.B or X.M = alpha.B
  char side = 'L';
  int lside = 1;
  // M must be triangular. uplo tells if it is upper or lower triangular
  char uplo = lower ? 'L' : 'U';
  int luplo = 1;
  // transa tells if M is transposed or not
  char transa = transposed ? 'T' : 'N';
  int ltransa = 1;
  // diag tells if M is unit diagonal or not
  char diag = 'N';
  int ldiag = 1;
  // the row dimension of M
  int m = B.nbRows_;
  // the column dimension of M
  int n = B.nbColumns_;
  // we solve the case alpha=1
  double alpha = 1.0;
  // leading dimension of M
  int lda = nbRows_;
  // leading dimension of B
  int ldb = b.nbRows_;
  if (keepIntact)
  {
    MatrixImplementation A(*this);
    dtrsm_(&side, &uplo, &transa, &diag, &m, &n, &alpha, const_cast<double*>(&(A[0])), &lda, const_cast<double*>(&(B[0])), &ldb, &lside, &luplo, &ltransa, &ldiag);
  }
  else dtrsm_(&side, &uplo, &transa, &diag, &m, &n, &alpha, const_cast<double*>(&((*this)[0])), &lda, const_cast<double*>(&(B[0])), &ldb, &lside, &luplo, &ltransa, &ldiag);
  return B;
}

/* Resolution of a linear system : square matrix */
Point MatrixImplementation::solveLinearSystemTri (const Point & b,
    const Bool keepIntact,
    const Bool lower,
    const Bool transposed)
{
  const UnsignedInteger m = b.getDimension();
  if (nbRows_ != m) throw InvalidDimensionException(HERE) << "The right-hand side dimension is " << m << ", expected " << nbRows_;
  if (nbRows_ == 0) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix";
  // A MatrixImplementation is also a collection of Scalar, so it is automatically converted into a Point
  return solveLinearSystemTri(MatrixImplementation(m, 1, b), keepIntact, lower, transposed);
}



/* Resolution of a linear system : square matrix */
MatrixImplementation MatrixImplementation::solveLinearSystemSquare (const MatrixImplementation & b,
    const Bool keepIntact)
{
  if (nbColumns_ != b.nbRows_ ) throw InvalidDimensionException(HERE) << "The right-hand side has row dimension=" << b.nbRows_ << ", expected " << nbRows_;
  if (!(nbRows_ > 0 && nbColumns_ > 0 && b.nbColumns_ > 0)) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix or empty right-hand side";

  // We must copy the right-hand side because it will be overwritten by the operation
  MatrixImplementation B(b);
  int m(nbRows_);
  int nrhs(B.nbColumns_);
  int info;
  std::vector<int> ipiv(m);
  if (keepIntact)
  {
    MatrixImplementation A(*this);
    dgesv_(&m, &nrhs, &A[0], &m, &ipiv[0], &B[0], &m, &info);
  }
  else dgesv_(&m, &nrhs, &(*this)[0], &m, &ipiv[0], &B[0], &m, &info);
  if (info != 0) throw NotDefinedException(HERE) << "Error: the matrix is singular.";
  return B;
}

/* Resolution of a linear system : square matrix */
Point MatrixImplementation::solveLinearSystemSquare (const Point & b,
    const Bool keepIntact)
{
  const UnsignedInteger m = b.getDimension();
  if (nbRows_ != m) throw InvalidDimensionException(HERE) << "The right-hand side dimension is " << m << ", expected " << nbRows_;
  if (nbRows_ == 0) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix";
  // A MatrixImplementation is also a collection of Scalar, so it is automatically converted into a Point
  return solveLinearSystemRect(MatrixImplementation(m, 1, b), keepIntact);
}

/* Resolution of a linear system : symmetric matrix */
MatrixImplementation MatrixImplementation::solveLinearSystemSym (const MatrixImplementation & b,
    const Bool keepIntact)
{
  if (nbColumns_ != b.nbRows_ ) throw InvalidDimensionException(HERE) << "The right-hand side has row dimension=" << b.nbRows_ << ", expected " << nbRows_;
  if (!(nbRows_ > 0 && nbColumns_ > 0 && b.nbColumns_ > 0)) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix or empty right-hand side";

  char uplo('L');
  // We must copy the right-hand side as it will be overwritten by the operation
  MatrixImplementation B(b);
  int n(nbRows_);
  int nrhs(B.nbColumns_);
  int lwork(-1);
  double lwork_d = -1.;
  int info = -1;
  std::vector<int> ipiv(n);
  int luplo(1);

  MatrixImplementation Q;
  if (keepIntact) Q = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? Q : *this;

  dsysv_(&uplo, &n, &nrhs, &A[0], &n, &ipiv[0], &B[0], &n, &lwork_d, &lwork, &info, &luplo);
  lwork = static_cast<int>(lwork_d);
  Point work(lwork);
  dsysv_(&uplo, &n, &nrhs, &A[0], &n, &ipiv[0], &B[0], &n, &work[0], &lwork, &info, &luplo);

  if (info != 0) throw NotDefinedException(HERE) << "Error: the matrix is singular.";
  return B;
}

/* Resolution of a linear system : symmetric matrix */
Point MatrixImplementation::solveLinearSystemSym (const Point & b,
    const Bool keepIntact)
{
  const UnsignedInteger dimension = b.getDimension();
  if (nbRows_ != dimension) throw InvalidDimensionException(HERE) << "The right-hand side dimension is " << dimension << ", expected " << nbRows_;
  if (nbRows_ == 0) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix";
  MatrixImplementation B(dimension, 1, b);
  // A MatrixImplementation is also a collection of Scalar, so it is automatically converted into a Point
  return solveLinearSystemSym(B, keepIntact);
}

/* Resolution of a linear system : covariance matrix */
MatrixImplementation MatrixImplementation::solveLinearSystemCov (const MatrixImplementation & b,
    const Bool keepIntact)
{
  if (nbRows_ != b.nbRows_ ) throw InvalidDimensionException(HERE) << "The right-hand side has row dimension=" << b.nbRows_ << ", expected " << nbRows_;
  if (!(nbRows_ > 0 && nbColumns_ > 0 && b.nbColumns_ > 0)) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix or empty right-hand side";

  char uplo('L');
  // We must copy the right-hand side as it will be overwritten by the operation
  MatrixImplementation B(b);
  int n(nbRows_);
  int nrhs(B.nbColumns_);
  int info;
  int luplo(1);
  if (keepIntact)
  {
    MatrixImplementation A(*this);
    dposv_(&uplo, &n, &nrhs, &A[0], &n, &B[0], &n, &info, &luplo);
  }
  else
  {
    dposv_(&uplo, &n, &nrhs, &(*this)[0], &n, &B[0], &n, &info, &luplo);
  }
  if (info != 0) throw NotDefinedException(HERE) << "Error: the matrix is singular.";
  return B;
}

/* Resolution of a linear system : symmetric matrix */
Point MatrixImplementation::solveLinearSystemCov (const Point & b,
    const Bool keepIntact)
{
  const UnsignedInteger dimension = b.getDimension();
  if (nbRows_ != dimension) throw InvalidDimensionException(HERE) << "The right-hand side dimension is " << dimension << ", expected " << nbRows_;
  if (nbRows_ == 0) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix";
  MatrixImplementation B(dimension, 1, b);
  // A MatrixImplementation is also a collection of Scalar, so it is automatically converted into a Point
  return solveLinearSystemCov(B, keepIntact);
}

/* Compute determinant */
Scalar MatrixImplementation::computeLogAbsoluteDeterminant (Scalar & sign,
    const Bool keepIntact)
{
  int n(nbRows_);
  if (!(n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the determinant of an empty matrix";
  Scalar logAbsoluteDeterminant = 0.0;
  sign = 1.0;
  if (n <= 2)
  {
    Scalar value = 0.0;
    if (n == 1) value = (*this)(0, 0);
    else value = (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
    if (value == 0.0)
    {
      sign = 0.0;
      logAbsoluteDeterminant = SpecFunc::LowestScalar;
    }
    else
    {
      sign = (value > 0.0 ? 1.0 : -1.0);
      logAbsoluteDeterminant = log(std::abs(value));
    }
  } // n <= 2
  else
  {
    std::vector<int> ipiv (n);
    int info = -1;

    MatrixImplementation Q;
    if (keepIntact) Q = MatrixImplementation(*this);
    MatrixImplementation & A = keepIntact ? Q : *this;

    // LU Factorization with LAPACK
    dgetrf_(&n, &n, &A[0], &n, &ipiv[0], &info);
    if (info > 0) return SpecFunc::LowestScalar;
    // Determinant computation
    for (UnsignedInteger i = 0; i < ipiv.size(); ++i)
    {
      const Scalar pivot = A[i * (ipiv.size() + 1)];
      if (std::abs(pivot) == 0.0)
      {
        logAbsoluteDeterminant = SpecFunc::LowestScalar;
        sign = 0.0;
      }
      else
      {
        logAbsoluteDeterminant += log(std::abs(pivot));
      }
      if (pivot < 0.0) sign = -sign;
      if (ipiv[i] != int(i + 1)) sign = -sign;
    }
  } // n > 2
  return logAbsoluteDeterminant;
}

/* Compute determinant */
Scalar MatrixImplementation::computeDeterminant (const Bool keepIntact)
{
  if (nbRows_ == 1) return (*this)(0, 0);
  if (nbRows_ == 2) return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
  Scalar sign = 0.0;
  const Scalar logAbsoluteDeterminant = computeLogAbsoluteDeterminant(sign, keepIntact);
  if (logAbsoluteDeterminant <= SpecFunc::LowestScalar) return 0.0;
  return sign * exp(logAbsoluteDeterminant);
}

/* Compute determinant for a symmetric matrix */
Scalar MatrixImplementation::computeLogAbsoluteDeterminantSym (Scalar & sign,
    const Bool keepIntact)
{
  symmetrize();
  return computeLogAbsoluteDeterminant(sign, keepIntact);
  /* The implementation based on dsytrf does not uses the 2x2 diagonal blocks correctly
     int n(nbRows_);
     if (n == 0) throw InvalidDimensionException(HERE);
     std::vector<int> ipiv (n);
     char uplo('L');
     int info;
     Scalar determinant = 1.0;
     int lwork(-1);
     double lwork_d;
     int luplo(1);

     // LU Factorization with LAPACK
     if (keepIntact)
     {
     MatrixImplementation A(*this);
     dsytrf_(&uplo, &n, &A[0], &n, &ipiv[0],&lwork_d, &lwork, &info, &luplo);
     lwork = static_cast<int>(lwork_d);
     Point work(lwork);
     dsytrf_(&uplo, &n, &A[0], &n, &ipiv[0],&work[0], &lwork, &info, &luplo);
     // Determinant computation
     for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(n); ++i)
     {
     determinant *= A[i * (ipiv.size() + 1)];
     if (ipiv[i] != int(i + 1)) determinant = -determinant;
     }
     }
     else
     {
     dsytrf_(&uplo, &n, &(*this)[0], &n, &ipiv[0],&lwork_d, &lwork, &info, &luplo);
     lwork = static_cast<int>(lwork_d);
     Point work(lwork);
     dsytrf_(&uplo, &n, &(*this)[0], &n, &ipiv[0],&work[0], &lwork, &info, &luplo);
     // Determinant computation
     for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(n); ++i)
     {
     determinant *= (*this)[i * (ipiv.size() + 1)];
     if (ipiv[i] != int(i + 1)) determinant = -determinant;
     }
     }

     return determinant; */
}

/* Compute determinant for a symmetric matrix */
Scalar MatrixImplementation::computeDeterminantSym (const Bool keepIntact)
{
  if (nbRows_ == 1) return (*this)(0, 0);
  if (nbRows_ == 2) return (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(1, 0);
  Scalar sign = 0.0;
  const Scalar logAbsoluteDeterminant = computeLogAbsoluteDeterminant(sign, keepIntact);
  if (logAbsoluteDeterminant <= SpecFunc::LowestScalar) return 0.0;
  return sign * exp(logAbsoluteDeterminant);
}

/* Compute trace */
Scalar MatrixImplementation::computeTrace() const
{
  Scalar trace = 0.0;
  for (UnsignedInteger i = 0; i < nbRows_; ++i) trace += (*this)(i, i);
  return trace;
}

/* Compute the eigenvalues of a square matrix */
MatrixImplementation::ComplexCollection MatrixImplementation::computeEigenValuesSquare (const Bool keepIntact)
{
  int n(nbRows_);
  if (!(n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the eigenvalues of an empty matrix";
  char jobvl('N');
  char jobvr('N');
  Point wr(n, 0.0);
  Point wi(n, 0.0);
  double vl = 0.;
  double vr = 0.;
  int ldvl(1);
  int ldvr(1);
  int lwork(-1);
  double lwork_d = -1.;
  int info = -1;
  int ljobvl(1);
  int ljobvr(1);

  MatrixImplementation Q;
  if (keepIntact) Q = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? Q : *this;

  dgeev_(&jobvl, &jobvr, &n, &A[0], &n, &wr[0], &wi[0], &vl, &ldvl, &vr, &ldvr, &lwork_d, &lwork, &info, &ljobvl, &ljobvr);
  lwork = static_cast<int>(lwork_d);
  Point work(lwork);
  dgeev_(&jobvl, &jobvr, &n, &A[0], &n, &wr[0], &wi[0], &vl, &ldvl, &vr, &ldvr, &work[0], &lwork, &info, &ljobvl, &ljobvr);

  if (info != 0) throw InternalException(HERE) << "Error: the QR algorithm failed to converge.";
  ComplexCollection eigenValues(n);
  for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(n); ++i) eigenValues[i] = Complex(wr[i], wi[i]);
  return eigenValues;
}

MatrixImplementation::ComplexCollection MatrixImplementation::computeEVSquare (ComplexMatrixImplementation & v,
    const Bool keepIntact)
{
  int n(nbRows_);
  if (!(n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the eigenvalues of an empty matrix";
  char jobvl('N');
  char jobvr('V');
  Point wr(n, 0.0);
  Point wi(n, 0.0);
  double vl;
  MatrixImplementation vr(n, n);
  int ldvl(1);
  int ldvr(n);
  int lwork(-1);
  double lwork_d;
  int info;
  int ljobvl(1);
  int ljobvr(1);

  MatrixImplementation Q;
  if (keepIntact) Q = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? Q : *this;

  dgeev_(&jobvl, &jobvr, &n, &A[0], &n, &wr[0], &wi[0], &vl, &ldvl, &vr[0], &ldvr, &lwork_d, &lwork, &info, &ljobvl, &ljobvr);
  lwork = static_cast<int>(lwork_d);
  Point work(lwork);
  dgeev_(&jobvl, &jobvr, &n, &A[0], &n, &wr[0], &wi[0], &vl, &ldvl, &vr[0], &ldvr, &work[0], &lwork, &info, &ljobvl, &ljobvr);

  // Cast the eigenvalues into OpenTURNS data structures
  ComplexCollection eigenValues(n);
  for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(n); ++i)
  {
    eigenValues[i] = Complex(wr[i], wi[i]);
  }
  if (info != 0) throw InternalException(HERE) << "Error: the QR algorithm failed to converge.";
  // Cast the eigenvectors into OpenTURNS data structures
  v = ComplexMatrixImplementation(n, n);
  UnsignedInteger j = 0;
  while (j < static_cast<UnsignedInteger>(n))
  {
    // Real eigenvalue
    if (wi[j] == 0.0)
    {
      for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(n); ++i) v(i, j) = vr(i, j);
      ++j;
    }
    // Complex conjugate pair of eigenvalues
    else
    {
      for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(n); ++i)
      {
        v(i, j)     = Complex(vr(i, j),  vr(i, j + 1));
        v(i, j + 1) = Complex(vr(i, j), -vr(i, j + 1));
      }
      j += 2;
    }
  } // Conversion of eigenvectors
  return eigenValues;
}

/* Compute the eigenvalues of a symmetric matrix */
Point MatrixImplementation::computeEigenValuesSym (const Bool keepIntact)
{
  int n(nbRows_);
  if (!(n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the eigenvalues of an empty matrix";
  char jobz('N');
  char uplo('L');
  Point w(n, 0.0);
  int lwork(-1);
  double lwork_d;
  int info;
  int ljobz(1);
  int luplo(1);

  MatrixImplementation Q;
  if (keepIntact) Q = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? Q : *this;

  dsyev_(&jobz, &uplo, &n, &A[0], &n, &w[0], &lwork_d, &lwork, &info, &ljobz, &luplo);
  lwork = static_cast<int>(lwork_d);
  Point work(lwork);
  dsyev_(&jobz, &uplo, &n, &A[0], &n, &w[0], &work[0], &lwork, &info, &ljobz, &luplo);

  if (info != 0) throw InternalException(HERE) << "Error: the QR algorithm failed to converge.";
  return w;
}

Point MatrixImplementation::computeEVSym (MatrixImplementation & v,
    const Bool keepIntact)
{
  int n(nbRows_);
  if (!(n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the eigenvalues of an empty matrix";
  char jobz('V');
  char uplo('L');
  Point w(n, 0.0);
  int lwork(-1);
  double lwork_d;
  int info;
  int ljobz(1);
  int luplo(1);

  MatrixImplementation Q;
  if (keepIntact) Q = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? Q : *this;

  dsyev_(&jobz, &uplo, &n, &A[0], &n, &w[0], &lwork_d, &lwork, &info, &ljobz, &luplo);
  lwork = static_cast<int>(lwork_d);
  Point work(lwork);
  dsyev_(&jobz, &uplo, &n, &A[0], &n, &w[0], &work[0], &lwork, &info, &ljobz, &luplo);
  v = A;

  if (info != 0) throw InternalException(HERE) << "Error: the QR algorithm failed to converge.";
  return w;
}

/* Compute the largest eigenvalue module using power iterations, square matrix */
Bool MatrixImplementation::computeLargestEigenValueModuleSquare(Scalar & maximumModule,
    const UnsignedInteger maximumIterations,
    const Scalar epsilon) const
{
  const UnsignedInteger dimension = getNbRows();
  Point currentEigenVector(dimension, 1.0);
  Point nextEigenVector(genVectProd(currentEigenVector));
  Scalar nextEigenValue = nextEigenVector.norm();
  maximumModule = nextEigenValue / std::sqrt(1.0 * dimension);
  Bool found = false;
  Scalar precision = 0.0;
  for (UnsignedInteger iteration = 0; iteration < maximumIterations && !found; ++iteration)
  {
    LOGDEBUG(OSS() << "(" << iteration << ") maximum module=" << maximumModule);
    currentEigenVector = nextEigenVector / nextEigenValue;
    nextEigenVector = genVectProd(currentEigenVector);
    nextEigenValue = nextEigenVector.norm();
    precision = std::abs(nextEigenValue - maximumModule);
    found = precision <= epsilon * nextEigenValue;
    LOGDEBUG(OSS() << "(" << iteration << ") precison=" << precision << ", relative precision=" << precision / nextEigenValue << ", found=" << found);
    maximumModule = nextEigenValue;
  }
  return found;
}

/* Compute the largest eigenvalue module using power iterations, symmetric matrix */
Bool MatrixImplementation::computeLargestEigenValueModuleSym(Scalar & maximumModule,
    const UnsignedInteger maximumIterations,
    const Scalar epsilon) const
{
  const UnsignedInteger dimension = getNbRows();
  Point currentEigenVector(dimension, 1.0);
  Point nextEigenVector(symVectProd(currentEigenVector));
  Scalar nextEigenValue = nextEigenVector.norm();
  maximumModule = nextEigenValue / std::sqrt(1.0 * dimension);
  Bool found = false;
  Scalar precision = 0.0;
  for (UnsignedInteger iteration = 0; iteration < maximumIterations && !found; ++iteration)
  {
    LOGDEBUG(OSS() << "(" << iteration << ") maximum module=" << maximumModule);
    currentEigenVector = nextEigenVector / nextEigenValue;
    nextEigenVector = symVectProd(currentEigenVector);
    nextEigenValue = nextEigenVector.norm();
    precision = std::abs(nextEigenValue - maximumModule);
    found = precision <= epsilon * nextEigenValue;
    LOGDEBUG(OSS() << "(" << iteration << ") precison=" << precision << ", relative precision=" << precision / nextEigenValue << ", found=" << found);
    maximumModule = nextEigenValue;
  }
  return found;
}

/* Compute the singular values of a matrix */
Point MatrixImplementation::computeSingularValues(const Bool keepIntact)
{
  int m = nbRows_;
  int n = nbColumns_;
  if (!(m > 0 && n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the singular values of an empty matrix";

  // check for nans, cf https://github.com/Reference-LAPACK/lapack/issues/469
  for (UnsignedInteger j = 0; j < getNbColumns(); ++ j)
    for (UnsignedInteger i = 0; i < getNbRows(); ++ i)
      if (!SpecFunc::IsNormal(operator()(i, j)))
        throw InvalidArgumentException(HERE) << "Cannot compute singular values due to nan/inf values";

  char jobz = 'N';
  Point S(std::min(m, n), 0.0);
  Point work(1, 0.0);
  MatrixImplementation u(1, 1);
  int ldu = 1;
  int ldvt = 1;
  MatrixImplementation vT(1, 1);
  int lwork = -1;
  std::vector<int> iwork(8 * std::min(m, n));
  int info = 0;
  int ljobz = 1;

  MatrixImplementation Q;
  if (keepIntact) Q = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? Q : *this;

  // First call to compute the optimal work size
  dgesdd_(&jobz, &m, &n, &A[0], &m, &S[0], &u[0], &ldu, &vT[0], &ldvt, &work[0], &lwork, &iwork[0], &info, &ljobz);
  lwork = static_cast<int>(work[0]);
  work = Point(lwork, 0.0);
  // Second call to compute the SVD
  dgesdd_(&jobz, &m, &n, &A[0], &m, &S[0], &u[0], &ldu, &vT[0], &ldvt, &work[0], &lwork, &iwork[0], &info, &ljobz);

  if (info != 0) throw InternalException(HERE) << "Error: the updating process failed.";
  return S;
}

/* Compute the singular values and singular decomposition of a matrix */
Point MatrixImplementation::computeSVD(MatrixImplementation & u,
                                       MatrixImplementation & vT,
                                       const Bool fullSVD,
                                       const Bool keepIntact)
{
  int m(nbRows_);
  int n(nbColumns_);
  if (!(m > 0 && n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the singular values decomposition of an empty matrix";
  char jobz( fullSVD ? 'A' : 'S');
  int ldu(m);
  u = MatrixImplementation(m, ( fullSVD ? m : std::min(m, n)));
  int ldvt = (fullSVD ? n : std::min(m, n));
  vT = MatrixImplementation(( fullSVD ? n : std::min(m, n)), n);
  Point S(std::min(m, n), 0.0);
  Point work(1, 0.0);
  int lwork(-1);
  std::vector<int> iwork(8 * std::min(m, n));
  int info(0);
  int ljobz(1);

  MatrixImplementation Q;
  if (keepIntact) Q = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? Q : *this;

  // First call to compute the optimal work size
  dgesdd_(&jobz, &m, &n, &A[0], &m, &S[0], &u[0], &ldu, &vT[0], &ldvt, &work[0], &lwork, &iwork[0], &info, &ljobz);
  lwork = static_cast<int>(work[0]);
  work = Point(lwork, 0.0);
  // Second call to compute the SVD
  dgesdd_(&jobz, &m, &n, &A[0], &m, &S[0], &u[0], &ldu, &vT[0], &ldvt, &work[0], &lwork, &iwork[0], &info, &ljobz);

  if (info != 0) throw InternalException(HERE) << "Error: LAPACK trouble in computing SVD decomposition, return code is " << info;
  return S;
}


/* Check if the matrix is SPD */
Bool MatrixImplementation::isPositiveDefinite() const
{
  int info;
  int n(nbRows_);
  if (!(n > 0)) throw InvalidDimensionException(HERE) << "Cannot check the definite positiveness of an empty matrix";
  char uplo('L');
  int luplo(1);
  MatrixImplementation A(*this);
  dpotrf_(&uplo, &n, &A[0], &n, &info, &luplo);
  return (info == 0) ;
}

MatrixImplementation::ScalarCollection MatrixImplementation::triangularVectProd(const ScalarCollection & pt,
    const char side,
    const Bool transpose) const
{
  char uplo = side;
  int luplo = 1;

  // trans tells if the matrix is transposed or not
  char trans = transpose ? 'T' : 'N';
  int ltrans = 1;

  // diag tells if M is unit diagonal or not
  char diag = 'N';
  int ldiag = 1;

  // the dimension of the matrix
  int n = nbRows_;

  // leading dimension of M
  int lda = nbRows_;
  int one = 1;

  ScalarCollection x(nbRows_);
  for (UnsignedInteger i = 0; i < pt.getSize(); ++i) x[i] = pt[i];

  dtrmv_(&uplo, &trans, &diag, &n, const_cast<double*>(&((*this)[0])), &lda, const_cast<double*>(&(x[0])), &one, &luplo, &ltrans, &ldiag);
  return x;
}

MatrixImplementation::ScalarCollection MatrixImplementation::triangularVectProd(const Point & pt,
    const char side,
    const Bool transpose) const
{
  return triangularVectProd(pt.getCollection(), side, transpose);
}

/* Build the Cholesky factorization of the matrix */
MatrixImplementation MatrixImplementation::computeCholesky(const Bool keepIntact)
{
  int n = nbRows_;
  if (!(n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the Cholesky decomposition of an empty matrix";
  int info = 0;
  char uplo = 'L';
  int luplo = 1;

  MatrixImplementation L;
  if (keepIntact) L = MatrixImplementation(*this);
  MatrixImplementation & A = keepIntact ? L : *this;

  dpotrf_(&uplo, &n, &A[0], &n, &info, &luplo);
  if (info != 0) throw NotSymmetricDefinitePositiveException(HERE) << "Error: the matrix is not definite positive.";
  for (UnsignedInteger j = 0; j < (UnsignedInteger)(n); ++ j)
    for (UnsignedInteger i = 0; i < j; ++ i)
      A(i, j) = 0.0;
  A.setName("");
  return A;
}

/* Update in-place the Cholesky factor L of an SPD matrix M given a rank-one update vv^T, ie L becomes Lnew such that LnewLnew^t = Mnew with Mnew = M + vv^t */
void MatrixImplementation::CholeskyUpdate(MatrixImplementation & cholesky,
    const Point & vector)
{
  UnsignedInteger dimension = cholesky.nbRows_;
  if (dimension != cholesky.nbColumns_) throw InvalidDimensionException(HERE) << "Cannot update a non-square Cholesky factor";
  if (dimension != vector.getDimension()) throw InvalidDimensionException(HERE) << "Incompatible Cholesky factor dimension and vector dimension";
  // Working copy of vector
  Point work(vector);
  int size = dimension;
  int one = 1;
  UnsignedInteger shift = 0;
  // Parameters of the Givens rotation
  double cosI = 0.;
  double sinI = 0.;
  for (UnsignedInteger i = 0; i < dimension - 1; ++i)
  {
    // Generate Givens rotation
    drotg_(&cholesky[shift], &work[i], &cosI, &sinI);
    // Flip rotation if negative diagonal entry
    if (cholesky[shift] < 0.0)
    {
      cholesky[shift] = -cholesky[shift];
      cosI = -cosI;
      sinI = -sinI;
    }
    --size;
    // Perform the rotation
    drot_(&size, &cholesky[shift + 1], &one, &work[i + 1], &one, &cosI, &sinI);
    shift += dimension + 1;
  }
  // Last rotation
  // Generate Givens rotation
  drotg_(&cholesky[shift], &work[dimension - 1], &cosI, &sinI);
  // Flip rotation if negative diagonal entry
  if (cholesky[shift] < 0.0) cholesky[shift] = -cholesky[shift];
}

/* Downdate in-place the Cholesky factor L of an SPD matrix M given a rank-one downdate vv^T, ie L becomes Lnew such that LnewLnew^t = Mnew with Mnew = M - vv^t */
void MatrixImplementation::CholeskyDowndate(MatrixImplementation & cholesky,
    const Point & vector)
{
  UnsignedInteger dimension = cholesky.nbRows_;
  if (dimension != cholesky.nbColumns_) throw InvalidDimensionException(HERE) << "Cannot update a non-square Cholesky factor";
  if (dimension != vector.getDimension()) throw InvalidDimensionException(HERE) << "Incompatible Cholesky factor dimension and vector dimension";
  // Working copy of vector
  Point work(cholesky.solveLinearSystemTri(vector));
  // Parameters of the Givens rotation
  Scalar qs = sqrt(1.0 - work.normSquare());
  Point cosI(dimension);
  Point sinI(dimension);
  for (SignedInteger i = dimension - 1; i >= 0; --i)
  {
    // Generate Givens rotation
    drotg_(&qs, &work[i], &cosI[i], &sinI[i]);
    // Flip rotation if negative diagonal entry
    if (qs < 0.0)
    {
      qs = -qs;
      cosI[i] = -cosI[i];
      sinI[i] = -sinI[i];
    }
  }
  work = Point(dimension, 0.0);
  int size(0);
  int one(1);
  UnsignedInteger shift = (dimension - 1) * (dimension + 1);
  for (SignedInteger i = dimension - 1; i >= 0; --i)
  {
    ++size;
    // Perform the rotation
    drot_(&size, &work[i], &one, &cholesky[shift], &one, &cosI[i], &sinI[i]);
    if (cholesky[shift] < 0.0)
    {
      qs = -1.0;
      dscal_(&size, &qs, &cholesky[shift], &one);
    }
    shift -= dimension + 1;
  }
}

/* Build the QR factorization of the mxn matrix A
   Case m >= n:
   [R]
   A = [Q1 Q2][0]
   with Q1 a mxn matrix, Q2 a (m-n)xm matrix, R a nxn upper triangular matrix.
   [R]
   If fullQR == true, the matrices [Q1 Q2] and [0] are returned
   Else it is the matrices [Q1] and [R] that are returned.
   Case m < n:
   A = [Q][R1 R2]
   with Q a mxm matrix, R1 a mxm upper triangular matrix, R2 a mx(m-n) matrix.
   If fullQR == true or false, the matrices [Q] and [R1 R2] are returned
*/
MatrixImplementation MatrixImplementation::computeQR(MatrixImplementation & R,
    const Bool fullQR,
    const Bool keepIntact)
{
  int m = nbRows_;
  int n = nbColumns_;
  int lda = nbRows_;

  if (!(m > 0 && n > 0)) throw InvalidDimensionException(HERE) << "Cannot compute the QR decomposition of an empty matrix";
  int k = std::min(m, n);
  Point tau(k);
  int lwork = -1;
  int info = -1;
  double lwork_d = -1.;
  MatrixImplementation A;
  if (keepIntact) A = MatrixImplementation(*this);
  MatrixImplementation & Q = keepIntact ? A : *this;

  // First call to dgeqrf to get the optimal size for the working space
  dgeqrf_(&m, &n, &Q[0], &lda, &tau[0], &lwork_d, &lwork, &info);
  if (info != 0) throw InternalException(HERE) << "Lapack DGEQRF: error code=" << info;
  // Here is the optimal size of the working space
  lwork = static_cast<int>(lwork_d);
  Point work(lwork);
  // Second call to compute the decomposition
  dgeqrf_(&m, &n, &Q[0], &lda, &tau[0], &work[0], &lwork, &info);
  if (info != 0) throw InternalException(HERE) << "Lapack DGEQRF: error code=" << info;

  // Rebuild R
  int p(fullQR ? m : k);
  R = MatrixImplementation(p, n);
  for ( UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(k) ; ++ i )
    for ( UnsignedInteger j = i; j < static_cast<UnsignedInteger>(n); ++ j )
      R(i, j) = Q(i, j);

  // Rebuild Q
  // It is done using the product of the reflectors computed by dgeqrf
  // First call to dorgqr to get the optimal size for the working space
  lwork = -1;
  dorgqr_(&m, &p, &k, &Q[0], &lda, &tau[0], &lwork_d, &lwork, &info);
  if (info != 0) throw InternalException(HERE) << "Lapack DORGQR: error code=" << info;
  // Here is the optimal size of the working space
  lwork = static_cast<int>(lwork_d);
  work = Point(lwork);
  // Second call to compute the product of reflectors
  if (fullQR && (m > n))
  {
    // Here we must copy Q into a larger matrix to get the desired mxm Q factor before the call to dorgqr
    Q.resize(m * m);
    Q.nbRows_ = m;
    Q.nbColumns_ = m;
  }
  dorgqr_(&m, &p, &k, &Q[0], &lda, &tau[0], &work[0], &lwork, &info);
  if (m < n)
  {
    // Here we must copy Q into a smaller matrix to get the desired mxm Q factor after the call to dorgqr
    Q.resize(m * m);
    Q.nbRows_ = m;
    Q.nbColumns_ = m;
  }
  if (info != 0) throw InternalException(HERE) << "Lapack DORGQR: error code=" << info;
  Q.setName("");
  return Q;
}

/* Method save() stores the object through the StorageManager */
void MatrixImplementation::save(Advocate & adv) const
{
  PersistentCollection<Scalar>::save(adv);
  adv.saveAttribute( "nbRows_",    nbRows_);
  adv.saveAttribute( "nbColumns_", nbColumns_);
}

/* Method load() reloads the object from the StorageManager */
void MatrixImplementation::load(Advocate & adv)
{
  PersistentCollection<Scalar>::load(adv);

  adv.loadAttribute( "nbRows_",    nbRows_);
  adv.loadAttribute( "nbColumns_", nbColumns_);
}

UnsignedInteger MatrixImplementation::stride(const UnsignedInteger dim) const
{
  UnsignedInteger stride = elementSize();
  if (dim > 0)
    stride *= nbRows_;
  return stride;
}


END_NAMESPACE_OPENTURNS
