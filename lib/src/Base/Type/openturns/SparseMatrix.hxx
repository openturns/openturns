//                                               -*- C++ -*-
/**
 *  @brief This class implements the computation of the Karhunen-Loeve
 *         basis and eigenvalues of a given covariance model based on
 *         P1 Lagrange approximation.
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
  SparseMatrix( const UnsignedInteger nbRows,
                const UnsignedInteger nbCols,
                const UnsignedInteger size = 1);

  /** Constructor from OT::Matrix */
  SparseMatrix(const Matrix & m);

  /** Virtual copy constructor */
  virtual SparseMatrix * clone() const;

  /** Read-only accessor to values */
  Scalar operator()(const UnsignedInteger i, const UnsignedInteger j) const;

  /** Filling matrix from coordinates and value */
  Scalar & operator()(const UnsignedInteger i, const UnsignedInteger j);

  /** Multiplication by a vector */
  Point operator *(const Point & rhs) const;

  /** Get the dimensions of the matrix */
  /** Number of rows */
  UnsignedInteger getNbRows() const;

  /** Number of columns */
  UnsignedInteger getNbColumns() const;

  /** Get the number of non-zeros elements */
  UnsignedInteger getNbNonZeros() const;

  /** Matrix transpose */
  SparseMatrix transpose() const;

  /** Sparse <> dense conversions */
  Matrix asDenseMatrix() const;

//   /** String converter */
//   virtual String __repr__() const;
// 
//   /** String converter */
//   virtual String __str__(const String & offset = "") const;
// 
//   /** Method save() stores the object through the StorageManager */
//   virtual void save(Advocate & adv) const;
// 
//   /** Method load() reloads the object from the StorageManager */
//   virtual void load(Advocate & adv);

private:
  // compressed column storage
  std::vector<Scalar> values_;
  std::vector<UnsignedInteger> columnIndex_;
  std::vector<UnsignedInteger> rowIndex_;
  UnsignedInteger size_;
  UnsignedInteger maxSize_;
  UnsignedInteger nbRows_;
  UnsignedInteger nbColumns_;
} ; /* class SparseMatrix */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPARSEMATRIX_HXX */
