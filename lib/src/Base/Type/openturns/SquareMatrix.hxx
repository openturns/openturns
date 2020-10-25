//                                               -*- C++ -*-
/**
 *  @brief SquareMatrix implements the classical mathematical square matrix
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
#ifndef OPENTURNS_SQUAREMATRIX_HXX
#define OPENTURNS_SQUAREMATRIX_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Matrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class IdentityMatrix;

class SymmetricMatrix;

class SquareComplexMatrix;

/**
 * @class SquareMatrix
 *
 * SquareMatrix implements the classical mathematical square matrix
 */

class OT_API SquareMatrix :
  public Matrix
{
  CLASSNAME

#ifndef SWIG
  friend SquareMatrix operator * (const Scalar s,
                                  const SquareMatrix & m);
#endif

public:

  /** Default constructor */
  SquareMatrix();

  /** Constructor with implementation */
  SquareMatrix(const Implementation & i);

  /** Constructor with implementation */
  SquareMatrix(const MatrixImplementation & i);

  /** Constructor with size (dim, which is the same for nbRows_ and nbColumns_) */
  explicit SquareMatrix(const UnsignedInteger dim);
#if 0
  /** Constructor from range of external collection */
  template <class InputIterator>
  SquareMatrix(const UnsignedInteger dim,
               InputIterator first,
               InputIterator last);
#endif
  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  SquareMatrix(const UnsignedInteger dim,
               const ScalarCollection & elementsValues);

  /** Constructor from symmetric matrix */
  SquareMatrix(const SymmetricMatrix & symmetric);

  /** String converter */
  String __repr__() const override;

  /** Get the dimension of the matrix */
  UnsignedInteger getDimension() const;

  /** SquareMatrix transpose */
  SquareMatrix transpose () const;

  /** SquareMatrix additions (must have the same dimensions) */
  SquareMatrix operator + (const SquareMatrix & m) const;
  SquareMatrix operator + (const SymmetricMatrix & m) const;

  /** SquareMatrix subtractions (must have the same dimensions) */
  SquareMatrix operator - (const SquareMatrix & m) const;
  SquareMatrix operator - (const SymmetricMatrix & m) const;

  /** SquareMatrix multiplications (must have consistent dimensions) */
#ifdef _MSC_VER   // VS2010 does not like 'using' being called after overloads
  using Matrix::operator *;
#endif
  SquareMatrix operator * (const SquareMatrix & m) const;
  SquareMatrix operator * (const SymmetricMatrix & m) const;
  SquareMatrix operator * (const IdentityMatrix & m) const;

  /** SquareMatrix integer power */
  SquareMatrix power(const UnsignedInteger n) const;

  /** Multiplication with a Point (must have consistent dimensions) */
  Point operator * (const Point & p) const;

  /** Multiplication with a Scalar */
  SquareMatrix operator * (const Scalar s) const;

  // We import the definitions from the upper class (for Matrix multiplication)
#ifndef _MSC_VER   // VS2010 does not like 'using' being called after overloads
  using Matrix::operator *;
#endif

  /** Division by a Scalar*/
  SquareMatrix operator / (const Scalar s) const;

  /** Resolution of a linear system */
  Point solveLinearSystem(const Point & b,
                          const Bool keepIntact = true);

  Matrix solveLinearSystem(const Matrix & b,
                           const Bool keepIntact = true);

  /** Compute determinant */
  Scalar computeLogAbsoluteDeterminant(Scalar & signOut,
                                       const Bool keepIntact = true);
  Scalar computeDeterminant(const Bool keepIntact = true);

  /** Compute trace */
  Scalar computeTrace() const;

  /** Compute eigenvalues */
  ComplexCollection computeEigenValues(const Bool keepIntact = true);
  ComplexCollection computeEV(SquareComplexMatrix & vOut,
                              const Bool keepIntact = true);
  /** Compute the largest eigenvalue module using power iterations */
  virtual Scalar computeLargestEigenValueModule(const UnsignedInteger maximumIterations = ResourceMap::GetAsUnsignedInteger("Matrix-LargestEigenValueIterations"),
      const Scalar epsilon = ResourceMap::GetAsScalar("Matrix-LargestEigenValueRelativeError")) const;

  /** Check if it is diagonal */
  Bool isDiagonal() const;

protected:

private:

}; /* class SquareMatrix */


inline SquareMatrix operator * (const Scalar s,
                                const SquareMatrix & m)
{
  return m.operator * (s);
}


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MATRIX_HXX */
