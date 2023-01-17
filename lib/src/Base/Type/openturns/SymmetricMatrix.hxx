//                                               -*- C++ -*-
/**
 *  @brief SymmetricMatrix implements the classical mathematical symmetric matrix
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SYMMETRICMATRIX_HXX
#define OPENTURNS_SYMMETRICMATRIX_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/SquareMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class IdentityMatrix;

/**
 * @class SymmetricMatrix
 *
 * SymmetricMatrix implements the classical mathematical square matrix
 */

class OT_API SymmetricMatrix :
  public SquareMatrix
{
  CLASSNAME

#ifndef SWIG
  friend SymmetricMatrix operator * (const Scalar & s,
                                     const SymmetricMatrix & m);
#endif

public:

  /** Default constructor */
  SymmetricMatrix();

  /** Constructor with implementation */
  SymmetricMatrix(const Implementation & i);

  /** Constructor with implementation */
  SymmetricMatrix(const MatrixImplementation & i);

#ifndef SWIG
  /** Copy constructor, added to solve glitches with inheritance */
  SymmetricMatrix(const SymmetricMatrix & other);

  /** Assignment operator */
  SymmetricMatrix& operator=(const SymmetricMatrix & rhs);
#endif

  /** Constructor with size (dim, which is the same for nbRows_ and nbColumns_) */
  explicit SymmetricMatrix(const UnsignedInteger dim);
#if 0
  /** Constructor from range of external collection */
  template <class InputIterator>
  SymmetricMatrix(const UnsignedInteger dim,
                  InputIterator first,
                  InputIterator last);
#endif

  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  SymmetricMatrix(const UnsignedInteger dim,
                  const ScalarCollection & elementsValues);

  /** Check if the internal representation is actually symmetric */
  void checkSymmetry() const;

  /** Test if the matrix is diagonal */
  Bool isDiagonal() const;

  /** Row extraction */
  const Matrix getRow(const UnsignedInteger rowIndex) const;
  /** Column extration */
  const Matrix getColumn(const UnsignedInteger columnIndex) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

#ifndef SWIG
  /** Operator () gives access to the elements of the matrix (to modify these elements) */
  /** The element of the matrix is designated by its row number i and its column number j */
  Scalar & operator () (const UnsignedInteger i,
                        const UnsignedInteger j);

  /** Operator () gives access to the elements of the matrix (read only) */
  /** The element of the matrix is designated by its row number i and its column number j */
  const Scalar & operator () (const UnsignedInteger i,
                              const UnsignedInteger j) const;
#endif

  /** SymmetricMatrix transpose */
  SymmetricMatrix transpose () const;

  /** SymmetricMatrix addition (must have the same dimensions) */
  Matrix operator + (const Matrix & m) const;
  SquareMatrix operator + (const SquareMatrix & m) const;
  SymmetricMatrix operator + (const SymmetricMatrix & m) const;

  /** SymmetricMatrix subtraction (must have the same dimensions) */
  Matrix operator - (const Matrix & m) const;
  SquareMatrix operator - (const SquareMatrix & m) const;
  SymmetricMatrix operator - (const SymmetricMatrix & m) const;

  /** SymmetricMatrix multiplications (must have consistent dimensions) */
  Matrix operator * (const Matrix & m) const;
  SquareMatrix operator * (const SquareMatrix & m) const;
  SquareMatrix operator * (const SymmetricMatrix & m) const;
  SymmetricMatrix operator * (const IdentityMatrix & m) const;

  /** SymmetricMatrix integer power */
  SymmetricMatrix power(const UnsignedInteger n) const;

  /** Multiplication with a Point (must have consistent dimensions) */
  Point operator * (const Point & p) const;

  /** Multiplication with a Scalar */
  SymmetricMatrix operator * (const Scalar & s) const;

  /** Division by a Scalar*/
  SymmetricMatrix operator / (const Scalar & s) const;

  /** Resolution of a linear system */
  Point solveLinearSystemInPlace(const Point & b);
  Point solveLinearSystem(const Point & b) const;
  Matrix solveLinearSystemInPlace(const Matrix & b);
  Matrix solveLinearSystem(const Matrix & b) const;

  /** Compute determinant */
  Scalar computeLogAbsoluteDeterminant(Scalar & signOut,
                                       const Bool keepIntact = true);
  Scalar computeDeterminant(const Bool keepIntact = true);

  /** Compute eigenvalues */
  Point computeEigenValues(const Bool keepIntact = true);
  Point computeEV(SquareMatrix & vOut,
                  const Bool keepIntact = true);
  /** Compute the largest eigenvalue module using power iterations */
  Scalar computeLargestEigenValueModule(const UnsignedInteger maximumIterations = ResourceMap::GetAsUnsignedInteger("Matrix-LargestEigenValueIterations"),
                                        const Scalar epsilon = ResourceMap::GetAsScalar("Matrix-LargestEigenValueRelativeError")) const override;

  /** Comparison operators */
  Bool operator == (const Matrix & rhs) const;

  /** Sum all coefficients */
  Scalar computeSumElements() const override;

protected:


private:
  /** Check if one needs to symmetrized the internal representation of the matrix */
  mutable Bool hasBeenSymmetrized_;

}; /* class SymmetricMatrix */

inline SymmetricMatrix operator * (const Scalar & s,
                                   const SymmetricMatrix & m)
{
  return m.operator * (s);
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MATRIX_HXX */
