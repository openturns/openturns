//                                               -*- C++ -*-
/**
 *  @brief SquareMatrix implements the classical mathematical square matrix
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_SQUAREMATRIX_HXX
#define OPENTURNS_SQUAREMATRIX_HXX

#include "OTprivate.hxx"
#include "Matrix.hxx"

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
  CLASSNAME;

#ifndef SWIG
  friend SquareMatrix operator * (const NumericalScalar s,
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
               const NumericalScalarCollection & elementsValues);


  /** String converter */
  String __repr__() const;

  /** Get the dimension of the matrix */
  UnsignedInteger getDimension() const;

  /** SquareMatrix transpose */
  SquareMatrix transpose () const;

  /** SquareMatrix additions (must have the same dimensions) */
  SquareMatrix operator + (const SquareMatrix & m) const;
  SquareMatrix operator + (const SymmetricMatrix & m) const;

  /** SquareMatrix substractions (must have the same dimensions) */
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

  /** Multiplication with a NumericalPoint (must have consistent dimensions) */
  NumericalPoint operator * (const NumericalPoint & p) const;

  /** Multiplication with a NumericalScalar */
  SquareMatrix operator * (const NumericalScalar s) const;

  // We import the definitions from the upper class (for Matrix multiplication)
#ifndef _MSC_VER   // VS2010 does not like 'using' being called after overloads
  using Matrix::operator *;
#endif

  /** Division by a NumericalScalar*/
  SquareMatrix operator / (const NumericalScalar s) const;

  /** Resolution of a linear system */
  NumericalPoint solveLinearSystem(const NumericalPoint & b,
                                   const Bool keepIntact = true);

  Matrix solveLinearSystem(const Matrix & b,
                           const Bool keepIntact = true);

  /** Compute determinant */
  NumericalScalar computeLogAbsoluteDeterminant(NumericalScalar & sign,
      const Bool keepIntact = true);
  NumericalScalar computeDeterminant(const Bool keepIntact = true);

  /** Compute trace */
  NumericalScalar computeTrace() const;

  /** Compute eigenvalues */
  NumericalComplexCollection computeEigenValues(const Bool keepIntact = true);
  NumericalComplexCollection computeEV(SquareComplexMatrix & v,
                                       const Bool keepIntact = true);

  /** Check if it is diagonal */
  Bool isDiagonal() const;

protected:

private:

}; /* class SquareMatrix */


inline SquareMatrix operator * (const NumericalScalar s,
                                const SquareMatrix & m)
{
  return m.operator * (s);
}


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MATRIX_HXX */
