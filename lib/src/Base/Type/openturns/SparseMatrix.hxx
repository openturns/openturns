//                                               -*- C++ -*-
/**
 *  @brief Compressed sparse column matrix representation
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SPARSEMATRIX_HXX
#define OPENTURNS_SPARSEMATRIX_HXX

#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SparseMatrix
 */

class OT_API SparseMatrix
  : public PersistentObject
{

  CLASSNAME

public:

  /** Default constructor without parameters */
  SparseMatrix();

  /** Default constructor with size */
  SparseMatrix(const UnsignedInteger nbRows,
               const UnsignedInteger nbCols);

  /** Constructor from triplets, duplicates are summed */
  SparseMatrix(const UnsignedInteger nbRows,
               const UnsignedInteger nbCols,
               const Indices & rowIndices,
               const Indices & columnIndices,
               const Point & values);

  /** Virtual copy constructor */
  SparseMatrix * clone() const override;

  /** Read-only accessor to values */
  Scalar operator()(const UnsignedInteger i, const UnsignedInteger j) const;

  /** Filling matrix from coordinates and value */
  Scalar & operator()(const UnsignedInteger i, const UnsignedInteger j);

  /** Multiplication by a vector */
  Point operator *(const Point & rhs) const;

  /** Number of rows */
  UnsignedInteger getNbRows() const;

  /** Number of columns */
  UnsignedInteger getNbColumns() const;

  /** Get the number of non-zeros elements */
  UnsignedInteger getNbNonZeros() const;

  /** Matrix transpose */
  SparseMatrix transpose() const;

  /** Sparse -> dense conversion */
  Matrix asDenseMatrix() const;

  /** Multiplication with dense matrix */
  Matrix operator * (const Matrix & m) const;

  /** String converter */
  String __repr__() const override;

  /** String converter */
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
//   void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
//   void load(Advocate & adv) override;

private:

  std::vector<Scalar> values_;
  std::vector<UnsignedInteger> columnPointer_;
  std::vector<UnsignedInteger> rowIndex_;

  UnsignedInteger size_ = 0;
  UnsignedInteger nbRows_ = 0;
  UnsignedInteger nbColumns_ = 0;

} ; /* class SparseMatrix */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPARSEMATRIX_HXX */
