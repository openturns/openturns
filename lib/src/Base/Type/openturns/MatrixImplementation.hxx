//                                               -*- C++ -*-
/**
 *  @brief MatrixImplementation implements the classical mathematical Matrix
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
#ifndef OPENTURNS_MATRIXIMPLEMENTATION_HXX
#define OPENTURNS_MATRIXIMPLEMENTATION_HXX

#include "openturns/PersistentCollection.hxx"
#include "openturns/Description.hxx"
#include "openturns/Point.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class MatrixImplementation
 *
 * MatrixImplementation implements the classical mathematical MatrixImplementation
 */

// Forward declaration of ComplexMatrixImplementation
class ComplexMatrixImplementation;
class Sample;

class OT_API MatrixImplementation
  : public PersistentCollection<Scalar>

{
  CLASSNAME

#ifndef SWIG
  /** Declaration of friend operators */
  friend MatrixImplementation operator * (const Scalar s,
                                          const MatrixImplementation & matrix)
  {
    return matrix.operator * (s);
  }
#endif


public:

  typedef Collection<Scalar>       ScalarCollection;
  typedef Collection<Complex>      ComplexCollection;

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
                       const ScalarCollection & elementsValues);

  /** Virtual constructor */
  MatrixImplementation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () gives access to the elements of the MatrixImplementation (to modify these elements) */
  /** The element of the MatrixImplementation is designated by its row number i and its column number j */
  Scalar & operator () (const UnsignedInteger i,
                        const UnsignedInteger j);

  /** Operator () gives access to the elements of the MatrixImplementation (read only) */
  /** The element of the MatrixImplementation is designated by its row number i and its column number j */
  const Scalar & operator () (const UnsignedInteger i,
                              const UnsignedInteger j) const;

  /** Get the dimensions of the MatrixImplementation */
  /** Number of rows */
  UnsignedInteger getNbRows() const;
  /** Number of columns */
  UnsignedInteger getNbColumns() const;
  /** Dimension (for square matrices only */
  UnsignedInteger getDimension() const;

  /** MatrixImplementation transpose */
  MatrixImplementation transpose() const;

  /** Resize */
  using Collection<Scalar>::resize;
  void resize(const UnsignedInteger newRowDim, const UnsignedInteger newColDim);

  /** MatrixImplementation reshape */
  MatrixImplementation reshape(const UnsignedInteger newRowDim,
                               const UnsignedInteger newColDim) const;
  void reshapeInPlace(const UnsignedInteger newRowDim,
                      const UnsignedInteger newColDim);


  /** Row extraction */
  const MatrixImplementation getRow(const UnsignedInteger rowIndex) const;
  const MatrixImplementation getRowSym(const UnsignedInteger rowIndex) const;
  /** Column extration */
  const MatrixImplementation getColumn(const UnsignedInteger columnIndex) const;
  const MatrixImplementation getColumnSym(const UnsignedInteger columnIndex) const;

  /** MatrixImplementation addition (must have the same dimensions) */
  MatrixImplementation operator + (const MatrixImplementation & matrix) const;

  /** In-place MatrixImplementation addition (must have the same dimensions) */
  MatrixImplementation & operator += (const MatrixImplementation & matrix);

  /** MatrixImplementation subtraction (must have the same dimensions) */
  MatrixImplementation operator - (const MatrixImplementation & matrix) const;

  /** In-place MatrixImplementation subtraction (must have the same dimensions) */
  MatrixImplementation & operator -= (const MatrixImplementation & matrix);

  /** MatrixImplementation multiplications (must have consistent dimensions) */
  MatrixImplementation genProd (const MatrixImplementation & matrix,
                                const Bool transposeLeft = false,
                                const Bool transposeRight = false) const;
  MatrixImplementation symProd (const MatrixImplementation & m,
                                const char symSide) const;

  /** MatrixImplementation integer power */
  MatrixImplementation genPower(const UnsignedInteger n) const;
  MatrixImplementation symPower(const UnsignedInteger n) const;

  /** Multiplications with a SampleImplementation (must have consistent dimensions) */
  Sample genSampleProd (const Sample & sample,
                        const Bool transposeMatrix,
                        const Bool transposeSample,
                        const char side) const;

  /** Multiplications with a Point (must have consistent dimensions) */
  Point genVectProd (const Point & pt,
                     const Bool transpose = false) const;
  Point symVectProd (const Point & pt) const;

  /** Using triangular matrix */
#ifndef SWIG
  ScalarCollection triangularVectProd (const ScalarCollection & pt,
                                       const char side = 'L',
                                       const Bool transpose = false) const;
#endif
  Point triangularVectProd (const Point & pt,
                            const char side = 'L',
                            const Bool transpose = false) const;

  /** Multiplication with a Scalar */
  MatrixImplementation operator * (const Scalar s) const;

  /** In-place Multiplication with a Scalar */
  MatrixImplementation & operator *= (const Scalar s);

  /** Division by a Scalar*/
  MatrixImplementation operator / (const Scalar s) const;

  /** In-place Division by a Scalar*/
  MatrixImplementation & operator /= (const Scalar s);

  /** Symmetrize MatrixImplementation in case it is a symmetric matrix (stored as a triangular matrix) */
  void symmetrize() const;

  /** Triangularize MatrixImplementation in case it is a triangular matrix (stored as a square matrix) */
  void triangularize(const Bool isLowerTriangular) const;

  /** Resolution of a linear system in case of a rectangular matrix */
  Point solveLinearSystemRect(const Point & b,
                              const Bool keepIntact = true);
  MatrixImplementation solveLinearSystemRect(const MatrixImplementation & b,
      const Bool keepIntact = true);

  /** Resolution of a linear system in case of a square matrix */
  Point solveLinearSystemSquareInPlace(const Point & b);
  Point solveLinearSystemSquare(const Point & b) const;
  MatrixImplementation solveLinearSystemSquareInPlace(const MatrixImplementation & b);
  MatrixImplementation solveLinearSystemSquare(const MatrixImplementation & b) const;

  /** Resolution of a linear system in case of a triangular matrix */
  Point solveLinearSystemTri(const Point & b,
                             const Bool lower = true,
                             const Bool transpose = false) const;
  Point solveLinearSystemTriInPlace(const Point & b,
                                    const Bool lower = true,
                                    const Bool transpose = false);

  MatrixImplementation solveLinearSystemTri(const MatrixImplementation & b,
                                            const Bool lower = true,
                                            const Bool transpose = false) const;
  MatrixImplementation solveLinearSystemTriInPlace(const MatrixImplementation & b,
                                                  const Bool lower = true,
                                                  const Bool transpose = false);

  /** Resolution of a linear system in case of a symmetric matrix */
  Point solveLinearSystemSym(const Point & b,
                             const Bool keepIntact = true);
  MatrixImplementation solveLinearSystemSym(const MatrixImplementation & b,
      const Bool keepIntact = true);

  /** Resolution of a linear system in case of a covariance matrix */
  Point solveLinearSystemCov(const Point & b,
                             const Bool keepIntact = true);
  MatrixImplementation solveLinearSystemCov(const MatrixImplementation & b,
      const Bool keepIntact = true);

  /** Triangular matrix product : side argument L/R for the position of the triangular matrix, up/lo to tell if it  */
  MatrixImplementation triangularProd(const MatrixImplementation & m,
                                      const char side = 'L',
                                      const char uplo = 'L') const;

  /** Compute determinant */
  Scalar computeLogAbsoluteDeterminant(Scalar & signOut,
                                       const Bool keepIntact = true);
  Scalar computeDeterminant(const Bool keepIntact = true);
  Scalar computeLogAbsoluteDeterminantSym(Scalar & signOut,
                                          const Bool keepIntact = true);
  Scalar computeDeterminantSym(const Bool keepIntact = true);

  /** Compute trace */
  Scalar computeTrace() const;

  /** Compute eigenvalues */
  ComplexCollection computeEigenValuesSquare(const Bool keepIntact = true);
  ComplexCollection computeEVSquare(ComplexMatrixImplementation & vOut,
                                    const Bool keepIntact = true);
  Point computeEigenValuesSym(const Bool keepIntact = true);
  Point computeEVSym(MatrixImplementation & vOut,
                     const Bool keepIntact = true);
  /** Compute the largest eigenvalue module using power iterations */
  Bool computeLargestEigenValueModuleSquare(Scalar & maximumModule,
      const UnsignedInteger maximumIterations = ResourceMap::GetAsUnsignedInteger("Matrix-LargestEigenValueIterations"),
      const Scalar epsilon = ResourceMap::GetAsScalar("Matrix-LargestEigenValueRelativeError")) const;
  Bool computeLargestEigenValueModuleSym(Scalar & maximumModule,
                                         const UnsignedInteger maximumIterations = ResourceMap::GetAsUnsignedInteger("Matrix-LargestEigenValueIterations"),
                                         const Scalar epsilon = ResourceMap::GetAsScalar("Matrix-LargestEigenValueRelativeError")) const;

  /** Compute singular values */
  Point computeSingularValues(const Bool keepIntact = true);

  /** Build the singular value decomposition */
  Point computeSVD(MatrixImplementation & uOut,
                   MatrixImplementation & vTOut,
                   const Bool fullSVD = false,
                   const Bool keepIntact = true);

  /** Check if the matrix is symmetric */
  virtual Bool isSymmetric() const;

  /** Check if the matrix is SPD */
  virtual Bool isPositiveDefinite() const;

  /** Check if the matrix values belong to (-1;1) */
  virtual Bool hasUnitRange() const;

  /** Set small elements to zero */
  virtual MatrixImplementation clean(const Scalar threshold) const;

  virtual MatrixImplementation cleanSym(const Scalar threshold) const;

  /** Build the Cholesky factorization of the matrix */
  virtual MatrixImplementation computeCholesky(const Bool keepIntact = true);

  /** Build the regularized Cholesky factorization of the matrix */
  virtual MatrixImplementation computeRegularizedCholesky() const;

#ifndef SWIG
  /** Update in-place the Cholesky factor L of an SPD matrix M given a rank-one update vv^T, ie L becomes Lnew such that LnewLnew^t = Mnew with Mnew = M + vv^t */
  static void CholeskyUpdate(MatrixImplementation & cholesky,
                             const Point & vector);

  /** Downdate in-place the Cholesky factor L of an SPD matrix M given a rank-one downdate vv^T, ie L becomes Lnew such that LnewLnew^t = Mnew with Mnew = M - vv^t */
  static void CholeskyDowndate(MatrixImplementation & cholesky,
                               const Point & vector);
#endif

  /** Build the QR factorization of the matrix */
  virtual MatrixImplementation computeQR(MatrixImplementation & ROut,
                                         const Bool fullQR = false,
                                         const Bool keepIntact = true);

  /** Compute the Gram matrix associated to the matrix. If transpose == true, compute M^T.M, else M.M^T. */
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
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

  /** Low-level data access */
  UnsignedInteger stride(const UnsignedInteger dim) const;

  /** Extract diagonal */
  MatrixImplementation getDiagonal(const SignedInteger k = 0) const;

  /** Fill diagonal with values */
  void setDiagonal(const Point &diag, const SignedInteger k = 0);

  /** Hadamard product aka elementwise product */
  MatrixImplementation computeHadamardProduct(const MatrixImplementation &other) const;

  /** Sum all coefficients */
  virtual Scalar computeSumElements() const;

  /** All elements are squared */
  void squareElements();

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
  : PersistentCollection<Scalar>(rowDim * colDim, 0.0),
    nbRows_(rowDim),
    nbColumns_(colDim)
{
  this->assign(first, last);
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MATRIXIMPLEMENTATION_HXX */
