//                                               -*- C++ -*-
/**
 *  @brief ComplexMatrix implements the classical mathematical matrix with complex values
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
#ifndef OPENTURNS_COMPLEXMATRIX_HXX
#define OPENTURNS_COMPLEXMATRIX_HXX

#include "openturns/ComplexMatrixImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/Matrix.hxx"
#include "openturns/SymmetricMatrix.hxx"
#include "openturns/IdentityMatrix.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

class HermitianMatrix;
class TriangularComplexMatrix;

/**
 * @class ComplexMatrix
 *
 * ComplexMatrix implements the classical mathematical matrix with complex values
 */

class OT_API ComplexMatrix :
  public TypedInterfaceObject<ComplexMatrixImplementation>
{
  CLASSNAME

#ifndef SWIG
  /** Declaration of friend operators */
  friend ComplexMatrix operator * (const Complex s,
                                   const ComplexMatrix & m);
#endif


public:

  typedef Collection<Complex>                       ComplexCollection;
  typedef Collection<Scalar>                        ScalarCollection;
  typedef TypedInterfaceObject<ComplexMatrixImplementation>::Implementation     Implementation ;

  /** Default constructor */
  ComplexMatrix();

  /** Constructor with implementation */
  ComplexMatrix(const Implementation & i);

  /** Constructor with implementation */
  ComplexMatrix(const ComplexMatrixImplementation & i);

  /** Constructor with size (rowDim and colDim) */
  ComplexMatrix(const UnsignedInteger rowDim,
                const UnsignedInteger colDim);

  /** Constructor from range of external collection */
  template <class InputIterator>
  ComplexMatrix(const UnsignedInteger rowDim,
                const UnsignedInteger colDim,
                const InputIterator first,
                const InputIterator last);

  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  ComplexMatrix(const UnsignedInteger rowDim,
                const UnsignedInteger colDim,
                const ComplexCollection & elementsValues);


  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  ComplexMatrix(const UnsignedInteger rowDim,
                const UnsignedInteger colDim,
                const ScalarCollection & elementsValues);

  /** Constructor from hermitian matrix */
  ComplexMatrix(const HermitianMatrix & hermitian);

  /** Set small elements to zero */
  virtual ComplexMatrix clean(const Scalar threshold) const;

  /** Resolution of a linear system */
  ComplexMatrix solveLinearSystem(const ComplexMatrix & b,
                                  const Bool keepIntact = true);
  ComplexCollection solveLinearSystem(const ComplexCollection & b,
                                      const Bool keepIntact = true);

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

#ifndef SWIG
  /** Operator () gives access to the elements of the matrix (to modify these elements) */
  /** The element of the matrix is designated by its row number i and its column number j */
  Complex & operator () (const UnsignedInteger i,
                         const UnsignedInteger j);

  /** Operator () gives access to the elements of the matrix (read only) */
  /** The element of the matrix is designated by its row number i and its column number j */
  const Complex & operator () (const UnsignedInteger i,
                               const UnsignedInteger j) const;
#endif

  /** Get the dimensions of the matrix */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;

  /** ComplexMatrix transpose */
  ComplexMatrix transpose () const;

  /** ComplexMatrix conjugate */
  ComplexMatrix conjugate () const;

  /** ComplexMatrix conjugate and transpose */
  ComplexMatrix conjugateTranspose () const;

  /** Get the real part of the matrix */
  Matrix real() const;

  /** Get the imaginary part of the matrix */
  Matrix imag() const;

  /** ComplexMatrix additions (must have the same dimensions) */
  ComplexMatrix operator + (const ComplexMatrix & m) const;

  /** ComplexMatrix additions with Matrix */
  ComplexMatrix operator + (const Matrix & m) const;

  /** ComplexMatrix subtractions (must have the same dimensions) */
  ComplexMatrix operator - (const ComplexMatrix & m) const;

  /** ComplexMatrix subtractions with Matrix */
  ComplexMatrix operator - (const Matrix & m) const;

  /** ComplexMatrix multiplications (must have consistent dimensions) */
  ComplexMatrix operator * (const ComplexMatrix & m) const;

  /** ComplexMatrix multiplications with Hermitian Matrix */
  ComplexMatrix operator * (const HermitianMatrix & m) const;

  /** ComplexMatrix multiplications with Triangular Matrix */
  ComplexMatrix operator * (const TriangularComplexMatrix & m) const;

  /** ComplexMatrix multiplication with Matrix argument */
  virtual ComplexMatrix operator * (const Matrix & m) const;

  /** ComplexMatrix multiplication with SymmetricMatrix argument */
  ComplexMatrix operator * (const SymmetricMatrix & m) const;

  /** ComplexMatrix multiplication with IdentityMatrix argument */
  ComplexMatrix operator * (const IdentityMatrix & m) const;

  /** Multiplication with a Point  */
  virtual ComplexCollection operator* (const Point & point) const;

  /** Multiplication with a ScalarCollection  */
  virtual  ComplexCollection operator* (const ScalarCollection & collection) const;

  /** Multiplication with a ComplexCollection (must have consistent dimensions) */
  virtual ComplexCollection operator* (const ComplexCollection & collection) const;

  /** Multiplication with a Complex */
  ComplexMatrix operator * (const Complex s) const;

  /** Division by a Complex*/
  ComplexMatrix operator / (const Complex s) const;

  /** Comparison operators */
  Bool operator == (const ComplexMatrix & rhs) const;

  /** Empty returns true if there is no element in the matrix */
  Bool isEmpty() const;

  /** Low-level data access */
  const Complex * data() const;
  UnsignedInteger elementSize() const;
  UnsignedInteger stride(const UnsignedInteger dim) const;

}; /* class ComplexMatrix */

/** Declaration of friend operators */
inline ComplexMatrix operator * (const Complex s,
                                 const ComplexMatrix & m)
{
  return m.operator * (s);
}


/** Constructor from range of external collection */
template <class InputIterator>
ComplexMatrix::ComplexMatrix(const UnsignedInteger rowDim,
                             const UnsignedInteger colDim,
                             const InputIterator first,
                             const InputIterator last)
  : TypedInterfaceObject<ComplexMatrixImplementation>(new ComplexMatrixImplementation(rowDim, colDim, first, last))
{
  // Nothing to do
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPLEXMATRIX_HXX */
