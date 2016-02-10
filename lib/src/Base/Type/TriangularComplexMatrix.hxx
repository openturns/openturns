//                                               -*- C++ -*-
/**
 *  @brief TriangularComplexMatrix implements the classical mathematical triangular matrix with complex values
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
#ifndef OPENTURNS_TRIANGULARCOMPLEXMATRIX_HXX
#define OPENTURNS_TRIANGULARCOMPLEXMATRIX_HXX

#include "SquareComplexMatrix.hxx"
#include "Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

class SymmetricMatrix;
class IdentityMatrix;
class SquareMatrix;
class Matrix;
class HermitianMatrix;

/**
 * @class TriangularComplexMatrix
 *
 * TriangularComplexMatrix implements the classical mathematical triangular matrix with complex values
 * Default implementation is triangular lower
 */

class OT_API TriangularComplexMatrix :
  public SquareComplexMatrix
{
  CLASSNAME;

#ifndef SWIG
  friend TriangularComplexMatrix operator * (const NumericalComplex s,
      const TriangularComplexMatrix & m);
#endif

public:


  /** Default constructor */
  TriangularComplexMatrix();

  /** Constructor with size (dim, which is the same for nbRows_ and nbColumns_) */
  explicit TriangularComplexMatrix(const UnsignedInteger dimension,
                                   Bool isLower = true);

  /** Constructor with implementation */
  TriangularComplexMatrix(const Implementation & i,
                          const Bool isLower = true);

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Get the dimension of the matrix */
  UnsignedInteger getDimension() const;

  /** TriangularComplexMatrix transpose */
  TriangularComplexMatrix transpose () const;

  /** TriangularComplexMatrix conjugate */
  TriangularComplexMatrix conjugate () const;

  /** TriangularComplexMatrix conjugate and transpose */
  TriangularComplexMatrix conjugateTranspose () const;

  /** Check if the matrix is lower or upper */
  Bool isLowerTriangular() const;

#ifndef SWIG
  /** Operator () gives access to the elements of the matrix (to modify these elements) */
  /** The element of the matrix is designated by its row number i and its column number j */
  NumericalComplex & operator () (const UnsignedInteger i,
                                  const UnsignedInteger j) ;

  /** Operator () gives access to the elements of the matrix (read only) */
  /** The element of the matrix is designated by its row number i and its column number j */
  const NumericalComplex & operator () (const UnsignedInteger i,
                                        const UnsignedInteger j) const;
#endif


  /** TriangularComplexMatrix additions : result is a complex matrix */
  SquareComplexMatrix operator + (const TriangularComplexMatrix & m) const;

  /** Addition operator with ComplexMatrix */
  SquareComplexMatrix operator + (const SquareComplexMatrix & m) const;

  /** Substraction operator with TriangularComplexMatrix */
  SquareComplexMatrix operator - (const TriangularComplexMatrix & m) const;

  /** Substraction operator with ComplexMatrix */
  SquareComplexMatrix operator - (const SquareComplexMatrix & m) const;

  /** Multiplication with a NumericalComplex */
  TriangularComplexMatrix operator * (const NumericalComplex s) const;

  /** ComplexMatrix multiplications */
  ComplexMatrix operator * (const ComplexMatrix & m) const;

  /** SquareComplexMatrix multiplications */
  SquareComplexMatrix operator * (const SquareComplexMatrix & m) const;

  /** TriangularComplexMatrix multiplications  */
  SquareComplexMatrix operator * (const TriangularComplexMatrix & m) const;

  /** HermitianMatrix multiplications  */
  SquareComplexMatrix operator * (const HermitianMatrix & m) const;

  /** Real SquareMatrix multiplications */
  SquareComplexMatrix operator * (const SquareMatrix & m) const;

  /** Real Matrix multiplications (must have consistent dimensions) */
  ComplexMatrix operator * (const Matrix & m) const;

  /** Real SymmetricMatrix multiplications  */
  SquareComplexMatrix operator * (const SymmetricMatrix & m) const;

  /** IdentityMatrix multiplications  */
  TriangularComplexMatrix operator * (const IdentityMatrix & m) const;

  /** Multiplication with a NumericaComplexCollection (must have consistent dimensions) */
  NumericalComplexCollection operator * (const NumericalComplexCollection & p) const;

  /** Multiplication with a NumericaScalarCollection (must have consistent dimensions) */
  NumericalComplexCollection operator * (const NumericalScalarCollection & p) const;

  /** Multiplication with a NumericalPoint (must have consistent dimensions) */
  NumericalComplexCollection operator * (const NumericalPoint & p) const;

  /** Division by a NumericalComplex*/
  TriangularComplexMatrix operator / (const NumericalComplex s) const;

private:

  /** Boolean information : is the matrix triangular lower or upper? */
  mutable Bool isLowerTriangular_;

}; /* class TriangularComplexMatrix */


inline TriangularComplexMatrix operator * (const NumericalComplex s,
    const TriangularComplexMatrix & m)
{
  return m.operator * (s);
}


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TRIANGULARCOMPLEXMATRIX_HXX */
