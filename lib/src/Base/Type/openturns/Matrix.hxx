//                                               -*- C++ -*-
/**
 *  @brief Matrix implements the classical mathematical matrix
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
#ifndef OPENTURNS_MATRIX_HXX
#define OPENTURNS_MATRIX_HXX

#include "openturns/MatrixImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS


class CovarianceMatrix;
class IdentityMatrix;
class SquareMatrix;
class SymmetricMatrix;
class Sample;

/**
 * @class Matrix
 *
 * Matrix implements the classical mathematical matrix
 */

class OT_API Matrix :
  public TypedInterfaceObject<MatrixImplementation>
{
  CLASSNAME

#ifndef SWIG
  /** Declaration of friend operators */
  friend Matrix operator * (const Scalar s,
                            const Matrix & m);
#endif


public:

  typedef Collection<Scalar>       ScalarCollection;
  typedef Collection<Complex>      ComplexCollection;

  /** Default constructor */
  Matrix();

  /** Constructor with implementation */
  Matrix(const Implementation & i);

  /** Constructor with implementation */
  Matrix(const MatrixImplementation & i);

  /** Constructor with size (rowDim and colDim) */
  Matrix(const UnsignedInteger rowDim,
         const UnsignedInteger colDim);

  /** Constructor from range of external collection */
  template <class InputIterator>
  Matrix(const UnsignedInteger rowDim,
         const UnsignedInteger colDim,
         const InputIterator first,
         const InputIterator last);

  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  Matrix(const UnsignedInteger rowDim,
         const UnsignedInteger colDim,
         const ScalarCollection & elementsValues);

  /** Constructor from symmetric matrix */
  Matrix(const SymmetricMatrix & symmetric);

  /** Set small elements to zero */
  virtual Matrix clean(const Scalar threshold) const;

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

  /** Get the dimensions of the matrix */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;

  /** Matrix transpose */
  Matrix transpose() const;

  /** Matrix reshape */
  Matrix reshape(const UnsignedInteger newRowDim,
                 const UnsignedInteger newColDim) const;
  void reshapeInPlace(const UnsignedInteger newRowDim,
                      const UnsignedInteger newColDim);

  /** Row extraction */
  const Matrix getRow(const UnsignedInteger rowIndex) const;
  /** Column extration */
  const Matrix getColumn(const UnsignedInteger columnIndex) const;

  /** Matrix additions (must have the same dimensions) */
  Matrix operator + (const Matrix & m) const;
  Matrix operator + (const SymmetricMatrix & m) const;

  /** Matrix subtractions (must have the same dimensions) */
  Matrix operator - (const Matrix & m) const;
  Matrix operator - (const SymmetricMatrix & m) const;

  /** Matrix multiplications (must have consistent dimensions) */
  Matrix operator * (const Matrix & m) const;
  Matrix operator * (const SymmetricMatrix & m) const;
  Matrix operator * (const IdentityMatrix & m) const;

  /** Multiplication with a Sample (must have consistent dimensions) */
  Sample operator * (const Sample & sample) const;

  /** Multiplication with a Point (must have consistent dimensions) */
  Point operator * (const Point & pt) const;

  /** Multiplication with a Scalar */
  Matrix operator * (const Scalar s) const;

  /** Division by a Scalar*/
  Matrix operator / (const Scalar s) const;

  /** Resolution of a linear system */
  Point solveLinearSystem(const Point & b,
                          const Bool keepIntact = true);
  Matrix solveLinearSystem(const Matrix & b,
                           const Bool keepIntact = true);

  /** Compute singular values */
  Point computeSingularValues(const Bool keepIntact = true);

  Point computeSVD(Matrix & uOut,
                   Matrix & vTOut,
                   const Bool fullSVD = false,
                   const Bool keepIntact = true);

  /** Build the QR factorization of the matrix */
  virtual Matrix computeQR(Matrix & ROut,
                           const Bool fullQR = false,
                           const Bool keepIntact = true);

  /** Compute the associated Gram matrix */
  virtual CovarianceMatrix computeGram(const Bool transpose = true) const;

  /** Comparison operators */
  Bool operator == (const Matrix & rhs) const;

  /** Empty returns true if there is no element in the matrix */
  Bool isEmpty() const;

  /** Low-level data access */
  const Scalar * data () const;
  UnsignedInteger elementSize() const;
  UnsignedInteger stride(const UnsignedInteger dim) const;

}; /* class Matrix */

/** Declaration of friend operators */
inline Matrix operator * (const Scalar s,
                          const Matrix & m)
{
  return m.operator * (s);
}


/** Constructor from range of external collection */
template <class InputIterator>
Matrix::Matrix(const UnsignedInteger rowDim,
               const UnsignedInteger colDim,
               const InputIterator first,
               const InputIterator last)
  : TypedInterfaceObject<MatrixImplementation>(new MatrixImplementation(rowDim, colDim, first, last))
{
  // Nothing to do
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MATRIX_HXX */
