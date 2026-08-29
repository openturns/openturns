//                                               -*- C++ -*-
/**
 *  @brief AlgebraEngine implements free functions for linear algebra operations
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
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

#include "openturns/AlgebraEngine.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* ===== BLAS Level 1 operations ===== */

Scalar AlgebraEngine::Dot(const DataContainer & a, const DataContainer & b)
{
  const UnsignedInteger n = a.getSize();
  if (n != b.getSize()) throw InvalidDimensionException(HERE) << "Cannot compute dot product: vectors have incompatible sizes " << n << " and " << b.getSize();
  if (n == 0) return 0.0;
  int intN = static_cast<int>(n);
  int one = 1;
  return ddot_(&intN, const_cast<double *>(a.data()), &one, const_cast<double *>(b.data()), &one);
}

Scalar AlgebraEngine::Norm(const DataContainer & v)
{
  const UnsignedInteger n = v.getSize();
  if (n == 0) return 0.0;
  int intN = static_cast<int>(n);
  int one = 1;
  return dnrm2_(&intN, const_cast<double *>(v.data()), &one);
}

Scalar AlgebraEngine::Norm1(const DataContainer & v)
{
  const UnsignedInteger n = v.getSize();
  if (n == 0) return 0.0;
  int intN = static_cast<int>(n);
  int one = 1;
  return dasum_(&intN, const_cast<double *>(v.data()), &one);
}

Scalar AlgebraEngine::NormInf(const DataContainer & v)
{
  const UnsignedInteger n = v.getSize();
  Scalar result = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    const Scalar absVal = std::abs(v[i]);
    if (absVal > result) result = absVal;
  }
  return result;
}

DataContainer AlgebraEngine::Scale(const DataContainer & x, Scalar alpha)
{
  const UnsignedInteger n = x.getSize();
  DataContainer result(n, 0.0);
  std::copy(x.data(), x.data() + n, result.data());
  if (n == 0) return result;
  int intN = static_cast<int>(n);
  int one = 1;
  dscal_(&intN, &alpha, result.data(), &one);
  return result;
}

DataContainer AlgebraEngine::Axpy(Scalar alpha, const DataContainer & x, const DataContainer & y)
{
  const UnsignedInteger n = x.getSize();
  if (n != y.getSize()) throw InvalidDimensionException(HERE) << "Cannot compute axpy: vectors have incompatible sizes " << n << " and " << y.getSize();
  DataContainer result(n, 0.0);
  std::copy(y.data(), y.data() + n, result.data());
  if (n == 0) return result;
  int intN = static_cast<int>(n);
  int one = 1;
  daxpy_(&intN, &alpha, const_cast<double *>(x.data()), &one, result.data(), &one);
  return result;
}

/* ===== BLAS Level 2 operations ===== */

DataContainer AlgebraEngine::MatrixPointProduct(const DataContainer & A, const DataContainer & x)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (x.getSize() != n) throw InvalidDimensionException(HERE) << "Cannot compute matrix-vector product: vector size=" << x.getSize() << " must match matrix columns=" << n;
  DataContainer result(m, 1, 0.0);
  if (m == 0 || n == 0) return result;
  int intM = static_cast<int>(m);
  int intN = static_cast<int>(n);
  int lda = intM;
  int one = 1;
  double alpha = 1.0;
  double beta = 0.0;
  int ltrans = 1;
  dgemv_((char *)"N", &intM, &intN, &alpha, const_cast<double *>(A.data()), &lda, const_cast<double *>(x.data()), &one, &beta, result.data(), &one, &ltrans);
  return result;
}

DataContainer AlgebraEngine::SymMatrixPointProduct(const DataContainer & A, const DataContainer & x)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for symmetric matrix-vector product";
  if (x.getSize() != n) throw InvalidDimensionException(HERE) << "Cannot compute symmetric matrix-vector product: vector size=" << x.getSize() << " must match matrix dimension=" << n;
  DataContainer result(n, 1, 0.0);
  if (n == 0) return result;
  int intN = static_cast<int>(n);
  int lda = intN;
  int one = 1;
  double alpha = 1.0;
  double beta = 0.0;
  int luplo = 1;
  dsymv_((char *)"L", &intN, &alpha, const_cast<double *>(A.data()), &lda, const_cast<double *>(x.data()), &one, &beta, result.data(), &one, &luplo);
  return result;
}

/* ===== BLAS Level 3 operations ===== */

DataContainer AlgebraEngine::MatrixProduct(const DataContainer & A, const DataContainer & B)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger k = A.getDimension();
  const UnsignedInteger kB = B.getSize();
  const UnsignedInteger n = B.getDimension();
  if (k != kB) throw InvalidDimensionException(HERE) << "Cannot multiply matrices: left columns=" << k << " != right rows=" << kB;
  DataContainer result(m, n, 0.0);
  if (m == 0 || n == 0 || k == 0) return result;
  int intM = static_cast<int>(m);
  int intN = static_cast<int>(n);
  int intK = static_cast<int>(k);
  double alpha = 1.0;
  double beta = 0.0;
  int ltransa = 1;
  int ltransb = 1;
  dgemm_((char *)"N", (char *)"N", &intM, &intN, &intK, &alpha, const_cast<double *>(A.data()), &intM, const_cast<double *>(B.data()), &intK, &beta, result.data(), &intM, &ltransa, &ltransb);
  return result;
}

DataContainer AlgebraEngine::SymProd(const DataContainer & A, const DataContainer & B, char symSide)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n)
    throw InvalidDimensionException(HERE) << "Cannot compute symmetric product: matrix must be square (rows=" << A.getSize() << ", columns=" << A.getDimension() << ")";
  if (B.getSize() != n || B.getDimension() != n)
    throw InvalidDimensionException(HERE) << "Cannot compute symmetric product: matrix size=" << B.getSize() << "x" << B.getDimension() << " does not match n=" << n;
  if (symSide == 'R' || symSide == 'r')
    return MatrixProduct(Transpose(A), MatrixProduct(B, A));
  return MatrixProduct(MatrixProduct(A, B), Transpose(A));
}

DataContainer AlgebraEngine::TriangularProd(const DataContainer & A, const DataContainer & B, char side, char uplo)
{
  const UnsignedInteger m = B.getSize();
  const UnsignedInteger n = B.getDimension();
  DataContainer result(m, n, 0.0);
  std::copy(B.data(), B.data() + m * n, result.data());
  if (m == 0 || n == 0) return result;
  int intM = static_cast<int>(m);
  int intN = static_cast<int>(n);
  double alpha = 1.0;
  int lda = intM;
  int ldb = intM;
  int lside = 1;
  int luplo = 1;
  int ltrans = 1;
  int ldiag = 1;
  dtrmm_(&side, &uplo, (char *)"N", (char *)"N", &intM, &intN, &alpha, const_cast<double *>(A.data()), &lda, result.data(), &ldb, &lside, &luplo, &ltrans, &ldiag);
  return result;
}

/* ===== LAPACK decompositions ===== */

DataContainer AlgebraEngine::ComputeCholesky(const DataContainer & A)
{
  const UnsignedInteger n = A.getSize();
  if (n == 0) throw InvalidDimensionException(HERE) << "Cannot compute Cholesky decomposition of an empty matrix";
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for Cholesky decomposition";
  DataContainer result(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, result.data());
  int intN = static_cast<int>(n);
  int info = 0;
  int luplo = 1;
  dpotrf_((char *)"L", &intN, result.data(), &intN, &info, &luplo);
  if (info != 0) throw NotSymmetricDefinitePositiveException(HERE) << "Error: the matrix is not definite positive (LAPACK dpotrf info=" << info << ")";
  for (UnsignedInteger j = 1; j < n; ++j)
    std::fill_n(result.data() + j * n, j, 0.0);
  return result;
}

void AlgebraEngine::ComputeQR(const DataContainer & A, DataContainer & Q, DataContainer & R, Bool fullQR)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (m == 0 || n == 0) throw InvalidDimensionException(HERE) << "Cannot compute the QR decomposition of an empty matrix";
  int intM = static_cast<int>(m);
  int intN = static_cast<int>(n);
  int lda = intM;
  int k = static_cast<int>(std::min(m, n));
  // DGEQRF updates the whole M x N matrix while DORGQR in full mode writes
  // an M x M Q, so the buffer must hold M x max(M, N) elements.
  const UnsignedInteger p = fullQR ? m : k;
  DataContainer workMatrix(m, std::max(m, n), 0.0);
  for (UnsignedInteger j = 0; j < n; ++j)
    for (UnsignedInteger i = 0; i < m; ++i)
      workMatrix(i, j) = A(i, j);

  std::vector<double> tau(k);

  int lwork = -1;
  double lwork_d = -1.0;
  int info = -1;
  dgeqrf_(&intM, &intN, workMatrix.data(), &lda, tau.data(), &lwork_d, &lwork, &info);
  if (info != 0) throw InternalException(HERE) << "LAPACK DGEQRF: error code=" << info;
  lwork = static_cast<int>(lwork_d);
  std::vector<double> work(lwork);
  dgeqrf_(&intM, &intN, workMatrix.data(), &lda, tau.data(), work.data(), &lwork, &info);
  if (info != 0) throw InternalException(HERE) << "LAPACK DGEQRF: error code=" << info;

  int intP = static_cast<int>(p);
  R = DataContainer(p, n, 0.0);
  for (UnsignedInteger i = 0; i < static_cast<UnsignedInteger>(k); ++i)
    for (UnsignedInteger j = i; j < n; ++j)
      R(i, j) = workMatrix(i, j);

  lwork = -1;
  lwork_d = -1.0;
  dorgqr_(&intM, &intP, &k, workMatrix.data(), &lda, tau.data(), &lwork_d, &lwork, &info);
  if (info != 0) throw InternalException(HERE) << "LAPACK DORGQR: error code=" << info;
  lwork = static_cast<int>(lwork_d);
  work = std::vector<double>(lwork);
  dorgqr_(&intM, &intP, &k, workMatrix.data(), &lda, tau.data(), work.data(), &lwork, &info);
  if (info != 0) throw InternalException(HERE) << "LAPACK DORGQR: error code=" << info;

  Q = DataContainer(m, p, 0.0);
  for (UnsignedInteger j = 0; j < p; ++j)
    for (UnsignedInteger i = 0; i < m; ++i)
      Q(i, j) = workMatrix(i, j);
}

void AlgebraEngine::ComputeSVD(const DataContainer & A, DataContainer & U, DataContainer & S, DataContainer & VT, Bool fullSVD)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (m == 0 || n == 0) throw InvalidDimensionException(HERE) << "Cannot compute SVD of an empty matrix";
  int intM = static_cast<int>(m);
  int intN = static_cast<int>(n);
  int minMN = static_cast<int>(std::min(m, n));

  DataContainer workMatrix(m, n, 0.0);
  std::copy(A.data(), A.data() + m * n, workMatrix.data());

  char jobz = fullSVD ? 'A' : 'S';
  int ljobz = 1;
  int ldu = intM;
  int ldvt = fullSVD ? intN : minMN;
  U = DataContainer(m, fullSVD ? m : static_cast<UnsignedInteger>(minMN), 0.0);
  VT = DataContainer(fullSVD ? n : static_cast<UnsignedInteger>(minMN), n, 0.0);
  S = DataContainer(static_cast<UnsignedInteger>(minMN), 0.0);
  std::vector<double> work(1);
  int lwork = -1;
  std::vector<int> iwork(8 * minMN);
  int info = 0;

  dgesdd_(&jobz, &intM, &intN, workMatrix.data(), &intM, S.data(), U.data(), &ldu, VT.data(), &ldvt, work.data(), &lwork, iwork.data(), &info, &ljobz);
  lwork = static_cast<int>(work[0]);
  work.resize(lwork);
  dgesdd_(&jobz, &intM, &intN, workMatrix.data(), &intM, S.data(), U.data(), &ldu, VT.data(), &ldvt, work.data(), &lwork, iwork.data(), &info, &ljobz);
  if (info != 0) throw InternalException(HERE) << "Error: LAPACK DGESDD failed with info=" << info;
}

void AlgebraEngine::ComputeLU(const DataContainer & A, DataContainer & L, DataContainer & U)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (m == 0 || n == 0) throw InvalidDimensionException(HERE) << "Cannot compute LU decomposition of an empty matrix";
  int intM = static_cast<int>(m);
  int intN = static_cast<int>(n);

  DataContainer workMatrix(m, n, 0.0);
  std::copy(A.data(), A.data() + m * n, workMatrix.data());

  std::vector<int> ipiv(std::min(m, n));
  int info = 0;
  dgetrf_(&intM, &intN, workMatrix.data(), &intM, ipiv.data(), &info);
  if (info != 0) throw InternalException(HERE) << "Error: LAPACK DGETRF failed with info=" << info;

  const UnsignedInteger minMN = std::min(m, n);
  L = DataContainer(m, minMN, 0.0);
  U = DataContainer(minMN, n, 0.0);

  for (UnsignedInteger j = 0; j < minMN; ++j)
  {
    L(j, j) = 1.0;
    for (UnsignedInteger i = j + 1; i < m; ++i)
      L(i, j) = workMatrix(i, j);
  }

  for (UnsignedInteger j = 0; j < n; ++j)
  {
    for (UnsignedInteger i = 0; i <= std::min(j, minMN - 1); ++i)
      U(i, j) = workMatrix(i, j);
  }
}

/* ===== Linear solvers ===== */

DataContainer AlgebraEngine::SolveLinearSystem(const DataContainer & A, const DataContainer & b)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for linear system solve";
  if (b.getSize() != n) throw InvalidDimensionException(HERE) << "Right-hand side dimension=" << b.getSize() << " must match matrix dimension=" << n;
  DataContainer result(n, 1, 0.0);
  std::copy(b.data(), b.data() + n, result.data());
  if (n == 0) return result;
  int intN = static_cast<int>(n);
  int nrhs = 1;
  int info = 0;
  std::vector<int> ipiv(n);
  DataContainer Acopy(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, Acopy.data());
  dgesv_(&intN, &nrhs, Acopy.data(), &intN, ipiv.data(), result.data(), &intN, &info);
  if (info != 0) throw NotDefinedException(HERE) << "Error: the matrix is singular (LAPACK dgesv info=" << info << ")";
  return result;
}

DataContainer AlgebraEngine::SolveLinearSystemSPD(const DataContainer & A, const DataContainer & b)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for SPD linear system solve";
  if (b.getSize() != n) throw InvalidDimensionException(HERE) << "Right-hand side dimension=" << b.getSize() << " must match matrix dimension=" << n;
  DataContainer result(n, 1, 0.0);
  std::copy(b.data(), b.data() + n, result.data());
  if (n == 0) return result;
  int intN = static_cast<int>(n);
  int nrhs = 1;
  int info = 0;
  int luplo = 1;
  DataContainer Acopy(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, Acopy.data());
  dposv_((char *)"L", &intN, &nrhs, Acopy.data(), &intN, result.data(), &intN, &info, &luplo);
  if (info != 0) throw NotDefinedException(HERE) << "Error: the matrix is not positive definite or singular (LAPACK dposv info=" << info << ")";
  return result;
}

DataContainer AlgebraEngine::SolveLinearSystemTriangular(const DataContainer & A, const DataContainer & b, Bool lower, Bool transpose)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for triangular solve";
  if (b.getSize() != n) throw InvalidDimensionException(HERE) << "Right-hand side dimension=" << b.getSize() << " must match matrix dimension=" << n;
  DataContainer result(n, 1, 0.0);
  std::copy(b.data(), b.data() + n, result.data());
  if (n == 0) return result;
  char side = 'L';
  int lside = 1;
  char uploTri = lower ? 'L' : 'U';
  int luplo = 1;
  char transa = transpose ? 'T' : 'N';
  int ltransa = 1;
  char diag = 'N';
  int ldiag = 1;
  int intN = static_cast<int>(n);
  int nrhs = 1;
  double alpha = 1.0;
  int lda = intN;
  int ldb = intN;
  dtrsm_(&side, &uploTri, &transa, &diag, &intN, &nrhs, &alpha, const_cast<double *>(A.data()), &lda, result.data(), &ldb, &lside, &luplo, &ltransa, &ldiag);
  return result;
}

DataContainer AlgebraEngine::SolveLinearSystemRectangular(const DataContainer & A, const DataContainer & b)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (b.getSize() != m) throw InvalidDimensionException(HERE) << "Right-hand side dimension=" << b.getSize() << " must match matrix row dimension=" << m;
  if (m == 0 || n == 0) throw InvalidDimensionException(HERE) << "Cannot solve a linear system with empty matrix or empty right-hand side";
  int intM = static_cast<int>(m);
  int intN = static_cast<int>(n);
  int p = std::max(intM, intN);
  int nrhs = 1;

  DataContainer B(p, 1, 0.0);
  for (UnsignedInteger i = 0; i < m; ++i)
    B[i] = b[i];

  std::vector<int> jpiv(n, 0);
  double rcond = SpecFunc::Precision;
  int rank = -1;
  int info = -1;

  DataContainer Acopy(m, n, 0.0);
  std::copy(A.data(), A.data() + m * n, Acopy.data());

  double lwork_d = -1.0;
  int lwork = -1;
  dgelsy_(&intM, &intN, &nrhs, Acopy.data(), &intM, B.data(), &p, jpiv.data(), &rcond, &rank, &lwork_d, &lwork, &info);
  lwork = static_cast<int>(lwork_d);
  std::vector<double> work(lwork);
  dgelsy_(&intM, &intN, &nrhs, Acopy.data(), &intM, B.data(), &p, jpiv.data(), &rcond, &rank, work.data(), &lwork, &info);

  DataContainer result(n, 1, 0.0);
  for (UnsignedInteger i = 0; i < n; ++i)
    result[i] = B[i];
  return result;
}

/* ===== Matrix properties ===== */

Scalar AlgebraEngine::ComputeDeterminant(const DataContainer & A)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for determinant computation";
  if (n == 0) return 1.0;
  if (n == 1) return A(0, 0);
  if (n == 2) return A(0, 0) * A(1, 1) - A(0, 1) * A(1, 0);
  DataContainer Acopy(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, Acopy.data());
  int intN = static_cast<int>(n);
  std::vector<int> ipiv(n);
  int info = 0;
  dgetrf_(&intN, &intN, Acopy.data(), &intN, ipiv.data(), &info);
  Scalar det = 1.0;
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    det *= Acopy(i, i);
    if (ipiv[i] != static_cast<int>(i + 1)) det = -det;
  }
  return det;
}

Scalar AlgebraEngine::ComputeLogAbsoluteDeterminant(const DataContainer & A, Scalar & signOut)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for determinant computation";
  signOut = 1.0;
  if (n == 0) return 0.0;
  if (n == 1)
  {
    const Scalar value = A(0, 0);
    if (value == 0.0)
    {
      signOut = 0.0;
      return SpecFunc::LowestScalar;
    }
    signOut = (value > 0.0 ? 1.0 : -1.0);
    return log(std::abs(value));
  }
  if (n == 2)
  {
    const Scalar value = A(0, 0) * A(1, 1) - A(0, 1) * A(1, 0);
    if (value == 0.0)
    {
      signOut = 0.0;
      return SpecFunc::LowestScalar;
    }
    signOut = (value > 0.0 ? 1.0 : -1.0);
    return log(std::abs(value));
  }
  DataContainer Acopy(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, Acopy.data());
  int intN = static_cast<int>(n);
  std::vector<int> ipiv(n);
  int info = 0;
  dgetrf_(&intN, &intN, Acopy.data(), &intN, ipiv.data(), &info);
  if (info > 0)
  {
    signOut = 0.0;
    return SpecFunc::LowestScalar;
  }
  Scalar logAbsoluteDeterminant = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    const Scalar pivot = Acopy(i, i);
    if (std::abs(pivot) == 0.0)
    {
      logAbsoluteDeterminant = SpecFunc::LowestScalar;
      signOut = 0.0;
    }
    else
    {
      logAbsoluteDeterminant += log(std::abs(pivot));
    }
    if (pivot < 0.0) signOut = -signOut;
    if (ipiv[i] != static_cast<int>(i + 1)) signOut = -signOut;
  }
  return logAbsoluteDeterminant;
}

Scalar AlgebraEngine::ComputeTrace(const DataContainer & A)
{
  const UnsignedInteger n = std::min(A.getSize(), A.getDimension());
  Scalar result = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
    result += A(i, i);
  return result;
}

DataContainer AlgebraEngine::ComputeEigenValuesSquare(const DataContainer & A)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidArgumentException(HERE) << "Matrix must be square for eigenvalue computation";
  if (n == 0) return DataContainer(0, 1, 0.0, DataContainer::COLUMN_MAJOR);
  int intN = static_cast<int>(n);

  DataContainer Acopy(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, Acopy.data());

  char jobvl = 'N';
  char jobvr = 'N';
  int ljobvl = 1;
  int ljobvr = 1;
  std::vector<Scalar> wr(n, 0.0);
  std::vector<Scalar> wi(n, 0.0);
  Scalar vl = 0.0;
  Scalar vr = 0.0;
  int ldvl = 1;
  int ldvr = 1;
  int lwork = -1;
  Scalar lwork_d = 0.0;
  int info = 0;

  dgeev_(&jobvl, &jobvr, &intN, Acopy.data(), &intN, wr.data(), wi.data(), &vl, &ldvl, &vr, &ldvr, &lwork_d, &lwork, &info, &ljobvl, &ljobvr);
  lwork = static_cast<int>(lwork_d);
  std::vector<Scalar> work(lwork);
  dgeev_(&jobvl, &jobvr, &intN, Acopy.data(), &intN, wr.data(), wi.data(), &vl, &ldvl, &vr, &ldvr, work.data(), &lwork, &info, &ljobvl, &ljobvr);
  if (info != 0) throw InternalException(HERE) << "LAPACK dgeev failed with info=" << info;

  DataContainer eigenValues(2 * n, 1, 0.0);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    eigenValues[2 * i] = wr[i];
    eigenValues[2 * i + 1] = wi[i];
  }
  return eigenValues;
}

DataContainer AlgebraEngine::ComputeEigenValuesSymmetric(const DataContainer & A)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidArgumentException(HERE) << "Matrix must be square for eigenvalue computation";
  if (n == 0) return DataContainer(0, 1, 0.0, DataContainer::COLUMN_MAJOR);
  int intN = static_cast<int>(n);

  DataContainer Acopy(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, Acopy.data());

  char jobz = 'N';
  char uplo = 'L';
  int ljobz = 1;
  int luplo = 1;
  std::vector<Scalar> w(n, 0.0);
  int lwork = -1;
  Scalar lwork_d = 0.0;
  int info = 0;

  dsyev_(&jobz, &uplo, &intN, Acopy.data(), &intN, w.data(), &lwork_d, &lwork, &info, &ljobz, &luplo);
  lwork = static_cast<int>(lwork_d);
  std::vector<Scalar> work(lwork);
  dsyev_(&jobz, &uplo, &intN, Acopy.data(), &intN, w.data(), work.data(), &lwork, &info, &ljobz, &luplo);
  if (info != 0) throw InternalException(HERE) << "LAPACK dsyev failed with info=" << info;

  DataContainer eigenValues(n, 1, 0.0);
  for (UnsignedInteger i = 0; i < n; ++i)
    eigenValues[i] = w[i];
  return eigenValues;
}

DataContainer AlgebraEngine::ComputeSingularValues(const DataContainer & A)
{
  DataContainer U, S, VT;
  ComputeSVD(A, U, S, VT, false);
  return S;
}

Bool AlgebraEngine::IsSymmetric(const DataContainer & A, Scalar tolerance)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) return false;
  if (n == 0) return true;
  if (tolerance == 0.0) tolerance = SpecFunc::Precision;
  for (UnsignedInteger i = 0; i < n; ++i)
    for (UnsignedInteger j = i + 1; j < n; ++j)
      if (std::abs(A(i, j) - A(j, i)) > tolerance * (std::abs(A(i, j)) + std::abs(A(j, i))))
        return false;
  return true;
}

Bool AlgebraEngine::IsPositiveDefinite(const DataContainer & A)
{
  try
  {
    ComputeCholesky(A);
    return true;
  }
  catch (const Exception &)
  {
    return false;
  }
}

Scalar AlgebraEngine::FrobeniusNorm(const DataContainer & A)
{
  const UnsignedInteger n = A.getSize() * A.getDimension();
  Scalar result = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
    result += A[i] * A[i];
  return std::sqrt(result);
}

DataContainer AlgebraEngine::ComputeGram(const DataContainer & A, Bool transpose)
{
  if (transpose)
    return MatrixProduct(Transpose(A), A);
  else
    return MatrixProduct(A, Transpose(A));
}

DataContainer AlgebraEngine::Transpose(const DataContainer & A)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  DataContainer result(n, m, 0.0);
  for (UnsignedInteger i = 0; i < m; ++i)
    for (UnsignedInteger j = 0; j < n; ++j)
      result(j, i) = A(i, j);
  return result;
}

DataContainer AlgebraEngine::Inverse(const DataContainer & A)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for inversion";

  DataContainer result(n, n, 0.0);

  DataContainer Acopy(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, Acopy.data());
  std::vector<int> ipiv(n);
  int intN = static_cast<int>(n);
  int info = 0;

  for (UnsignedInteger j = 0; j < n; ++j)
    result(j, j) = 1.0;

  dgesv_(&intN, &intN, Acopy.data(), &intN, ipiv.data(), result.data(), &intN, &info);
  if (info != 0) throw NotDefinedException(HERE) << "Error: the matrix is singular (LAPACK dgesv info=" << info << ")";
  return result;
}

DataContainer AlgebraEngine::InverseSPD(const DataContainer & A)
{
  const UnsignedInteger n = A.getSize();
  if (A.getDimension() != n) throw InvalidDimensionException(HERE) << "Matrix must be square for SPD inversion";

  DataContainer result(n, n, 0.0);

  DataContainer Acopy(n, n, 0.0);
  std::copy(A.data(), A.data() + n * n, Acopy.data());
  int intN = static_cast<int>(n);
  int nrhs = static_cast<int>(n);
  int info = 0;
  int luplo = 1;

  for (UnsignedInteger j = 0; j < n; ++j)
    result(j, j) = 1.0;

  dposv_((char *)"L", &intN, &nrhs, Acopy.data(), &intN, result.data(), &intN, &info, &luplo);
  if (info != 0) throw NotDefinedException(HERE) << "Error: the matrix is not positive definite or singular (LAPACK dposv info=" << info << ")";
  return result;
}

Scalar AlgebraEngine::SumElements(const DataContainer & A)
{
  const UnsignedInteger n = A.getSize() * A.getDimension();
  Scalar result = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
    result += A[i];
  return result;
}

DataContainer AlgebraEngine::Clean(const DataContainer & A, Scalar threshold)
{
  const UnsignedInteger n = A.getSize();
  const UnsignedInteger d = A.getDimension();
  const UnsignedInteger totalSize = n * d;
  DataContainer result(n, d, 0.0, A.getLayout());
  for (UnsignedInteger i = 0; i < totalSize; ++i)
    result[i] = (std::abs(A[i]) < threshold) ? 0.0 : A[i];
  return result;
}


DataContainer AlgebraEngine::MatrixProductBlockwise(const DataContainer & A,
    const DataContainer & B,
    UnsignedInteger blockSize)
{
  // A is (M x K) column-major, B is (K x N) column-major
  const UnsignedInteger M = A.getDimension();
  const UnsignedInteger K = A.getSize();
  const UnsignedInteger N = B.getDimension();

  if (K != B.getSize())
    throw InvalidArgumentException(HERE) << "Incompatible dimensions for matrix product";

  DataContainer C(M, N, 0.0, DataContainer::COLUMN_MAJOR);

  // Tile loop: for each block-row of A and block-column of B
  for (UnsignedInteger i0 = 0; i0 < M; i0 += blockSize)
  {
    const UnsignedInteger iEnd = std::min(i0 + blockSize, M);

    for (UnsignedInteger j0 = 0; j0 < N; j0 += blockSize)
    {
      const UnsignedInteger jEnd = std::min(j0 + blockSize, N);

      // Accumulate along k in blocks
      for (UnsignedInteger k0 = 0; k0 < K; k0 += blockSize)
      {
        const UnsignedInteger kEnd = std::min(k0 + blockSize, K);

        // Extract A tile: rows [i0, iEnd), cols [k0, kEnd)
        const UnsignedInteger tileM = iEnd - i0;
        const UnsignedInteger tileK = kEnd - k0;
        std::vector<Scalar> Atile(tileM * tileK);

        for (UnsignedInteger kk = 0; kk < tileK; ++kk)
          for (UnsignedInteger ii = 0; ii < tileM; ++ii)
            Atile[ii + kk * tileM] = A((i0 + ii), (k0 + kk));

        // Extract B tile: rows [k0, kEnd), cols [j0, jEnd)
        const UnsignedInteger tileN = jEnd - j0;
        std::vector<Scalar> Btile(tileK * tileN);

        for (UnsignedInteger jj = 0; jj < tileN; ++jj)
          for (UnsignedInteger kk = 0; kk < tileK; ++kk)
            Btile[kk + jj * tileK] = B((k0 + kk), (j0 + jj));

        // Ctile += Atile * Btile via DGEMM
        {
          int m = tileM;
          int n = tileN;
          int k = tileK;
          Scalar alpha = 1.0;
          Scalar beta = 1.0;
          int ltransa = 1;
          int ltransb = 1;
          std::vector<Scalar> Ctile(tileM * tileN, 0.0);

          dgemm_((char *)"N", (char *)"N", &m, &n, &k, &alpha,
                 Atile.data(), &m,
                 Btile.data(), &k,
                 &beta,
                 Ctile.data(), &m, &ltransa, &ltransb);

          // Accumulate into C
          for (UnsignedInteger jj = 0; jj < tileN; ++jj)
            for (UnsignedInteger ii = 0; ii < tileM; ++ii)
              C((i0 + ii), (j0 + jj)) += Ctile[ii + jj * tileM];
        }
      }
    }
  }
  return C;
}


DataContainer AlgebraEngine::ComputeCholeskyBlockwise(const DataContainer & A,
    UnsignedInteger blockSize)
{
  const UnsignedInteger n = A.getSize();
  if (n == 0) return DataContainer();

  // Work on a copy (column-major storage)
  // CRITICAL: must deep-copy to avoid COW corruption of A via LAPACK in-place writes
  DataContainer L(A.getSize(), A.getDimension(), 0.0, DataContainer::COLUMN_MAJOR);
  std::copy(A.data(), A.data() + A.getSize() * A.getDimension(), L.data());

  for (UnsignedInteger j = 0; j < n; j += blockSize)
  {
    const UnsignedInteger jEnd = std::min(j + blockSize, n);
    const UnsignedInteger nb = jEnd - j;

    // Phase 1: Factorize diagonal block L(j:jEnd, j:jEnd) via dpotrf_
    {
      int intNb = static_cast<int>(nb);
      int intN = static_cast<int>(n);
      int info = 0;
      int one = 1;
      dpotrf_((char *)"L", &intNb, L.data() + (j + j * n), &intN, &info, &one);
      if (info > 0)
        throw InvalidArgumentException(HERE) << "Matrix is not positive definite at column " << (j + info - 1);
      if (info < 0)
        throw InternalException(HERE) << "LAPACK DPOTRF failed with info=" << info;
    }

    // Phase 2: Compute the panel of L below the diagonal
    // L(jEnd:n, j:jEnd) = A(jEnd:n, j:jEnd) * inv(L(j:jEnd, j:jEnd)^T)
    // via DTRSM: L * X = A(panel) => X = inv(L^T) * A(panel)
    {
      const UnsignedInteger m = n - jEnd;
      if (m > 0 && nb > 0)
      {
        Scalar alpha = 1.0;
        int intM = static_cast<int>(m);
        int intNb = static_cast<int>(nb);
        int intN = static_cast<int>(n);
        int one = 1;
        // DTRSM: side='R', uplo='U', trans='T', diag='N'
        // B * op(A) = alpha * B  =>  panel * inv(L_diag^T) = panel / L_diag^T
        // So after: panel = A(jEnd:n, j:jEnd) * inv(L(j:jEnd, j:jEnd)^T)
        dtrsm_((char *)"R", (char *)"L", (char *)"T", (char *)"N",
               &intM, &intNb, &alpha,
               L.data() + (j + j * n), &intN,
               L.data() + (jEnd + j * n), &intN, &one, &one, &one, &one);
      }
    }

    // Phase 3: Update trailing submatrix
    // L(jEnd:n, jEnd:n) -= L(jEnd:n, j:jEnd) * L(jEnd:n, j:jEnd)^T
    {
      const UnsignedInteger m = n - jEnd;
      if (m > 0 && nb > 0)
      {
        // Extract the divided panel: m x nb
        std::vector<Scalar> panelData(m * nb);
        for (UnsignedInteger jj = 0; jj < nb; ++jj)
          for (UnsignedInteger ii = 0; ii < m; ++ii)
            panelData[ii + jj * m] = L(jEnd + ii, j + jj);

        // trailing -= panel * panel^T via DSYRK
        Scalar alpha = -1.0;
        Scalar beta = 1.0;
        int intM = static_cast<int>(m);
        int intNb = static_cast<int>(nb);
        int intN = static_cast<int>(n);
        int one = 1;
        dsyrk_((char *)"L", (char *)"N", &intM, &intNb, &alpha,
               panelData.data(), &intM,
               &beta,
               L.data() + (jEnd + jEnd * n), &intN, &one, &one);
      }
    }
  }

  // Zero out upper triangle
  for (UnsignedInteger j = 0; j < n; ++j)
    for (UnsignedInteger i = 0; i < j; ++i)
      L(i, j) = 0.0;

  return L;
}


DataContainer AlgebraEngine::SolveLinearSystemBlockwise(const DataContainer & A,
    const DataContainer & b,
    UnsignedInteger blockSize)
{
  // For non-symmetric A, fall back to the standard solver
  // Block solve is beneficial mainly for large SPD systems
  if (IsSymmetric(A))
  {
    // Block Cholesky solve: Ax = b, A = L * L^T
    DataContainer L = ComputeCholeskyBlockwise(A, blockSize);
    const UnsignedInteger n = A.getSize();
    const UnsignedInteger nrhs = b.getDimension();

    DataContainer x(n, nrhs, 0.0, DataContainer::COLUMN_MAJOR);
    // Copy b into x
    for (UnsignedInteger j = 0; j < nrhs; ++j)
      for (UnsignedInteger i = 0; i < n; ++i)
        x(i, j) = b(i, j);

    // Forward solve: L * y = b using DTRSM
    {
      Scalar alpha = 1.0;
      int intN = static_cast<int>(n);
      int intNrhs = static_cast<int>(nrhs);
      int one = 1;
      dtrsm_((char *)"L", (char *)"L", (char *)"N", (char *)"N",
             &intN, &intNrhs, &alpha,
             L.data(), &intN,
             x.data(), &intN, &one, &one, &one, &one);
    }

    // Back solve: L^T * x = y using DTRSM
    {
      Scalar alpha = 1.0;
      int intN = static_cast<int>(n);
      int intNrhs = static_cast<int>(nrhs);
      int one = 1;
      dtrsm_((char *)"L", (char *)"L", (char *)"T", (char *)"N",
             &intN, &intNrhs, &alpha,
             L.data(), &intN,
             x.data(), &intN, &one, &one, &one, &one);
    }

    return x;
  }
  else
  {
    // Non-symmetric: fall back to in-memory solver
    return SolveLinearSystem(A, b);
  }
}


void AlgebraEngine::ComputeQRBlockwise(const DataContainer & A,
                                       DataContainer & Q,
                                       DataContainer & R,
                                       Bool fullQR,
                                       UnsignedInteger blockSize)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (m == 0 || n == 0)
    throw InvalidDimensionException(HERE) << "Cannot compute QR decomposition of an empty matrix";
  const UnsignedInteger k = std::min(m, n);

  DataContainer workMatrix(m, n, 0.0);
  std::copy(A.data(), A.data() + m * n, workMatrix.data());

  Scalar * wm = workMatrix.data();

  int intM = static_cast<int>(m);
  int intN = static_cast<int>(n);
  int intK = static_cast<int>(k);
  int info = -1;
  int lwork = -1;
  double workQuery = 0.0;
  dgeqrf_(&intM, &intN, wm, &intM, nullptr, &workQuery, &lwork, &info);
  lwork = static_cast<int>(workQuery);
  double workQuery2 = 0;
  int lwork2 = -1;
  dorgqr_(&intM, &intM, &intK, wm, &intM, nullptr, &workQuery2, &lwork2, &info);
  if (static_cast<int>(workQuery2) > lwork)
    lwork = static_cast<int>(workQuery2);
  if (lwork < 1) lwork = 1;
  std::vector<double> work(lwork);

  std::vector<double> allTau(k, 0.0);

  for (UnsignedInteger j = 0; j < n; j += blockSize)
  {
    const UnsignedInteger jEnd = std::min(j + blockSize, n);
    const UnsignedInteger nb = jEnd - j;
    const UnsignedInteger mSub = (j < m) ? m - j : 0;
    int intMSub = static_cast<int>(mSub);
    int intNb = static_cast<int>(nb);

    if (mSub > 0 && intNb > 0)
    {
      std::vector<double> tau(nb);
      info = -1;
      dgeqrf_(&intMSub, &intNb, wm + j * m + j, &intM, tau.data(), work.data(), &lwork, &info);
      if (info != 0)
        throw InternalException(HERE) << "LAPACK DGEQRF failed with info=" << info;

      for (UnsignedInteger i = 0; i < nb && (j + i) < k; ++i)
        allTau[j + i] = tau[i];

      const UnsignedInteger kPanel = std::min(mSub, nb);

      for (UnsignedInteger jj = jEnd; jj < n; jj += blockSize)
      {
        const UnsignedInteger jjEnd = std::min(jj + blockSize, n);
        const UnsignedInteger nc = jjEnd - jj;

        for (UnsignedInteger p = 0; p < kPanel; ++p)
        {
          const Scalar tauP = tau[p];
          if (tauP == 0.0) continue;

          std::vector<double> v(mSub, 0.0);
          v[p] = 1.0;
          for (UnsignedInteger row = p + 1; row < mSub; ++row)
            v[row] = wm[(j + p) * m + j + row];

          std::vector<double> dot(nc, 0.0);
          for (UnsignedInteger col = 0; col < nc; ++col)
            for (UnsignedInteger row = p; row < mSub; ++row)
              dot[col] += v[row] * wm[(jj + col) * m + j + row];

          for (UnsignedInteger col = 0; col < nc; ++col)
            for (UnsignedInteger row = p; row < mSub; ++row)
              wm[(jj + col) * m + j + row] -= tauP * v[row] * dot[col];
        }
      }
    }
  }

  const UnsignedInteger rRows = fullQR ? m : k;
  R = DataContainer(rRows, n, 0.0);
  Scalar * rData = R.data();
  for (UnsignedInteger j = 0; j < n; ++j)
    for (UnsignedInteger i = 0; i <= std::min(j, k - 1); ++i)
      rData[i + j * rRows] = wm[i + j * m];

  Q = DataContainer(m, rRows, 0.0);
  Scalar * qData = Q.data();
  for (UnsignedInteger j = 0; j < k; ++j)
    for (UnsignedInteger i = 0; i < m; ++i)
      qData[i + j * m] = wm[i + j * m];

  if (fullQR)
  {
    int intR = static_cast<int>(rRows);
    info = -1;
    dorgqr_(&intM, &intR, &intK, qData, &intM, allTau.data(), work.data(), &lwork, &info);
    if (info != 0)
      throw InternalException(HERE) << "LAPACK DORGQR failed with info=" << info;
    for (UnsignedInteger j = k; j < rRows; ++j)
      qData[j + j * m] = 1.0;
  }
  else
  {
    info = -1;
    dorgqr_(&intM, &intK, &intK, qData, &intM, allTau.data(), work.data(), &lwork, &info);
    if (info != 0)
      throw InternalException(HERE) << "LAPACK DORGQR failed with info=" << info;
  }
}


void AlgebraEngine::ComputeLUBlockwise(const DataContainer & A,
                                       DataContainer & P,
                                       DataContainer & L,
                                       DataContainer & U,
                                       UnsignedInteger blockSize)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (m == 0 || n == 0)
    throw InvalidDimensionException(HERE) << "Cannot compute LU decomposition of an empty matrix";
  const UnsignedInteger minMN = std::min(m, n);

  DataContainer workMatrix(m, n, 0.0);
  std::copy(A.data(), A.data() + m * n, workMatrix.data());

  int intM = static_cast<int>(m);

  std::vector<int> allPivots(m);
  for (UnsignedInteger i = 0; i < m; ++i) allPivots[i] = i;

  std::vector<int> ipiv(blockSize);

  for (UnsignedInteger j = 0; j < minMN; j += blockSize)
  {
    const UnsignedInteger jEnd = std::min(j + blockSize, minMN);
    const UnsignedInteger nb = jEnd - j;
    const UnsignedInteger panelRows = m - j;
    const UnsignedInteger k = std::min(panelRows, nb);

    int intPanelRows = static_cast<int>(panelRows);
    int intNb = static_cast<int>(nb);
    int info = 0;
    dgetrf_(&intPanelRows, &intNb, workMatrix.data() + (j + j * m), &intM, ipiv.data(), &info);
    if (info != 0)
      throw InternalException(HERE) << "LAPACK DGETRF failed with info=" << info;

    // Apply row permutations from ipiv: all columns except current panel
    for (UnsignedInteger i = 0; i < k; ++i)
    {
      const int pivot = ipiv[i] - 1;
      if (pivot != static_cast<int>(i))
      {
        std::swap(allPivots[j + i], allPivots[j + pivot]);
        // Swap already-processed columns (L multipliers from previous panels)
        for (UnsignedInteger col = 0; col < j; ++col)
          std::swap(workMatrix(j + i, col), workMatrix(j + pivot, col));
        // Swap trailing columns (past panel end)
        for (UnsignedInteger col = jEnd; col < n; ++col)
          std::swap(workMatrix(j + i, col), workMatrix(j + pivot, col));
      }
    }

    if (jEnd < n && jEnd < m)
    {
      if (nb > 1)
      {
        // For nb>1: DTRSM solves L_panel * U12 = A_right => U12 = inv(L_panel) * A_right
        // L_panel is the unit lower triangular factor stored in strictly lower part of panel
        int intK = static_cast<int>(nb);
        int intNRHS = static_cast<int>(n - jEnd);
        Scalar alpha = 1.0;
        char side = 'L';
        char uplo = 'L';
        char trans = 'N';
        char diag = 'U';
        int lside = 1, luplo = 1, ltrans = 1, ldiag = 1;
        dtrsm_(&side, &uplo, &trans, &diag, &intK, &intNRHS, &alpha,
               workMatrix.data() + (j + j * m), &intM,
               workMatrix.data() + (j + jEnd * m), &intM,
               &lside, &luplo, &ltrans, &ldiag);
      }
      // For nb==1: L_below already contains 1/U(j,j), so A_right is U_row directly
      {
        int intK = static_cast<int>(nb);
        int intMUpdate = static_cast<int>(m - jEnd);
        int intNRHS = static_cast<int>(n - jEnd);
        Scalar alpha = -1.0;
        Scalar beta = 1.0;
        char ta = 'N';
        char tb = 'N';
        int lta = 1, ltb = 1;
        dgemm_(&ta, &tb, &intMUpdate, &intNRHS, &intK, &alpha,
               workMatrix.data() + (jEnd + j * m), &intM,
               workMatrix.data() + (j + jEnd * m), &intM,
               &beta,
               workMatrix.data() + (jEnd + jEnd * m), &intM,
               &lta, &ltb);
      }
    }
  }

  // Permutation matrix: allPivots[i] = original row now at position i
  P = DataContainer(m, m, 0.0);
  for (UnsignedInteger i = 0; i < m; ++i)
    P(i, allPivots[i]) = 1.0;

  // L: unit lower triangular (strictly lower part from workMatrix)
  L = DataContainer(m, minMN, 0.0);
  for (UnsignedInteger j = 0; j < minMN; ++j)
  {
    L(j, j) = 1.0;
    for (UnsignedInteger i = j + 1; i < m; ++i)
      L(i, j) = workMatrix(i, j);
  }

  // U: upper triangular from workMatrix
  U = DataContainer(minMN, n, 0.0);
  for (UnsignedInteger j = 0; j < n; ++j)
    for (UnsignedInteger i = 0; i <= std::min(j, minMN - 1); ++i)
      U(i, j) = workMatrix(i, j);
}


DataContainer AlgebraEngine::SolveLinearSystemTriangularBlockwise(
  const DataContainer & T,
  const DataContainer & b,
  Bool lower,
  Bool transpose,
  UnsignedInteger blockSize)
{
  const UnsignedInteger n = T.getSize();
  if (T.getDimension() != n)
    throw InvalidDimensionException(HERE) << "Matrix must be square for triangular solve";
  if (b.getSize() != n)
    throw InvalidDimensionException(HERE) << "Right-hand side dimension=" << b.getSize() << " must match matrix dimension=" << n;

  const UnsignedInteger nrhs = b.getDimension();
  DataContainer result(n, nrhs, 0.0);
  std::copy(b.data(), b.data() + n * nrhs, result.data());

  if (n == 0 || nrhs == 0) return result;

  char side = 'L';
  int lside = 1;
  char uploChar = lower ? 'L' : 'U';
  int luplo = 1;
  char transChar = transpose ? 'T' : 'N';
  int ltrans = 1;
  char diag = 'N';
  int ldiag = 1;
  Scalar alpha = 1.0;
  int intN = static_cast<int>(n);
  int lda = intN;

  for (UnsignedInteger j = 0; j < nrhs; j += blockSize)
  {
    const UnsignedInteger jEnd = std::min(j + blockSize, nrhs);
    const UnsignedInteger nb = jEnd - j;
    int intNRHS = static_cast<int>(nb);
    int ldb = intN;
    dtrsm_(&side, &uploChar, &transChar, &diag, &intN, &intNRHS, &alpha,
           const_cast<double *>(T.data()), &lda,
           result.data() + j * n, &ldb,
           &lside, &luplo, &ltrans, &ldiag);
  }

  return result;
}


DataContainer AlgebraEngine::ComputeGramBlockwise(const DataContainer & A,
    Bool transpose,
    UnsignedInteger blockSize)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();

  if (transpose)
  {
    DataContainer G(n, n, 0.0);
    for (UnsignedInteger i0 = 0; i0 < m; i0 += blockSize)
    {
      const UnsignedInteger iEnd = std::min(i0 + blockSize, m);
      const UnsignedInteger nb = iEnd - i0;

      DataContainer Ablock(nb, n, 0.0);
      for (UnsignedInteger col = 0; col < n; ++col)
        for (UnsignedInteger row = 0; row < nb; ++row)
          Ablock(row, col) = A(i0 + row, col);

      int intN = static_cast<int>(n);
      int intK = static_cast<int>(nb);
      Scalar alpha = 1.0;
      Scalar beta = 1.0;
      int one = 1;
      dsyrk_((char *)"L", (char *)"T", &intN, &intK, &alpha,
             Ablock.data(), &intK,
             &beta,
             G.data(), &intN, &one, &one);
    }

    for (UnsignedInteger j = 0; j < n; ++j)
      for (UnsignedInteger i = 0; i < j; ++i)
        G(i, j) = G(j, i);

    return G;
  }
  else
  {
    DataContainer G(m, m, 0.0);
    for (UnsignedInteger j0 = 0; j0 < n; j0 += blockSize)
    {
      const UnsignedInteger jEnd = std::min(j0 + blockSize, n);
      const UnsignedInteger nb = jEnd - j0;

      DataContainer Ablock(m, nb, 0.0);
      for (UnsignedInteger col = 0; col < nb; ++col)
        for (UnsignedInteger row = 0; row < m; ++row)
          Ablock(row, col) = A(row, j0 + col);

      int intM = static_cast<int>(m);
      int intK = static_cast<int>(nb);
      Scalar alpha = 1.0;
      Scalar beta = 1.0;
      int one = 1;
      dsyrk_((char *)"L", (char *)"N", &intM, &intK, &alpha,
             Ablock.data(), &intM,
             &beta,
             G.data(), &intM, &one, &one);
    }

    for (UnsignedInteger j = 0; j < m; ++j)
      for (UnsignedInteger i = 0; i < j; ++i)
        G(i, j) = G(j, i);

    return G;
  }
}


Scalar AlgebraEngine::ComputeDeterminantBlockwise(const DataContainer & A,
    UnsignedInteger blockSize)
{
  Scalar signOut = 1.0;
  return std::exp(ComputeLogAbsoluteDeterminantBlockwise(A, signOut, blockSize)) * signOut;
}


Scalar AlgebraEngine::ComputeLogAbsoluteDeterminantBlockwise(const DataContainer & A,
    Scalar & signOut,
    UnsignedInteger blockSize)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (m == 0 || n == 0)
    throw InvalidDimensionException(HERE) << "Cannot compute determinant of an empty matrix";
  if (m != n)
    throw InvalidDimensionException(HERE) << "Matrix must be square for determinant";

  DataContainer workMatrix(m, n, 0.0);
  std::copy(A.data(), A.data() + m * n, workMatrix.data());

  int intM = static_cast<int>(m);
  signOut = 1.0;
  Scalar logAbsDet = 0.0;

  std::vector<int> ipiv(blockSize);

  for (UnsignedInteger j = 0; j < m; j += blockSize)
  {
    const UnsignedInteger jEnd = std::min(j + blockSize, m);
    const UnsignedInteger nb = jEnd - j;
    const UnsignedInteger panelRows = m - j;
    const UnsignedInteger k = std::min(panelRows, nb);

    int intPanelRows = static_cast<int>(panelRows);
    int intNb = static_cast<int>(nb);
    int info = 0;
    dgetrf_(&intPanelRows, &intNb, workMatrix.data() + (j + j * m), &intM, ipiv.data(), &info);
    if (info != 0)
      throw InternalException(HERE) << "LAPACK DGETRF failed with info=" << info;

    // Apply row permutations from ipiv to trailing columns and track sign
    for (UnsignedInteger i = 0; i < k; ++i)
    {
      const int pivot = ipiv[i] - 1;
      if (pivot != static_cast<int>(i))
      {
        signOut *= -1.0;
        for (UnsignedInteger col = jEnd; col < n; ++col)
        {
          const Scalar tmp = workMatrix(i + j, col);
          workMatrix(i + j, col) = workMatrix(pivot + j, col);
          workMatrix(pivot + j, col) = tmp;
        }
      }
    }

    // Accumulate diagonal product
    for (UnsignedInteger i = 0; i < k; ++i)
    {
      const Scalar diagVal = workMatrix(j + i, j + i);
      if (diagVal == 0.0)
      {
        signOut = 0.0;
        return -std::numeric_limits<Scalar>::infinity();
      }
      logAbsDet += std::log(std::abs(diagVal));
      if (diagVal < 0.0) signOut *= -1.0;
    }

    // Schur complement: A_trailing -= L_below * U_right
    if (jEnd < m && jEnd < n)
    {
      if (nb > 1)
      {
        // For nb>1: DTRSM solves L_panel * X = A_right => X = inv(L_panel) * A_right
        int intK = static_cast<int>(nb);
        int intNRHS = static_cast<int>(n - jEnd);
        Scalar alpha = 1.0;
        char side = 'L';
        char uplo = 'L';
        char trans = 'N';
        char diag = 'U';
        int lside = 1, luplo = 1, ltrans = 1, ldiag = 1;
        dtrsm_(&side, &uplo, &trans, &diag, &intK, &intNRHS, &alpha,
               workMatrix.data() + (j + j * m), &intM,
               workMatrix.data() + (j + jEnd * m), &intM,
               &lside, &luplo, &ltrans, &ldiag);
      }
      // For nb==1: L_below already contains 1/U(j,j), so A_right is U_row directly
      // For nb>1: X = inv(L_panel) * A_right computed above
      {
        int intMUpdate = static_cast<int>(m - jEnd);
        int intNRHS = static_cast<int>(n - jEnd);
        int intK = static_cast<int>(nb);
        Scalar alpha = -1.0;
        Scalar beta = 1.0;
        char ta = 'N';
        char tb = 'N';
        int lta = 1, ltb = 1;
        dgemm_(&ta, &tb, &intMUpdate, &intNRHS, &intK, &alpha,
               workMatrix.data() + (jEnd + j * m), &intM,
               workMatrix.data() + (j + jEnd * m), &intM,
               &beta,
               workMatrix.data() + (jEnd + jEnd * m), &intM,
               &lta, &ltb);
      }
    }
  }

  return logAbsDet;
}


DataContainer AlgebraEngine::InverseBlockwise(const DataContainer & A,
    UnsignedInteger blockSize)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (m == 0 || n == 0)
    throw InvalidDimensionException(HERE) << "Cannot invert an empty matrix";
  if (m != n)
    throw InvalidDimensionException(HERE) << "Matrix must be square for inverse";

  DataContainer workMatrix(m, n, 0.0);
  std::copy(A.data(), A.data() + m * n, workMatrix.data());

  int intM = static_cast<int>(m);

  // Phase 1: Block LU factorization with tracking row permutations
  std::vector<int> allPivots(m);
  for (UnsignedInteger i = 0; i < m; ++i) allPivots[i] = i;

  std::vector<int> ipiv(blockSize);

  for (UnsignedInteger j = 0; j < m; j += blockSize)
  {
    const UnsignedInteger jEnd = std::min(j + blockSize, m);
    const UnsignedInteger nb = jEnd - j;
    const UnsignedInteger panelRows = m - j;
    const UnsignedInteger k = std::min(panelRows, nb);

    int intPanelRows = static_cast<int>(panelRows);
    int intNb = static_cast<int>(nb);
    int info = 0;
    dgetrf_(&intPanelRows, &intNb, workMatrix.data() + (j + j * m), &intM, ipiv.data(), &info);
    if (info != 0)
      throw InternalException(HERE) << "LAPACK DGETRF failed with info=" << info;

    // Track overall row permutations and apply to trailing columns
    for (UnsignedInteger i = 0; i < k; ++i)
    {
      const int pivot = ipiv[i] - 1;
      if (pivot != static_cast<int>(i))
      {
        std::swap(allPivots[j + i], allPivots[j + pivot]);
        // Apply swap to already-processed columns of workMatrix
        for (UnsignedInteger col = 0; col < j; ++col)
          std::swap(workMatrix(j + i, col), workMatrix(j + pivot, col));
        // Apply swap to trailing columns
        for (UnsignedInteger col = jEnd; col < n; ++col)
          std::swap(workMatrix(j + i, col), workMatrix(j + pivot, col));
      }
    }

    // Schur complement: A_trailing -= L_below * U_right
    if (jEnd < n && nb > 0)
    {
      if (nb > 1)
      {
        // For nb>1: DTRSM solves L_panel * X = A_right => X = inv(L_panel) * A_right
        int intK = static_cast<int>(nb);
        int intNRHS = static_cast<int>(n - jEnd);
        Scalar alpha = 1.0;
        char side = 'L';
        char uplo = 'L';
        char trans = 'N';
        char diag = 'U';
        int lside = 1, luplo = 1, ltrans = 1, ldiag = 1;
        dtrsm_(&side, &uplo, &trans, &diag, &intK, &intNRHS, &alpha,
               workMatrix.data() + (j + j * m), &intM,
               workMatrix.data() + (j + jEnd * m), &intM,
               &lside, &luplo, &ltrans, &ldiag);
      }
      // For nb==1: L_below already contains 1/U(j,j), so A_right is U_row directly
      if (jEnd < m)
      {
        int intMUpdate = static_cast<int>(m - jEnd);
        int intNRHS = static_cast<int>(n - jEnd);
        int intK = static_cast<int>(nb);
        Scalar alpha = -1.0;
        Scalar beta = 1.0;
        char ta = 'N';
        char tb = 'N';
        int lta = 1, ltb = 1;
        dgemm_(&ta, &tb, &intMUpdate, &intNRHS, &intK, &alpha,
               workMatrix.data() + (jEnd + j * m), &intM,
               workMatrix.data() + (j + jEnd * m), &intM,
               &beta,
               workMatrix.data() + (jEnd + jEnd * m), &intM,
               &lta, &ltb);
      }
    }
  }

  // Phase 2: Solve A^{-1} via DTRSM (BLAS-optimized triangular solve)
  DataContainer inv(m, m, 0.0);

  // Construct permutation matrix P: allPivots[i] = original row now at position i
  // P[i, allPivots[i]] = 1
  for (UnsignedInteger i = 0; i < m; ++i)
    inv(i, allPivots[i]) = 1.0;

  // Forward solve: L * Y = P * I using DTRSM
  // L is unit lower triangular, stored in the strictly lower part of workMatrix
  {
    Scalar alpha = 1.0;
    int intNrhs = static_cast<int>(m);
    int one = 1;
    // DTRSM: op(A)*X = alpha*B => L*Y = P*I
    // side='L', uplo='L', transa='N', diag='U' (unit diagonal)
    dtrsm_((char *)"L", (char *)"L", (char *)"N", (char *)"U",
           &intM, &intNrhs, &alpha,
           workMatrix.data(), &intM,
           inv.data(), &intM, &one, &one, &one, &one);
  }

  // Back solve: U * X = Y using DTRSM
  // U is upper triangular, stored in the upper part of workMatrix
  {
    Scalar alpha = 1.0;
    int intNrhs = static_cast<int>(m);
    int one = 1;
    // DTRSM: op(A)*X = alpha*B => U*X = Y
    // side='L', uplo='U', transa='N', diag='N' (non-unit diagonal)
    dtrsm_((char *)"L", (char *)"U", (char *)"N", (char *)"N",
           &intM, &intNrhs, &alpha,
           workMatrix.data(), &intM,
           inv.data(), &intM, &one, &one, &one, &one);
  }

  return inv;
}


DataContainer AlgebraEngine::InverseSPDBlockwise(const DataContainer & A,
    UnsignedInteger blockSize)
{
  DataContainer L = ComputeCholeskyBlockwise(A, blockSize);
  L.makeUnique();
  const UnsignedInteger n = A.getSize();

  // Inverse via forward/back solve on L using DTRSM
  DataContainer inv(n, n, 0.0);
  for (UnsignedInteger i = 0; i < n; ++i)
    inv(i, i) = 1.0;

  // Forward solve: L * Y = I using DTRSM
  // L is lower triangular, stored in column-major
  {
    Scalar alpha = 1.0;
    int intN = static_cast<int>(n);
    int intNrhs = static_cast<int>(n);
    int one = 1;
    // DTRSM: L*Y = I
    // side='L', uplo='L', transa='N', diag='N' (non-unit diagonal)
    dtrsm_((char *)"L", (char *)"L", (char *)"N", (char *)"N",
           &intN, &intNrhs, &alpha,
           L.data(), &intN,
           inv.data(), &intN, &one, &one, &one, &one);
  }

  // Back solve: L^T * X = Y using DTRSM
  {
    Scalar alpha = 1.0;
    int intN = static_cast<int>(n);
    int intNrhs = static_cast<int>(n);
    int one = 1;
    // DTRSM: L^T*X = Y
    // side='L', uplo='L', transa='T', diag='N'
    dtrsm_((char *)"L", (char *)"L", (char *)"T", (char *)"N",
           &intN, &intNrhs, &alpha,
           L.data(), &intN,
           inv.data(), &intN, &one, &one, &one, &one);
  }

  return inv;
}




DataContainer AlgebraEngine::SolveLinearSystemRectangularBlockwise(
  const DataContainer & A,
  const DataContainer & b,
  UnsignedInteger blockSize)
{
  const UnsignedInteger m = A.getSize();
  const UnsignedInteger n = A.getDimension();
  if (m == 0 || n == 0)
    throw InvalidDimensionException(HERE) << "Cannot solve with empty matrix";
  if (b.getSize() != m)
    throw InvalidDimensionException(HERE) << "Right-hand side dimension=" << b.getSize() << " must match matrix row dimension=" << m;

  const UnsignedInteger nrhs = b.getDimension();
  const UnsignedInteger k = std::min(m, n);

  DataContainer workA(m, n, 0.0);
  std::copy(A.data(), A.data() + m * n, workA.data());

  DataContainer workB(m, nrhs, 0.0);
  std::copy(b.data(), b.data() + m * nrhs, workB.data());

  Scalar * wa = workA.data();
  Scalar * wb = workB.data();

  int intM = static_cast<int>(m);
  int info = -1;
  int lwork = -1;
  double workQuery = 0.0;
  dgeqrf_(&intM, &intM, wa, &intM, nullptr, &workQuery, &lwork, &info);
  lwork = static_cast<int>(workQuery);
  if (lwork < 1) lwork = 1;
  std::vector<double> work(lwork);

  for (UnsignedInteger j = 0; j < n; j += blockSize)
  {
    const UnsignedInteger jEnd = std::min(j + blockSize, n);
    const UnsignedInteger nb = jEnd - j;
    const UnsignedInteger mSub = (j < m) ? m - j : 0;
    if (mSub == 0 || nb == 0) continue;

    int intMSub = static_cast<int>(mSub);
    int intNb = static_cast<int>(nb);

    std::vector<double> tau(nb);
    info = -1;
    dgeqrf_(&intMSub, &intNb, wa + j * m + j, &intM, tau.data(), work.data(), &lwork, &info);
    if (info != 0)
      throw InternalException(HERE) << "LAPACK DGEQRF failed with info=" << info;

    const UnsignedInteger kPanel = std::min(mSub, nb);

    for (UnsignedInteger p = 0; p < kPanel; ++p)
    {
      if (tau[p] == 0.0) continue;

      std::vector<double> v(mSub, 0.0);
      v[p] = 1.0;
      for (UnsignedInteger row = p + 1; row < mSub; ++row)
        v[row] = wa[(j + p) * m + j + row];

      for (UnsignedInteger col = 0; col < nrhs; ++col)
      {
        Scalar dot = 0.0;
        for (UnsignedInteger row = p; row < mSub; ++row)
          dot += v[row] * wb[(j + row) + col * m];
        for (UnsignedInteger row = p; row < mSub; ++row)
          wb[(j + row) + col * m] -= tau[p] * v[row] * dot;
      }

      for (UnsignedInteger jj = jEnd; jj < n; ++jj)
      {
        Scalar dot = 0.0;
        for (UnsignedInteger row = p; row < mSub; ++row)
          dot += v[row] * wa[jj * m + j + row];
        for (UnsignedInteger row = p; row < mSub; ++row)
          wa[jj * m + j + row] -= tau[p] * v[row] * dot;
      }
    }
  }

  DataContainer x(n, nrhs, 0.0);
  for (UnsignedInteger col = 0; col < nrhs; ++col)
  {
    for (SignedInteger i = static_cast<SignedInteger>(k) - 1; i >= 0; --i)
    {
      Scalar sum = workB(i, col);
      for (UnsignedInteger j2 = i + 1; j2 < n; ++j2)
        sum -= workA(i, j2) * x(j2, col);
      x(i, col) = sum / workA(i, i);
    }
  }

  return x;
}

END_NAMESPACE_OPENTURNS
