//                                               -*- C++ -*-
/**
 *  @brief SquareComplexMatrix implements the classical mathematical square matrix
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
#ifndef OPENTURNS_SQUARECOMPLEXMATRIX_HXX
#define OPENTURNS_SQUARECOMPLEXMATRIX_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ComplexMatrix.hxx"

BEGIN_NAMESPACE_OPENTURNS

class IdentityMatrix;

class HermitianMatrix;

class TriangularComplexMatrix;

/**
 * @class SquareComplexMatrix
 *
 * SquareComplexMatrix implements the classical mathematical square matrix
 */

class OT_API SquareComplexMatrix :
  public ComplexMatrix
{
  CLASSNAME

#ifndef SWIG
  friend SquareComplexMatrix operator * (const Complex s,
                                         const SquareComplexMatrix & m);
#endif

public:

  /** Default constructor */
  SquareComplexMatrix();

  /** Constructor with size (dimension, which is the same for nbRows_ and nbColumns_) */
  explicit SquareComplexMatrix(const UnsignedInteger dim);


  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  SquareComplexMatrix(const UnsignedInteger dimension,
                      const ComplexCollection & elementsValues);

  /** Constructor with implementation */
  SquareComplexMatrix(const Implementation & i);

  /** Constructor from hermitian matrix */
  SquareComplexMatrix(const HermitianMatrix & hermitian);

  /** String converter */
  String __repr__() const override;

  /** Get the dimension of the matrix */
  UnsignedInteger getDimension() const;

  /** SquareComplexMatrix transpose */
  SquareComplexMatrix transpose () const;

  /** SquareComplexMatrix transpose */
  SquareComplexMatrix conjugate () const;

  /** SquareComplexMatrix transpose */
  SquareComplexMatrix conjugateTranspose () const;

  /** SquareComplexMatrix additions (must have the same dimensions) */
  SquareComplexMatrix operator + (const SquareComplexMatrix & m) const;
  SquareComplexMatrix operator + (const HermitianMatrix & m) const;

  /** SquareComplexMatrix subtractions (must have the same dimensions) */
  SquareComplexMatrix operator - (const SquareComplexMatrix & m) const;
  SquareComplexMatrix operator - (const HermitianMatrix & m) const;

  /** SquareComplexMatrix multiplications (must have consistent dimensions) */
  ComplexMatrix operator * (const ComplexMatrix & m) const;
  SquareComplexMatrix operator * (const SquareComplexMatrix & m) const;
  SquareComplexMatrix operator * (const HermitianMatrix & m) const;
  SquareComplexMatrix operator * (const IdentityMatrix & m) const;

  /** SquareComplexMatrix integer power */
  SquareComplexMatrix power(const UnsignedInteger n) const;

  /** Multiplication with a Point (must have consistent dimensions) */
  ComplexCollection operator * (const ComplexCollection & p) const override;

  /** Multiplication with a Point (must have consistent dimensions) */
  ComplexCollection operator * (const ScalarCollection & p) const override;

  /** Multiplication with a Point (must have consistent dimensions) */
  ComplexCollection operator * (const Point & p) const override;

  /** Multiplication with a Complex */
  SquareComplexMatrix operator * (const Complex s) const;

  /** Division by a Complex*/
  SquareComplexMatrix operator / (const Complex s) const;


}

; /* class SquareComplexMatrix */


inline SquareComplexMatrix operator * (const Complex s,
                                       const SquareComplexMatrix & m)
{
  return m.operator * (s);
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SQUARECOMPLEXMATRIX_HXX */
