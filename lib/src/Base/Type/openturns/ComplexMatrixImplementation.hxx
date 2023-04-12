//                                               -*- C++ -*-
/**
 *  @brief ComplexMatrixImplementation implements the Matrix class with complex values
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
#ifndef OPENTURNS_COMPLEXMATRIXIMPLEMENTATION_HXX
#define OPENTURNS_COMPLEXMATRIXIMPLEMENTATION_HXX

#include "openturns/PersistentCollection.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Point.hxx"
#include "openturns/MatrixImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComplexMatrixImplementation
 *
 * ComplexMatrixImplementation implements the classical mathematical ComplexMatrixImplementation
 */

class OT_API ComplexMatrixImplementation
  : public PersistentCollection<Complex>

{
  CLASSNAME

#ifndef SWIG
  /** Declaration of friend operators */
  friend ComplexMatrixImplementation operator * (const Complex s,
      const ComplexMatrixImplementation & matrix)
  {
    return matrix.operator * (s);
  }
#endif

public:

  typedef Collection<Complex>        ComplexCollection;
  typedef Collection<Scalar>         ScalarCollection;

  /** Default constructor */
  ComplexMatrixImplementation();

  /** Constructor with size (rowDim and colDim) */
  ComplexMatrixImplementation(const UnsignedInteger rowDim,
                              const UnsignedInteger colDim);

  /** Constructor from range of external collection */
  template <class InputIterator>
  ComplexMatrixImplementation(const UnsignedInteger rowDim,
                              const UnsignedInteger colDim,
                              const InputIterator first,
                              const InputIterator last);

  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  ComplexMatrixImplementation(const UnsignedInteger rowDim,
                              const UnsignedInteger colDim,
                              const ComplexCollection & elementsValues);


  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  ComplexMatrixImplementation(const UnsignedInteger rowDim,
                              const UnsignedInteger colDim,
                              const ScalarCollection & elementsValues);

  /** Constructor from MatrixImplementation */
  ComplexMatrixImplementation(const MatrixImplementation & matrix);

  /** Virtual constructor */
  ComplexMatrixImplementation * clone() const override;

  /** Resolution of a linear system in case of a rectangular matrix */
  ComplexCollection solveLinearSystemRectInPlace(const ComplexCollection & b);
  ComplexCollection solveLinearSystemRect(const ComplexCollection & b) const;
  ComplexMatrixImplementation solveLinearSystemRectInPlace(const ComplexMatrixImplementation & b);
  ComplexMatrixImplementation solveLinearSystemRect(const ComplexMatrixImplementation & b) const;

  /** Set small elements to zero */
  virtual ComplexMatrixImplementation clean(const Scalar threshold) const;

  virtual ComplexMatrixImplementation cleanHerm(const Scalar threshold) const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () gives access to the elements of the ComplexMatrixImplementation (to modify these elements) */
  /** The element of the ComplexMatrixImplementation is designated by its row number i and its column number j */
  Complex & operator () (const UnsignedInteger i,
                         const UnsignedInteger j);

  /** Operator () gives access to the elements of the ComplexMatrixImplementation (read only) */
  /** The element of the ComplexMatrixImplementation is designated by its row number i and its column number j */
  const Complex & operator () (const UnsignedInteger i,
                               const UnsignedInteger j) const;

  /** Get the dimensions of the ComplexMatrixImplementation */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;
  /** Dimension (for square matrices only */
  UnsignedInteger getDimension() const;

  /** ComplexMatrixImplementation transpose */
  ComplexMatrixImplementation transpose () const;
  ComplexMatrixImplementation transposeHerm () const;

  /** ComplexMatrixImplementation conjugate */
  ComplexMatrixImplementation conjugate () const;
  ComplexMatrixImplementation conjugateHerm () const;

  /** ComplexMatrixImplementation conjugateTranspose */
  ComplexMatrixImplementation conjugateTranspose () const;

  /** "Hermitianize" ComplexMatrixImplementation in case it is an hermitian matrix (stored as a triangular matrix) */
  void hermitianize() const;

  /** Get the real part of the matrix */
  MatrixImplementation realRect() const;
  MatrixImplementation realSym() const;

  /** Get the imaginary part of the matrix */
  MatrixImplementation imagRect() const;
  MatrixImplementation imagSym() const;

  /** Operator overload */
  /** ComplexMatrixImplementation addition (must have the same dimensions) */
  ComplexMatrixImplementation operator + (const ComplexMatrixImplementation & matrix) const;
  /** ComplexMatrixImplementation addition with MatrixImplementation */
  ComplexMatrixImplementation operator + (const MatrixImplementation & matrix) const;

  /** ComplexMatrixImplementation subtraction (must have the same dimensions) */
  ComplexMatrixImplementation operator - (const ComplexMatrixImplementation & matrix) const;
  /** ComplexMatrixImplementation subtraction with MatrixImplementation  */
  ComplexMatrixImplementation operator - (const MatrixImplementation & matrix) const;

  /** Multiplication with a Complex */
  ComplexMatrixImplementation operator * (const Complex s) const;

  /** Division by a Complex*/
  ComplexMatrixImplementation operator / (const Complex s) const;

  /** ComplexMatrixImplementation multiplications (must have consistent dimensions) */
  ComplexMatrixImplementation genProd(const ComplexMatrixImplementation & matrix) const;
  ComplexMatrixImplementation symProd(const ComplexMatrixImplementation & m,
                                      const char symSide) const;
  ComplexMatrixImplementation hermProd(const ComplexMatrixImplementation & m,
                                       const char hermSide) const;
  /** Triangular matrix product : side argument L/R for the position of the triangular matrix, up/lo to tell if it  */
  ComplexMatrixImplementation triangularProd(const ComplexMatrixImplementation & m,
      const char side = 'L',
      const char uplo = 'L') const;

  /** ComplexMatrixImplementation integer power */
  ComplexMatrixImplementation genPower(const UnsignedInteger n) const;
  ComplexMatrixImplementation symPower(const UnsignedInteger n) const;
  ComplexMatrixImplementation hermPower(const UnsignedInteger n) const;

  /** Multiplications with a ComplexCollection (must have consistent dimensions) */
  ComplexCollection genVectProd (const ComplexCollection & pt) const;
  ComplexCollection genVectProd (const ScalarCollection & pt) const;
  ComplexCollection genVectProd (const Point & pt) const;

  /** Using some optimization (for Hermitian matrix) */
  ComplexCollection hermVectProd (const ComplexCollection & pt) const;
  ComplexCollection hermVectProd (const ScalarCollection & pt) const;
  ComplexCollection hermVectProd (const Point & pt) const;

  /** Using triangular matrix */
  ComplexCollection triangularVectProd(const ComplexCollection & pt,
                                       const char side = 'L') const;
  ComplexCollection triangularVectProd(const ScalarCollection & pt,
                                       const char side = 'L') const;


  ComplexCollection triangularVectProd(const Point & pt,
                                       const char side = 'L') const;

  /** Check if the matrix is self-adjoint */
  virtual Bool isHermitian() const;

  /** Check if the matrix is HPD */
  virtual Bool isHermitianPositiveDefinite(const Bool keepIntact = true);

  /** Build the Cholesky factorization of the matrix */
  virtual ComplexMatrixImplementation computeCholesky(const Bool keepIntact = true);

  /** Comparison operators */
  using PersistentCollection::operator ==;
  Bool operator == (const ComplexMatrixImplementation & rhs) const;

  using PersistentCollection::operator !=;
  inline Bool operator != (const ComplexMatrixImplementation & rhs) const
  {
    return !((*this) == rhs);
  }

  /** Empty returns true if there is no element in the ComplexMatrixImplementation */
  Bool isEmpty() const;

  /** Returns true if triangular lower or upper */
  Bool isTriangular(Bool lower = true) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Low-level data access */
  UnsignedInteger stride(const UnsignedInteger dim) const;

protected:

  /** ComplexMatrixImplementation Dimensions */
  UnsignedInteger nbRows_;
  UnsignedInteger nbColumns_;

  /** Position conversion function : the indices i & j are used to compute the actual position of the element in the collection */
  inline UnsignedInteger convertPosition (const UnsignedInteger i,
                                          const UnsignedInteger j) const;

}; /* class ComplexMatrixImplementation */

inline UnsignedInteger ComplexMatrixImplementation::convertPosition (const UnsignedInteger i,
    const UnsignedInteger j) const
{
  return i + nbRows_ * j ;
}

/** Constructor from range of external collection */
template <class InputIterator>
ComplexMatrixImplementation::ComplexMatrixImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const InputIterator first,
    const InputIterator last)
  : PersistentCollection<Complex>(rowDim * colDim, 0.0),
    nbRows_(rowDim),
    nbColumns_(colDim)
{
  this->assign(first, last);
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPLEXMATRIXIMPLEMENTATION_HXX */
