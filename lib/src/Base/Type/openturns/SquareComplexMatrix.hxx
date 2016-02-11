//                                               -*- C++ -*-
/**
 *  @brief SquareComplexMatrix implements the classical mathematical square matrix
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
  CLASSNAME;

#ifndef SWIG
  friend SquareComplexMatrix operator * (const NumericalComplex s,
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
                      const NumericalComplexCollection & elementsValues);

  /** Constructor with implementation */
  SquareComplexMatrix(const Implementation & i);

  /** String converter */
  String __repr__() const;

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

  /** SquareComplexMatrix substractions (must have the same dimensions) */
  SquareComplexMatrix operator - (const SquareComplexMatrix & m) const;
  SquareComplexMatrix operator - (const HermitianMatrix & m) const;

  /** SquareComplexMatrix multiplications (must have consistent dimensions) */
  ComplexMatrix operator * (const ComplexMatrix & m) const;
  SquareComplexMatrix operator * (const SquareComplexMatrix & m) const;
  SquareComplexMatrix operator * (const HermitianMatrix & m) const;
  SquareComplexMatrix operator * (const IdentityMatrix & m) const;

  /** SquareComplexMatrix integer power */
  SquareComplexMatrix power(const UnsignedInteger n) const;

  /** Multiplication with a NumericalPoint (must have consistent dimensions) */
  NumericalComplexCollection operator * (const NumericalComplexCollection & p) const;

  /** Multiplication with a NumericalPoint (must have consistent dimensions) */
  NumericalComplexCollection operator * (const NumericalScalarCollection & p) const;

  /** Multiplication with a NumericalPoint (must have consistent dimensions) */
  NumericalComplexCollection operator * (const NumericalPoint & p) const;

  /** Multiplication with a NumericalComplex */
  SquareComplexMatrix operator * (const NumericalComplex s) const;

  /** Division by a NumericalComplex*/
  SquareComplexMatrix operator / (const NumericalComplex s) const;


}

; /* class SquareComplexMatrix */


inline SquareComplexMatrix operator * (const NumericalComplex s,
                                       const SquareComplexMatrix & m)
{
  return m.operator * (s);
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SQUARECOMPLEXMATRIX_HXX */
