//                                               -*- C++ -*-
/**
 *  @brief Compressed sparse column matrix representation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <algorithm>
#include <utility>
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(SparseMatrix)

static const Factory<SparseMatrix> Factory_SparseMatrix;

/* Default constructor */
SparseMatrix::SparseMatrix()
  : PersistentObject()
  , size_(0)
  , nbRows_(0)
  , nbColumns_(0)
{
  columnPointer_.assign(1, 0);
}

/* Constructor with dimensions */
SparseMatrix::SparseMatrix( const UnsignedInteger nbRows,
                            const UnsignedInteger nbColumns)
  : PersistentObject()
  , size_(0)
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
  , size_(0)
  , nbRows_(nbRows)
  , nbColumns_(nbColumns)
{
  const UnsignedInteger size = values.getSize();
  if ((rowIndices.getSize() != size) || (columnIndices.getSize() != size))
    throw InvalidArgumentException(HERE) << "Invalid triplets size";
  for (UnsignedInteger n = 0; n < size; ++ n)
  {
    if (!(rowIndices[n] < nbRows_))
      throw OutOfBoundException(HERE) << "The row index " << rowIndices[n] << " must be less than the row dimension " << nbRows_;
    if (!(columnIndices[n] < nbColumns_))
      throw OutOfBoundException(HERE) << "The column index " << columnIndices[n] << " must be less than the column dimension " << nbColumns_;
  }

  // count the entries of each column, then turn the counts into start offsets
  columnPointer_.assign(nbColumns + 1, 0);
  for (UnsignedInteger n = 0; n < size; ++ n)
    ++ columnPointer_[columnIndices[n] + 1];
  for (UnsignedInteger j = 0; j < nbColumns; ++ j)
    columnPointer_[j + 1] += columnPointer_[j];

  // fill the columns, each one in the order of the triplets
  rowIndex_.assign(size, 0);
  values_.assign(size, 0.0);
  std::vector<UnsignedInteger> cursor(columnPointer_);
  for (UnsignedInteger n = 0; n < size; ++ n)
  {
    const UnsignedInteger column = columnIndices[n];
    const UnsignedInteger destination = cursor[column]++;
    rowIndex_[destination] = rowIndices[n];
    values_[destination] = values[n];
  }

  // sort the row indices within each column and sum the duplicate coordinates,
  // building the compacted layout in separate arrays: the deduplication shifts
  // the columns, so an in-place compaction would mix the column boundaries
  std::vector<std::pair<UnsignedInteger, Scalar> > columnEntries;
  std::vector<UnsignedInteger> compactRowIndex;
  std::vector<Scalar> compactValues;
  std::vector<UnsignedInteger> compactColumnPointer(nbColumns_ + 1, 0);
  compactRowIndex.reserve(size);
  compactValues.reserve(size);
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
  {
    const UnsignedInteger start = columnPointer_[j];
    const UnsignedInteger end = columnPointer_[j + 1];
    columnEntries.clear();
    columnEntries.reserve(end - start);
    for (UnsignedInteger k = start; k < end; ++ k)
      columnEntries.push_back(std::make_pair(rowIndex_[k], values_[k]));
    std::sort(columnEntries.begin(), columnEntries.end());
    UnsignedInteger k = 0;
    while (k < columnEntries.size())
    {
      const UnsignedInteger row = columnEntries[k].first;
      Scalar value = columnEntries[k].second;
      ++ k;
      while ((k < columnEntries.size()) && (columnEntries[k].first == row))
      {
        value += columnEntries[k].second;
        ++ k;
      }
      compactRowIndex.push_back(row);
      compactValues.push_back(value);
    }
    compactColumnPointer[j + 1] = compactRowIndex.size();
  }
  rowIndex_.swap(compactRowIndex);
  values_.swap(compactValues);
  columnPointer_.swap(compactColumnPointer);
  size_ = rowIndex_.size();
}

/* Conversion from a dense matrix */
SparseMatrix SparseMatrix::FromDenseMatrix(const Matrix & matrix)
{
  const UnsignedInteger nbRows = matrix.getNbRows();
  const UnsignedInteger nbColumns = matrix.getNbColumns();
  SparseMatrix result(nbRows, nbColumns);
  // count the non-zero coefficients of each column
  for (UnsignedInteger j = 0; j < nbColumns; ++ j)
  {
    UnsignedInteger count = 0;
    for (UnsignedInteger i = 0; i < nbRows; ++ i)
      if (matrix(i, j) != 0.0)
        ++ count;
    result.columnPointer_[j + 1] = count;
  }
  // turn the counts into start offsets
  for (UnsignedInteger j = 0; j < nbColumns; ++ j)
    result.columnPointer_[j + 1] += result.columnPointer_[j];
  // the coefficients are visited in increasing row order, so each column is
  // already sorted and contains no duplicate
  const UnsignedInteger size = result.columnPointer_[nbColumns];
  result.size_ = size;
  result.rowIndex_.assign(size, 0);
  result.values_.assign(size, 0.0);
  std::vector<UnsignedInteger> cursor(result.columnPointer_);
  for (UnsignedInteger j = 0; j < nbColumns; ++ j)
    for (UnsignedInteger i = 0; i < nbRows; ++ i)
      if (matrix(i, j) != 0.0)
      {
        const UnsignedInteger destination = cursor[j]++;
        result.rowIndex_[destination] = i;
        result.values_[destination] = matrix(i, j);
      }
  return result;
}

/* Virtual constructor */
SparseMatrix * SparseMatrix::clone() const
{
  return new SparseMatrix(*this);
}

/* Read-only accessor to values */
Scalar SparseMatrix::operator()(const UnsignedInteger i, const UnsignedInteger j) const
{
  if (!(i < nbRows_)) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (!(j < nbColumns_)) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
  {
    if (rowIndex_[k] == i)
      return values_[k];
  }
  return 0.0;
}

/* Filling matrix from coordinates and value */
Scalar & SparseMatrix::operator()(const UnsignedInteger i, const UnsignedInteger j)
{
  if (!(i < nbRows_)) throw OutOfBoundException(HERE) << "i (" << i << ") must be less than row dim (" << nbRows_ << ")";
  if (!(j < nbColumns_)) throw OutOfBoundException(HERE) << "j (" << j << ") must be less than column dim (" << nbColumns_ << ")";
  UnsignedInteger index = columnPointer_[j + 1];
  for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
  {
    if (i < rowIndex_[k])
    {
      index = k;
      break;
    }
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
  {
    const Scalar yJ(rhs[j]);
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
      output[rowIndex_[k]] += values_[k] * yJ;
  }
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

/* Accessor to the column pointer of the compressed sparse column layout */
Indices SparseMatrix::getColumnPointer() const
{
  return Indices(columnPointer_.begin(), columnPointer_.end());
}

/* Accessor to the row indices of the compressed sparse column layout */
Indices SparseMatrix::getRowIndices() const
{
  return Indices(rowIndex_.begin(), rowIndex_.end());
}

/* Accessor to the values of the compressed sparse column layout */
Point SparseMatrix::getValues() const
{
  return Point(values_.begin(), values_.end());
}

/* Transposition */
SparseMatrix SparseMatrix::transpose() const
{
  // Filling the result one coefficient at a time would be quadratic, as each
  // insertion shifts the stored coefficients and fixes all the following
  // column pointers. The transposed coefficients are gathered in triplets and
  // handed over to the constructor, which lays them out in a single pass.
  Indices rowIndices;
  Indices columnIndices;
  Point values;
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
    {
      // the row and the column of the stored coefficients are swapped
      rowIndices.add(j);
      columnIndices.add(rowIndex_[k]);
      values.add(values_[k]);
    }
  return SparseMatrix(nbColumns_, nbRows_, rowIndices, columnIndices, values);
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

/* Multiplication by a dense matrix given as left operand */
Matrix SparseMatrix::multiplyByDenseOnLeft(const Matrix & m) const
{
  if (m.getNbColumns() != getNbRows())
    throw InvalidDimensionException(HERE) << "SparseMatrix multiplication expected column dimension " << getNbRows();
  const UnsignedInteger nbRowsOfResult = m.getNbRows();
  Matrix result(nbRowsOfResult, nbColumns_);
  // Each column j of the result is the combination of the columns of m
  // selected by the non-zero coefficients of column j of the sparse matrix
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
    {
      const Scalar value = values_[k];
      const UnsignedInteger i = rowIndex_[k];
      for (UnsignedInteger p = 0; p < nbRowsOfResult; ++ p)
        result(p, j) += value * m(p, i);
    }
  return result;
}

/* Multiplication by a symmetric matrix given as left operand */
Matrix SparseMatrix::multiplyBySymmetricOnLeft(const SymmetricMatrix & m) const
{
  if (m.getNbColumns() != getNbRows())
    throw InvalidDimensionException(HERE) << "SparseMatrix multiplication expected column dimension " << getNbRows();
  const UnsignedInteger nbRowsOfResult = m.getNbRows();
  Matrix result(nbRowsOfResult, nbColumns_);
  // Here m(p, i) is read through the symmetric accessor, which is valid
  // although only the lower triangle of m is actually stored
  for (UnsignedInteger j = 0; j < nbColumns_; ++ j)
    for (UnsignedInteger k = columnPointer_[j]; k < columnPointer_[j + 1]; ++ k)
    {
      const Scalar value = values_[k];
      const UnsignedInteger i = rowIndex_[k];
      for (UnsignedInteger p = 0; p < nbRowsOfResult; ++ p)
        result(p, j) += value * m(p, i);
    }
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
      oss << ((j == nbColumns_ - 1) && (k == columnPointer_[j + 1] - 1) ? "" : ",");
    }
  oss << "]";
  return oss;
}

String SparseMatrix::__str__(const String & /*offset*/) const
{
  return __repr__();
}

/* Method save() stores the object through the StorageManager */
void SparseMatrix::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute("columnPointer_", Indices(columnPointer_.begin(), columnPointer_.end()));
  adv.saveAttribute("rowIndex_", Indices(rowIndex_.begin(), rowIndex_.end()));
  adv.saveAttribute("values_", Point(values_.begin(), values_.end()));
  adv.saveAttribute("size_", size_);
  adv.saveAttribute("nbRows_", nbRows_);
  adv.saveAttribute("nbColumns_", nbColumns_);
}

/* Method load() reloads the object from the StorageManager */
void SparseMatrix::load(Advocate & adv)
{
  PersistentObject::load(adv);
  Indices columnPointer;
  Indices rowIndices;
  Point values;
  adv.loadAttribute("columnPointer_", columnPointer);
  adv.loadAttribute("rowIndex_", rowIndices);
  adv.loadAttribute("values_", values);
  columnPointer_.assign(columnPointer.begin(), columnPointer.end());
  rowIndex_.assign(rowIndices.begin(), rowIndices.end());
  values_.assign(values.begin(), values.end());
  adv.loadAttribute("size_", size_);
  adv.loadAttribute("nbRows_", nbRows_);
  adv.loadAttribute("nbColumns_", nbColumns_);
}

END_NAMESPACE_OPENTURNS
