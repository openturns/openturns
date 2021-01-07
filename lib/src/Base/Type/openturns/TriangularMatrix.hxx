//                                               -*- C++ -*-
/**
 *  @brief TriangularMatrix implements the classical mathematical triangular matrix with values
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
#ifndef OPENTURNS_TRIANGULARMATRIX_HXX
#define OPENTURNS_TRIANGULARMATRIX_HXX

#include "openturns/SquareMatrix.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

class SymmetricMatrix;
class IdentityMatrix;
class SquareMatrix;
class Matrix;
class HermitianMatrix;

/**
 * @class TriangularMatrix
 *
 * TriangularMatrix implements the classical mathematical triangular matrix with scalar values
 * Default implementation is triangular lower
 */

class OT_API TriangularMatrix :
  public SquareMatrix
{
  CLASSNAME

#ifndef SWIG
  friend TriangularMatrix operator * (const Scalar s,
                                      const TriangularMatrix & m);
#endif

public:


  /** Default constructor */
  TriangularMatrix();

  /** Constructor with size (dim, which is the same for nbRows_ and nbColumns_) */
  explicit TriangularMatrix(const UnsignedInteger dimension,
                            Bool isLower = true);

  /** Constructor with implementation */
  TriangularMatrix(const Implementation & i,
                   const Bool isLower = true);

  /** Constructor with matrix implementation */
  TriangularMatrix(const MatrixImplementation & i,
                   const Bool isLower = true);

  /** Check if the internal representation is actually symmetric */
  void checkTriangularity() const;

  /** Test if the matrix is diagonal */
  Bool isDiagonal() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Get the dimension of the matrix */
  UnsignedInteger getDimension() const;

  /** TriangularMatrix transpose */
  TriangularMatrix transpose () const;

  /** Check if the matrix is lower or upper */
  Bool isLowerTriangular() const;

#ifndef SWIG
  /** Operator () gives access to the elements of the matrix (to modify these elements) */
  /** The element of the matrix is designated by its row number i and its column number j */
  Scalar & operator ()(const UnsignedInteger i,
                       const UnsignedInteger j) ;

  /** Operator () gives access to the elements of the matrix (read only) */
  /** The element of the matrix is designated by its row number i and its column number j */
  const Scalar & operator ()(const UnsignedInteger i,
                             const UnsignedInteger j) const;
#endif


  /** TriangularMatrix additions : result is a square matrix */
  SquareMatrix operator + (const TriangularMatrix & m) const;

  /** Addition operator */
  SquareMatrix operator + (const SquareMatrix & m) const;

  /** Subtraction operator with TriangularMatrix */
  SquareMatrix operator - (const TriangularMatrix & m) const;

  /** Subtraction operator */
  SquareMatrix operator - (const SquareMatrix & m) const;

  /** Multiplication with a scalar */
  TriangularMatrix operator * (const Scalar s) const ;

  /** Matrix multiplications */
  Matrix operator * (const Matrix & m) const;

  /** SquareMatrix multiplications */
  SquareMatrix operator * (const SquareMatrix & m) const;

  /** TriangularMatrix multiplications  */
  SquareMatrix operator * (const TriangularMatrix & m) const;

  /** Real SymmetricMatrix multiplications  */
  SquareMatrix operator * (const SymmetricMatrix & m) const;

  /** IdentityMatrix multiplications  */
  TriangularMatrix operator * (const IdentityMatrix & m) const;

  /** Multiplication with a ScalarCollection (must have consistent dimensions) */
  ScalarCollection operator * (const ScalarCollection & p) const;

  /** Multiplication with a Point (must have consistent dimensions) */
  ScalarCollection operator * (const Point & p) const;

  /** Division by a Scalar */
  TriangularMatrix operator / (const Scalar s) const;

  /** Resolution of a linear system */
  Point solveLinearSystem(const Point & b,
                          const Bool keepIntact = true);

  Matrix solveLinearSystem(const Matrix & b,
                           const Bool keepIntact = true);
private:

  /** Boolean information : is the matrix triangular lower or upper? */
  mutable Bool isLowerTriangular_;

  /** Check if one needs to symmetrized the internal representation of the matrix */
  mutable Bool hasBeenTriangularized_;

}; /* class TriangularMatrix */


inline TriangularMatrix operator * (const Scalar s,
                                    const TriangularMatrix & m)
{
  return m.operator * (s);
}


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRIANGULARMATRIX_HXX */
