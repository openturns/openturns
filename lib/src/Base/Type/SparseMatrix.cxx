//                                               -*- C++ -*-
/**
 *  @brief Compressed sparse column matrix representation
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
#include "openturns/SparseMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseMatrix)

/* Default constructor */
SparseMatrix::SparseMatrix()
  : PersistentObject()
{
  // Nothing to do
}

/* Constructor with dimensions */
SparseMatrix::SparseMatrix( const UnsignedInteger nbRows,
                            const UnsignedInteger nbColumns)
  : PersistentObject()
  , nbRows_(nbRows)
  , nbColumns_(nbColumns)
{
  columnPointer_.resize(nbColumns + 1, 0);
}

/* Constructor from triples */
SparseMatrix::SparseMatrix(const UnsignedInteger nbRows,
                           const UnsignedInteger nbColumns,
                           const Indices & rowIndices,
                           const Indices & columnIndices,
                           const Point & values)
  : PersistentObject()
  , nbRows_(nbRows)
  , nbColumns_(nbColumns)
{
  const UnsignedInteger size = values.getSize();
  size_ = size;
  if ((rowIndices.getSize() != size) || (columnIndices.getSize() != size)) throw InvalidArgumentException(HERE) << "Invalid triplets size";

  columnPointer_.resize(nbColumns + 1, 0);

  // allocate storage
  rowIndex_.resize(size, 0);
  values_.resize(size, 0.0);

  // compute number of non-zero entries per column
  for (UnsignedInteger n = 0; n < size; ++ n)
    columnPointer_[columnIndices[n]] ++;

  // initialize column pointer array
  UnsignedInteger cumsum = 0;
  for (UnsignedInteger j = 0; j < nbColumns; ++ j)
  {
    const UnsignedInteger temp = columnPointer_[j];
    columnPointer_[j] = cumsum;
    cumsum += temp;
  }
  columnPointer_[nbColumns] = size;

  // fill values
  for (UnsignedInteger n = 0; n < size; ++ n)
  {
    const UnsignedInteger column = columnIndices[n];
    const UnsignedInteger row = rowIndices[n];
    const UnsignedInteger dest = columnPointer_[column];
    rowIndex_[dest] = row;
    values_[dest] = values[n];
    // update column pointer to the next element to insert
    ++ columnPointer_[column];
  }

  // shift column pointer
  UnsignedInteger last = 0;
  for (UnsignedInteger j = 0; j < nbColumns; ++ j)
  {
    const UnsignedInteger temp = columnPointer_[j];
    columnPointer_[j] = last;
    last = temp;
  }
}

/* Virtual constructor */
SparseMatrix * SparseMatrix::clone() const
{
  return new SparseMatrix(*this);
}

/* Read-only accessor to values */
Scalar SparseMatrix::operator()(const UnsignedInteger i, const UnsignedInteger j) const
{
  if (i >= nbRows_) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (j >= nbColumns_) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
  {
    // TODO: sum duplicate coordinate values
    if (rowIndex_[k] == i)
      return values_[k];
  }
  return 0.0;
}

/* Filling matrix from coordinates and value */
Scalar & SparseMatrix::operator()(const UnsignedInteger i, const UnsignedInteger j)
{
  if (i >= nbRows_) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (j >= nbColumns_) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  UnsignedInteger index = columnPointer_[j + 1];
  for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
  {
    if (i < rowIndex_[k])
    {
      index = k;
      break;
    }
    // TODO: handle duplicate coordinate values
    if (rowIndex_[k] == i)
      return values_[k];
  }
  ++ size_;
  rowIndex_.insert(rowIndex_.begin() + index, i);
  values_.insert(values_.begin() + index, 0.0);
  for (UnsignedInteger k = j; k < nbColumns_; ++ k)
    columnPointer_[k + 1] += 1;
  return values_[index];
}

/* Multiplication by a vector */
Point SparseMatrix::operator *(const Point & rhs) const
{
  if (rhs.getDimension() != nbColumns_) throw InvalidDimensionException(HERE) << "Invalid rhs size";
  Point output(nbRows_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
      output[rowIndex_[k]] += values_[k] * rhs[j];
  return output;
}

/* Number of rows */
UnsignedInteger SparseMatrix::getNbRows() const
{
  return nbRows_;
}

/* Number of columns */
UnsignedInteger SparseMatrix::getNbColumns() const
{
  return nbColumns_;
}

/* Get the number of non-zeros elements */
UnsignedInteger SparseMatrix::getNbNonZeros() const
{
  return size_;
}

/* Transposition */
SparseMatrix SparseMatrix::transpose() const
{
  SparseMatrix output(getNbColumns(), getNbRows());
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
      output(j, rowIndex_[k]) += values_[k];
  return output;
}

/* Sparse / dense conversions */
Matrix SparseMatrix::asDenseMatrix() const
{
  Matrix result(nbRows_, nbColumns_);
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
      result(rowIndex_[k], j) += values_[k];
  return result;
}

/* Multiplication with dense matrix */
Matrix SparseMatrix::operator * (const Matrix & m) const
{
  if (m.getNbRows() != getNbColumns())
    throw InvalidDimensionException(HERE) << "SparseMatrix multiplication expected row dimension " << getNbColumns();
  Matrix result(getNbRows(), m.getNbColumns());
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
      for (UnsignedInteger p = 0; p < m.getNbColumns(); ++ p)
        result(rowIndex_[k], p) += values_[k] * m(j, p);
  return result;
}

/* String converter */
String SparseMatrix::__repr__() const
{
  OSS oss(true);
  oss << "class=" << getClassName()
      << " rows=" << getNbRows()
      << " columns=" << getNbColumns()
      << " triplets=[";
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
    {
      oss << "[" << rowIndex_[k] << "," << j << "," << values_[k] << "]";
      oss << ((j == nbColumns_-1) && (k == columnPointer_[j + 1] - 1) ? "" : ",");
    }
  oss << "]";
  return oss;
}

String SparseMatrix::__str__(const String & /*offset*/) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
// void SparseMatrix::save(Advocate & adv) const
// {
//   PersistentObject::save(adv);
//   adv.saveAttribute( "values_", Point(Collection<Scalar>(values_.begin(), values_.end())));
// }


/* Method load() reloads the object from the StorageManager */
// void SparseMatrix::load(Advocate & adv)
// {
//   PersistentObject::load(adv);
//   adv.loadAttribute("values_", values_);
// }

END_NAMESPACE_OPENTURNS
