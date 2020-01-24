//                                               -*- C++ -*-
/**
 *  @brief SparseMatrix implements Eigen sparse matrix class interface
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-Phimeca
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
#include "openturns/SparseMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseMatrix)

/* Default constructor */
SparseMatrix::SparseMatrix()
  : PersistentObject()
  , size_(0)
  , maxSize_(0)
  , nbRows_(0)
  , nbColumns_(0)
{
  // Nothing to do
}

/* Constructor with dimensions */
SparseMatrix::SparseMatrix( const UnsignedInteger nbRows,
                            const UnsignedInteger nbColumns,
                            const UnsignedInteger maxSize)
  : PersistentObject()
  , size_(0)
  , maxSize_(maxSize)
  , nbRows_(nbRows)
  , nbColumns_(nbColumns)
{
  if (!maxSize)
    throw InvalidArgumentException(HERE) << "maxSize must be positive";
  nbRows_ = nbRows;
  nbColumns_ = nbColumns;
  maxSize_ = 1;
  values_.resize(maxSize, 0.0);
  rowIndex_.resize(maxSize, 0);
  columnIndex_.resize(nbColumns + 1, 0);
}

/** Constructor from matrix */
SparseMatrix::SparseMatrix(const Matrix & m)
  : PersistentObject()
{
  for (UnsignedInteger i = 0; i < m.getNbRows(); ++i)
    for (UnsignedInteger j = 0; j < m.getNbColumns(); ++j)
      if (m(i,j) != 0)
        this->operator()(i,j) = m(i, j);
}

/* Virtual constructor */
SparseMatrix * SparseMatrix::clone() const
{
  return new SparseMatrix(*this);
}

/** Read-only accessor to values */
Scalar SparseMatrix::operator()(const UnsignedInteger i, const UnsignedInteger j) const
{
  if (i >= nbRows_) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (j >= nbColumns_) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  for (UnsignedInteger k = columnIndex_[j]; k < columnIndex_[j + 1]; ++ k)
  {
    if (rowIndex_[k] == i)
      return values_[k];
  }
  return 0.0;
}

/** Filling matrix from coordinates and value */
Scalar & SparseMatrix::operator()(const UnsignedInteger i, const UnsignedInteger j)
{
  if (i >= nbRows_) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (j >= nbColumns_) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  if (size_ >= maxSize_)
  {
    maxSize_ *= 2;
    values_.resize(maxSize_, 0.0);
    rowIndex_.resize(maxSize_, 0);
  }
  UnsignedInteger index = columnIndex_[j + 1];
  for (UnsignedInteger k = columnIndex_[j]; k < columnIndex_[j + 1]; ++ k)
  {
    if (i < rowIndex_[k])
    {
      index = k;
      break;
    }
    if (rowIndex_[k] == i)
    {
      return values_[k];
    }
  }
  ++ size_;
  rowIndex_.insert(rowIndex_.begin() + index, i);
  values_.insert(values_.begin() + index, 0.0);
  for (UnsignedInteger k = j; k < nbColumns_; ++ k)
    columnIndex_[k + 1] += 1;
  return values_[index];
}

/** Multiplication by a vector */
Point SparseMatrix::operator *(const Point & rhs) const
{
  if (rhs.getDimension() != nbColumns_) throw InvalidDimensionException(HERE) << "Invalid rhs size";
  Point output(nbRows_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnIndex_[j]; k < columnIndex_[j + 1]; ++ k)
      output[rowIndex_[k]] += values_[k] * rhs[j];
  return output;
}

/** Get the dimensions of the matrix */
/** Number of rows */
UnsignedInteger SparseMatrix::getNbRows() const
{
  return nbRows_;
}

/** Number of columns */
UnsignedInteger SparseMatrix::getNbColumns() const
{
  return nbColumns_;
}

/** Get the number of non-zeros elements */
UnsignedInteger SparseMatrix::getNbNonZeros() const
{
  return size_;
}

/** Transposition */
SparseMatrix SparseMatrix::transpose() const
{
  SparseMatrix output(getNbColumns(), getNbRows());
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnIndex_[j]; k < columnIndex_[j + 1]; ++ k)
      output(j, rowIndex_[k]) = values_[k];
  return output;
}

/** Sparse / dense conversions */
Matrix SparseMatrix::asDenseMatrix() const
{
  Matrix result(nbRows_, nbColumns_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnIndex_[j]; k < columnIndex_[j + 1]; ++ k)
      result(rowIndex_[k], j) = values_[k];
  return result;
}

END_NAMESPACE_OPENTURNS
