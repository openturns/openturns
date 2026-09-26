//                                               -*- C++ -*-
/**
 *  @brief AlgebraEngine provides free functions for linear algebra operations
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ALGEBRAENGINE_HXX
#define OPENTURNS_ALGEBRAENGINE_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DataContainer.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AlgebraEngine
 *
 * AlgebraEngine provides free functions for linear algebra operations
 * on DataContainer objects, delegating to LAPACK/BLAS routines.
 * These functions operate on raw pointers from DataContainer::data(),
 * enabling zero-copy interoperability between different container types.
 */
class OT_API AlgebraEngine
{
public:

  /* ===== BLAS Level 1 operations ===== */

  /** Dot product of two vectors: result = a^T * b */
  static Scalar Dot(const DataContainer & a, const DataContainer & b);

  /** Euclidean norm: result = ||v||_2 */
  static Scalar Norm(const DataContainer & v);

  /** l1-norm: result = ||v||_1 */
  static Scalar Norm1(const DataContainer & v);

  /** linf-norm: result = ||v||_inf */
  static Scalar NormInf(const DataContainer & v);

  /** Scale: result = alpha * x */
  static DataContainer Scale(const DataContainer & x, Scalar alpha);

  /** Axpy: result = alpha * x + y */
  static DataContainer Axpy(Scalar alpha, const DataContainer & x, const DataContainer & y);

  /* ===== BLAS Level 2 operations ===== */

  /** Matrix-vector product: result = A * x (column-major A) */
  static DataContainer MatrixPointProduct(const DataContainer & A, const DataContainer & x);

  /** Symmetric matrix-vector product: result = A * x (column-major symmetric A) */
  static DataContainer SymMatrixPointProduct(const DataContainer & A, const DataContainer & x);

  /* ===== BLAS Level 3 operations ===== */

  /** Matrix multiplication: result = A * B (column-major) */
  static DataContainer MatrixProduct(const DataContainer & A, const DataContainer & B);

  /** Symmetric matrix product: result = A * B * A^T or A^T * B * A */
  static DataContainer SymProd(const DataContainer & A, const DataContainer & B, char symSide);

  /** Triangular matrix product */
  static DataContainer TriangularProd(const DataContainer & A, const DataContainer & B, char side = 'L', char uplo = 'L');

  /* ===== LAPACK decompositions ===== */

  /** Cholesky factorization: L * L^T = A (lower triangular) */
  static DataContainer ComputeCholesky(const DataContainer & A);

  /** QR factorization: A = Q * R */
  static void ComputeQR(const DataContainer & A, DataContainer & Q, DataContainer & R, Bool fullQR = false);

  /** SVD: A = U * S * V^T */
  static void ComputeSVD(const DataContainer & A, DataContainer & U, DataContainer & S, DataContainer & VT, Bool fullSVD = false);

  /** LU factorization with pivoting */
  static void ComputeLU(const DataContainer & A, DataContainer & L, DataContainer & U);

  /* ===== Linear solvers ===== */

  /** Solve Ax = b for square non-singular A */
  static DataContainer SolveLinearSystem(const DataContainer & A, const DataContainer & b);

  /** Solve Ax = b for symmetric positive definite A */
  static DataContainer SolveLinearSystemSPD(const DataContainer & A, const DataContainer & b);

  /** Solve Ax = b for triangular A */
  static DataContainer SolveLinearSystemTriangular(const DataContainer & A, const DataContainer & b, Bool lower = true, Bool transpose = false);

  /** Solve rectangular system via least-squares (pseudoinverse) */
  static DataContainer SolveLinearSystemRectangular(const DataContainer & A, const DataContainer & b);

  /* ===== Matrix properties ===== */

  /** Compute determinant */
  static Scalar ComputeDeterminant(const DataContainer & A);

  /** Compute log-absolute-determinant with sign */
  static Scalar ComputeLogAbsoluteDeterminant(const DataContainer & A, Scalar & signOut);

  /** Compute trace */
  static Scalar ComputeTrace(const DataContainer & A);

  /** Compute eigenvalues of a square matrix */
  static DataContainer ComputeEigenValuesSquare(const DataContainer & A);

  /** Compute eigenvalues of a symmetric matrix */
  static DataContainer ComputeEigenValuesSymmetric(const DataContainer & A);

  /** Compute singular values */
  static DataContainer ComputeSingularValues(const DataContainer & A);

  /** Check if matrix is symmetric */
  static Bool IsSymmetric(const DataContainer & A, Scalar tolerance = 0.0);

  /** Check if matrix is positive definite */
  static Bool IsPositiveDefinite(const DataContainer & A);

  /** Frobenius norm */
  static Scalar FrobeniusNorm(const DataContainer & A);

  /** Compute Gram matrix: A^T * A or A * A^T */
  static DataContainer ComputeGram(const DataContainer & A, Bool transpose = true);

  /** Transpose matrix */
  static DataContainer Transpose(const DataContainer & A);

  /** Compute inverse of a square matrix */
  static DataContainer Inverse(const DataContainer & A);

  /** Compute inverse of a symmetric positive definite matrix */
  static DataContainer InverseSPD(const DataContainer & A);

  /** Compute sum of all elements */
  static Scalar SumElements(const DataContainer & A);

  /** Clean small elements */
  static DataContainer Clean(const DataContainer & A, Scalar threshold);

  /* ===== Block-based operations for out-of-core processing ===== */

  /** Block matrix multiplication: C = A * B in blocks of blockSize rows/cols */
  static DataContainer MatrixProductBlockwise(const DataContainer & A,
      const DataContainer & B,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block Cholesky: L * L^T = A in blocks of blockSize */
  static DataContainer ComputeCholeskyBlockwise(const DataContainer & A,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block linear solve: Ax = b in blocks of blockSize */
  static DataContainer SolveLinearSystemBlockwise(const DataContainer & A,
      const DataContainer & b,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block QR factorization: A = Q * R in blocks of blockSize */
  static void ComputeQRBlockwise(const DataContainer & A,
                                 DataContainer & Q,
                                 DataContainer & R,
                                 Bool fullQR = false,
                                 UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block LU factorization with partial pivoting */
  static void ComputeLUBlockwise(const DataContainer & A,
                                 DataContainer & P,
                                 DataContainer & L,
                                 DataContainer & U,
                                 UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block triangular solve: T * x = b or T^T * x = b */
  static DataContainer SolveLinearSystemTriangularBlockwise(const DataContainer & T,
      const DataContainer & b,
      Bool lower = true,
      Bool transpose = false,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block Gram matrix: A^T * A (or A * A^T) in blocks of blockSize */
  static DataContainer ComputeGramBlockwise(const DataContainer & A,
      Bool transpose = true,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block determinant via block LU factorization */
  static Scalar ComputeDeterminantBlockwise(const DataContainer & A,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block log-absolute-determinant with sign via block LU factorization */
  static Scalar ComputeLogAbsoluteDeterminantBlockwise(const DataContainer & A,
      Scalar & signOut,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block matrix inverse via block LU factorization */
  static DataContainer InverseBlockwise(const DataContainer & A,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block inverse of symmetric positive definite matrix via block Cholesky */
  static DataContainer InverseSPDBlockwise(const DataContainer & A,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

  /** Block least-squares solve via block QR factorization */
  static DataContainer SolveLinearSystemRectangularBlockwise(const DataContainer & A,
      const DataContainer & b,
      UnsignedInteger blockSize = ResourceMap::GetAsUnsignedInteger("AlgebraEngine-DefaultBlockSize"));

}; /* class AlgebraEngine */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ALGEBRAENGINE_HXX */
