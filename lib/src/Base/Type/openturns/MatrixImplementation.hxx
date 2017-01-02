//                                               -*- C++ -*-
/**
 *  @brief MatrixImplementation implements the classical mathematical Matrix
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_MATRIXIMPLEMENTATION_HXX
#define OPENTURNS_MATRIXIMPLEMENTATION_HXX

#include "openturns/PersistentCollection.hxx"
#include "openturns/Description.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MatrixImplementation
 *
 * MatrixImplementation implements the classical mathematical MatrixImplementation
 */

// Forward declaration of ComplexMatrixImplementation
class ComplexMatrixImplementation;

class OT_API MatrixImplementation
  : public PersistentCollection<NumericalScalar>

{
  CLASSNAME;

#ifndef SWIG
  /** Declaration of friend operators */
  friend MatrixImplementation operator * (const NumericalScalar s,
                                          const MatrixImplementation & matrix)
  {
    return matrix.operator * (s);
  }
#endif


public:

  typedef Collection<NumericalScalar>       NumericalScalarCollection;
  typedef Collection<NumericalComplex>      NumericalComplexCollection;

  /** Default constructor */
  MatrixImplementation();

  /** Constructor with size (rowDim and colDim) */
  MatrixImplementation(const UnsignedInteger rowDim,
                       const UnsignedInteger colDim);

  /** Constructor from range of external collection */
  template <class InputIterator>
  MatrixImplementation(const UnsignedInteger rowDim,
                       const UnsignedInteger colDim,
                       const InputIterator first,
                       const InputIterator last);

  /** Constructor from external collection */
  /** If the dimensions of the matrix and of the collection */
  /** do not correspond, either the collection is truncated */
  /** or the rest of the matrix is filled with zeros */
  MatrixImplementation(const UnsignedInteger rowDim,
                       const UnsignedInteger colDim,
                       const NumericalScalarCollection & elementsValues);

  /** Virtual constructor */
  virtual MatrixImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () gives access to the elements of the MatrixImplementation (to modify these elements) */
  /** The element of the MatrixImplementation is designated by its row number i and its column number j */
  NumericalScalar & operator () (const UnsignedInteger i,
                                 const UnsignedInteger j);

  /** Operator () gives access to the elements of the MatrixImplementation (read only) */
  /** The element of the MatrixImplementation is designated by its row number i and its column number j */
  const NumericalScalar & operator () (const UnsignedInteger i,
                                       const UnsignedInteger j) const;

  /** Get the dimensions of the MatrixImplementation */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;
  /** Dimension (for square matrices only */
  UnsignedInteger getDimension() const;

  /** MatrixImplementation transpose */
  MatrixImplementation transpose () const;

  /** Row extraction */
  const MatrixImplementation getRow(const UnsignedInteger rowIndex) const;
  const MatrixImplementation getRowSym(const UnsignedInteger rowIndex) const;
  /** Column extration */
  const MatrixImplementation getColumn(const UnsignedInteger columnIndex) const;
  const MatrixImplementation getColumnSym(const UnsignedInteger columnIndex) const;

  /** MatrixImplementation addition (must have the same dimensions) */
  MatrixImplementation operator + (const MatrixImplementation & matrix) const;

  /** MatrixImplementation substraction (must have the same dimensions) */
  MatrixImplementation operator - (const MatrixImplementation & matrix) const;

  /** MatrixImplementation multiplications (must have consistent dimensions) */
  MatrixImplementation genProd (const MatrixImplementation & matrix,
                                const Bool transposeLeft = false,
                                const Bool transposeRight = false) const;
  MatrixImplementation symProd (const MatrixImplementation & m,
                                const char symSide) const;

  /** MatrixImplementation integer power */
  MatrixImplementation genPower(const UnsignedInteger n) const;
  MatrixImplementation symPower(const UnsignedInteger n) const;

  /** Multiplications with a NumericalPoint (must have consistent dimensions) */
  NumericalPoint genVectProd (const NumericalPoint & pt,
                              const Bool transpose = false) const;
  NumericalPoint symVectProd (const NumericalPoint & pt) const;

  /** Using triangular matrix */
  NumericalScalarCollection triangularVectProd (const NumericalScalarCollection & pt,
      const char side = 'L') const;
  NumericalScalarCollection triangularVectProd (const NumericalPoint & pt,
      const char side = 'L') const;

  /** Multiplication with a NumericalScalar */
  MatrixImplementation operator * (const NumericalScalar s) const;

  /** Division by a NumericalScalar*/
  MatrixImplementation operator / (const NumericalScalar s) const;

  /** Symmetrize MatrixImplementation in case it is a symmetric matrix (stored as a triangular matrix) */
  void symmetrize() const;

  /** Triangularize MatrixImplementation in case it is a triangular matrix (stored as a square matrix) */
  void triangularize(const Bool isLowerTriangular) const;

  /** Resolution of a linear system in case of a rectangular matrix */
  NumericalPoint solveLinearSystemRect(const NumericalPoint & b,
                                       const Bool keepIntact = true);
  MatrixImplementation solveLinearSystemRect(const MatrixImplementation & b,
      const Bool keepIntact = true);

  /** Resolution of a linear system in case of a square matrix */
  NumericalPoint solveLinearSystemSquare(const NumericalPoint & b,
                                         const Bool keepIntact = true);
  MatrixImplementation solveLinearSystemSquare(const MatrixImplementation & b,
      const Bool keepIntact = true);

  /** Resolution of a linear system in case of a triangular matrix */
  NumericalPoint solveLinearSystemTri(const NumericalPoint & b,
                                      const Bool keepIntact = true,
                                      const Bool lower = true,
                                      const Bool transpose = false);

  MatrixImplementation solveLinearSystemTri(const MatrixImplementation & b,
      const Bool keepIntact = true,
      const Bool lower = true,
      const Bool transpose = false);

  /** Resolution of a linear system in case of a symmetric matrix */
  NumericalPoint solveLinearSystemSym(const NumericalPoint & b,
                                      const Bool keepIntact = true);
  MatrixImplementation solveLinearSystemSym(const MatrixImplementation & b,
      const Bool keepIntact = true);

  /** Resolution of a linear system in case of a covariance matrix */
  NumericalPoint solveLinearSystemCov(const NumericalPoint & b,
                                      const Bool keepIntact = true);
  MatrixImplementation solveLinearSystemCov(const MatrixImplementation & b,
      const Bool keepIntact = true);

  /** Triangular matrix product : side argument L/R for the position of the triangular matrix, up/lo to tell if it  */
  MatrixImplementation triangularProd(const MatrixImplementation & m,
                                      const char side = 'L',
                                      const char uplo = 'L') const;

  /** Compute determinant */
  NumericalScalar computeLogAbsoluteDeterminant(NumericalScalar & sign,
      const Bool keepIntact = true);
  NumericalScalar computeDeterminant(const Bool keepIntact = true);
  NumericalScalar computeLogAbsoluteDeterminantSym(NumericalScalar & sign,
      const Bool keepIntact = true);
  NumericalScalar computeDeterminantSym(const Bool keepIntact = true);

  /** Compute trace */
  NumericalScalar computeTrace() const;

  /** Compute eigenvalues */
  NumericalComplexCollection computeEigenValuesSquare(const Bool keepIntact = true);
  NumericalComplexCollection computeEVSquare(ComplexMatrixImplementation & v,
      const Bool keepIntact = true);
  NumericalPoint computeEigenValuesSym(const Bool keepIntact = true);
  NumericalPoint computeEVSym(MatrixImplementation & v,
                              const Bool keepIntact = true);

  /** Compute singular values */
  NumericalPoint computeSingularValues(const Bool keepIntact = true);

  /** Build the singular value decomposition */
  NumericalPoint computeSVD(MatrixImplementation & u,
                            MatrixImplementation & vT,
                            const Bool fullSVD = false,
                            const Bool keepIntact = true);

  /** Check if the matrix is symmetric */
  virtual Bool isSymmetric() const;

  /** Check if the matrix is SPD */
  virtual Bool isPositiveDefinite(const Bool keepIntact = true);

  /** Check if the matrix values belong to (-1;1) */
  virtual Bool hasUnitRange() const;

  /** Set small elements to zero */
  virtual MatrixImplementation clean(const NumericalScalar threshold) const;

  virtual MatrixImplementation cleanSym(const NumericalScalar threshold) const;

  /** Build the Cholesky factorization of the matrix */
  virtual MatrixImplementation computeCholesky(const Bool keepIntact = true);

  /** Update in-place the Cholesky factor L of an SPD matrix M given a rank-one update vv^T, ie L becomes Lnew such that LnewLnew^t = Mnew with Mnew = M + vv^t */
  static void CholeskyUpdate(MatrixImplementation & cholesky,
                             const NumericalPoint & vector);

  /** Downdate in-place the Cholesky factor L of an SPD matrix M given a rank-one downdate vv^T, ie L becomes Lnew such that LnewLnew^t = Mnew with Mnew = M - vv^t */
  static void CholeskyDowndate(MatrixImplementation & cholesky,
                               const NumericalPoint & vector);

  /** Build the QR factorization of the matrix */
  virtual MatrixImplementation computeQR(MatrixImplementation & R,
                                         const Bool fullQR = false,
                                         const Bool keepIntact = true);

  /** Compute the Gram matrix associated to the matrix */
  virtual MatrixImplementation computeGram(const Bool transpose = true) const;

  /** Comparison operators */
  Bool operator == (const MatrixImplementation & rhs) const;
  inline Bool operator != (const MatrixImplementation & rhs) const
  {
    return !((*this) == rhs);
  }

  /** Empty returns true if there is no element in the MatrixImplementation */
  Bool isEmpty() const;

  /** Returns true if triangular lower or upper */
  Bool isTriangular(Bool lower = true) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

  // These functions are only intended to be used by SWIG, DO NOT use them for your own purpose !
  // INTENTIONALY NOT DOCUMENTED
  const NumericalScalar * __baseaddress__ () const;
  UnsignedInteger __elementsize__ () const;
  UnsignedInteger __stride__ (UnsignedInteger dim) const;

protected:

  /** MatrixImplementation Dimensions */
  UnsignedInteger nbRows_;
  UnsignedInteger nbColumns_;

  /** Position conversion function : the indices i & j are used to compute the actual position of the element in the collection */
  inline UnsignedInteger convertPosition (const UnsignedInteger i,
                                          const UnsignedInteger j) const;



}; /* class MatrixImplementation */

inline UnsignedInteger MatrixImplementation::convertPosition (const UnsignedInteger i,
    const UnsignedInteger j) const
{
  return i + nbRows_ * j ;
}

/** Constructor from range of external collection */
template <class InputIterator>
MatrixImplementation::MatrixImplementation(const UnsignedInteger rowDim,
    const UnsignedInteger colDim,
    const InputIterator first,
    const InputIterator last)
  : PersistentCollection<NumericalScalar>(rowDim * colDim, 0.0),
    nbRows_(rowDim),
    nbColumns_(colDim)
{
  this->assign(first, last);
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MATRIXIMPLEMENTATION_HXX */
