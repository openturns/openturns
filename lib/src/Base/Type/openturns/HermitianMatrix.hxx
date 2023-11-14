//                                               -*- C++ -*-
/**
 *  @brief HermitianMatrix implements the classical mathematical hermitian matrix
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
#ifndef OPENTURNS_HERMITIANMATRIX_HXX
#define OPENTURNS_HERMITIANMATRIX_HXX

#include "openturns/SquareComplexMatrix.hxx"
#include "openturns/Collection.hxx"


BEGIN_NAMESPACE_OPENTURNS

class SymmetricMatrix;
class IdentityMatrix;
class SquareMatrix;
class Matrix;
class TriangularComplexMatrix;

/**
 * @class HermitianMatrix
 *
 * HermitianMatrix implements the classical mathematical hermitian matrix
 */

class OT_API HermitianMatrix :
  public SquareComplexMatrix
{
  CLASSNAME

#ifndef SWIG
  friend HermitianMatrix operator * (const Complex s,
                                     const HermitianMatrix & m);
#endif

public:

  typedef Collection<Complex>                       ComplexCollection;
  typedef Collection<Scalar>                        ScalarCollection;
  typedef TypedInterfaceObject<ComplexMatrixImplementation>::Implementation     Implementation ;

  /** Default constructor */
  HermitianMatrix();

  /** Constructor with size (dim, which is the same for nbRows_ and nbColumns_) */
  explicit HermitianMatrix(const UnsignedInteger dimension);

  /** Constructor with implementation */
  HermitianMatrix(const Implementation & i);

#ifndef SWIG
  /** Copy constructor, added to solve glitches with inheritance */
  HermitianMatrix(const HermitianMatrix & other);

  /** Assignment operator */
  HermitianMatrix & operator=(const HermitianMatrix & rhs);
#endif

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /** Get the dimension of the matrix */
  UnsignedInteger getDimension() const;

  /** HermitianMatrix transpose */
  HermitianMatrix transpose () const;

  /** HermitianMatrix conjugate */
  HermitianMatrix conjugate () const;

  /** HermitianMatrix conjugate and transpose */
  HermitianMatrix conjugateTranspose () const;

  /** Get the real part of the matrix */
  SymmetricMatrix real() const;

  /** Get the imaginary part of the matrix */
  SquareMatrix imag() const;

#ifndef SWIG
  /** Operator () gives access to the elements of the matrix (to modify these elements) */
  /** The element of the matrix is designated by its row number i and its column number j */
  Complex & operator () (const UnsignedInteger i,
                         const UnsignedInteger j) ;

  /** Operator () gives access to the elements of the matrix (read only) */
  /** The element of the matrix is designated by its row number i and its column number j */
  const Complex operator () (const UnsignedInteger i,
                             const UnsignedInteger j) const;
#endif

  /** Check if the internal representation is really hermitian */
  void checkHermitian() const;

  /** HermitianMatrix additions */
  HermitianMatrix operator + (const HermitianMatrix & m) const;

  /** ComplexMatrix additions */
  SquareComplexMatrix operator + (const SquareComplexMatrix & m) const;

  /** HermitianMatrix subtractions */
  HermitianMatrix operator - (const HermitianMatrix & m) const;

  /** ComplexMatrix subtractions */
  SquareComplexMatrix operator - (const SquareComplexMatrix & m) const;

#ifdef _MSC_VER   // VS2010 does not like 'using' being called after overloads
  using SquareComplexMatrix::operator *;
#endif

  /** Multiplication with a Complex */
  HermitianMatrix operator * (const Complex s) const;

  /** ComplexMatrix multiplications (must have consistent dimensions) */
  ComplexMatrix operator * (const ComplexMatrix & m) const;

  /** ComplexMatrix multiplications (must have consistent dimensions) */
  SquareComplexMatrix operator * (const SquareComplexMatrix & m) const;

  /** HermitianMatrix multiplications  */
  SquareComplexMatrix operator * (const HermitianMatrix & m) const;

  /** TriangularComplexMatrix multiplications  */
  SquareComplexMatrix operator * (const TriangularComplexMatrix & m) const;

  /** Real SquareMatrix multiplications */
  SquareComplexMatrix operator * (const SquareMatrix & m) const;

  /** Real Matrix multiplications (must have consistent dimensions) */
  ComplexMatrix operator * (const Matrix & m) const override;

  /** Real SymmetricMatrix multiplications  */
  SquareComplexMatrix operator * (const SymmetricMatrix & m) const;

  /** Real IdentityMatrix multiplications  */
  HermitianMatrix operator * (const IdentityMatrix & m) const;

  /** Multiplication with a NumericaComplexCollection (must have consistent dimensions) */
  ComplexCollection operator * (const ComplexCollection & p) const override;

  /** Multiplication with a NumericaScalarCollection (must have consistent dimensions) */
  ComplexCollection operator * (const ScalarCollection & p) const override;

  /** Multiplication with a Point (must have consistent dimensions) */
  ComplexCollection operator * (const Point & p) const override;

#ifndef _MSC_VER
  using SquareComplexMatrix::operator *;
#endif

  /** HermitianMatrix integer power */
  HermitianMatrix power(const UnsignedInteger n) const;

  /** Division by a Complex*/
  HermitianMatrix operator / (const Complex s) const;

  /** Compute the Cholesky factor  */
  TriangularComplexMatrix computeCholesky(const Bool keepIntact = true);

private:

  /** Check if one needs to symmetrized the internal representation of the tensor */
  mutable Bool hasBeenHermitianized_;

}

; /* class HermitianMatrix */


inline HermitianMatrix operator * (const Complex s,
                                   const HermitianMatrix & m)
{
  return m.operator * (s);
}


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HERMITIANMATRIX_HXX */
