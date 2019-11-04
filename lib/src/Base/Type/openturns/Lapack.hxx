//                                               -*- C++ -*-
/**
 *  @brief This file includes all of the Lapack functions used in the platform
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_LAPACK_HXX
#define OPENTURNS_LAPACK_HXX

#include "openturns/OTconfig.hxx"

BEGIN_C_DECLS

/** Function daxpy is to be used to compute the sum or the difference of two matrices
    y := alpha*x + y */
void daxpy_(int *n, double *alpha, double *x, int *incx, double *y, int *incy);

/** Function dscal is to be used to compute the product of a matrix by a numerical scalar
    x := alpha*x */
void dscal_(int *n, double *alpha, double *x, int *incx);

/** Function ddot is to be used to compute the dot product of two vectors
    dot := y'*x */
double ddot_(int *n, double *x, int *incx, double *y, int *incy);

/** Function dnrm2 is to be used to compute the Euclidean norm of a vector
    dnrm2 := sqrt(x'*x) */
double dnrm2_(int *n, double *x, int *incx);

/** Function dasum is to be used to compute the 1-norm of a vector
    dasum := |x_1|+...+|x_n| */
double dasum_(int *n, double *x, int *incx);

/** Function drot is to be used to generate a Givens rotation
    [ c  s]   [a]   [r]
    [-s  c] * [b] = [0] */
void drotg_(double *a, double *b, double *c, double *s);

/** Function drot is to be used to apply a Givens rotation defined by (c, s) to a sequence of 2D points (x_i, y_i), i=0..n-1. The transformation is done in-place. */
void drot_(int *n, double *x, int *incx, double *y, int *incy, double *c, double *s);

/** Function dgemv is to be used to compute the product of a matrix with a vector (numerical point)
    y := alpha*A(trans)*x + beta*y */
void dgemv_(char *trans, int *m, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int *ltrans);

/** Function dsymv is to be used to compute the product of a matrix with a vector (numerical point);  optimization for symmetric matrices
    y := alpha*A(trans)*x + beta*y */
void dsymv_(char *uplo, int *n, double *alpha, double *a, int *lda, double *x, int *incx, double *beta, double *y, int *incy, int *luplo);

/** Function dspmv is to be used to compute the product of a matrix with a vector (numerical point);  optimization for positive definite matrices
    y := alpha*A*x + beta*y */
void dspmv_(char *uplo, int *n, double *alpha, double *a, double *x, int *incx, double *beta, double *y, int *incy, int *luplo);

/** Function dgemm is to be used to compute the product of two matrices
    c := alpha*a(trans)*b(trans) + beta*c */
void dgemm_(char *transa, char *transb, int *m, int *n, int *k, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int *ltransa, int *ltransb);

/** Function dsymm is to be used to compute the product of two matrices;  optimization for symmetric matrices
    c := alpha*a*b + beta*c if side is l or alpha*b*a + beta*c if side is r */
void dsymm_(char *side, char *uplo, int *m, int *n, double *alpha, double *a, int *lda, double *b, int *ldb, double *beta, double *c, int *ldc, int *lside, int *luplo);

/** Function dgeev is used to compute the eigenvalues of a square matrix */
void dgeev_(char *jobvl, char *jobvr, int *n, double *a, int *lda, double *wr, double *wi,
            double *vl, int *ldvl, double *vr, int *ldvr, double *work, int *lwork, int *info, int *ljobvl, int *ljobvr);

/** Function dsyev is used to compute the eigenvalues of a symmetric matrix */
void dsyev_(char *jobz, char *uplo, int *n, double *a, int *lda, double *w, double *work, int *lwork, int *info, int *ljobz, int *luplo);

/** Function dgelsy is used to solve the linear system corresponding to a matrix */
void dgelsy_(int *m, int *n, int *nrhs, double *a, int *lda, double *b, int *ldb, int *jpvt, double *rcond, int *rank, double *work, int *lwork, int *info);

/** Function dgesv is used to solve the linear system corresponding to a square matrix */
void dgesv_(int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, int *info);

/** Function dsysv is used to solve the linear system corresponding to a symmetric matrix */
void dsysv_(char *uplo, int *n, int *nrhs, double *a, int *lda, int *ipiv, double *b, int *ldb, double *work, int *lwork, int *info, int *luplo);

/** Function dpotrf is used to check if a matrix is positive definite */
void dpotrf_(char *uplo, int *n, double *a, int *lda, int *info, int *luplo);

/** Function dgetrf is used to make the LU factorisation of a matrix */
void dgetrf_(int *m, int *n, double *a, int *lda, int *ipiv, int *info);

/** Function dsytrf is used to make the LU factorisation of a symmetric matrix */
void dsytrf_(char *uplo, int *n, double *a, int *lda, int *ipiv, double *work, int *lwork, int *info, int *luplo);

/** Function dgetrf is used to make the QR factorisation of a matrix */
void dgeqrf_(int *m, int *n, double *a, int *lda, double *tau, double *work, int *lwork, int *info);

/** Function dorgqr is used to generate a real matrix Q with orthogonal columns */
void dorgqr_(int *m, int *n, int *k, double *a, int *lda, double *tau, double *work, int *lwork, int *info);

/** Function dger is used to make the Kronecker product of two vectors */
void dger_(int *m, int *n, double *alpha, double *x, int *incx, double *y, int *incy, double *a, int *lda);

/** Function dstev computes the eigenvalues and eigenvectors of a symmetric tridiagonal matrix */
void dstev_(char *jobz, int *n, double *d, double *e, double *z, int *ldz, double *work, int *info, int *ljobz);

/** Function dsyr performs a rank one update on a square matrix A -> A + alpha * x . x' */
void dsyr_(char *uplo, int *n, double *alpha, double *x, int *incx, double *a, int *lda, int *luplo);

/** Function dsyr performs a rank one update on a square matrix A -> A + alpha * x . x' */
void dsyrk_(char *uplo, char *trans, int *n, int *k, double *alpha, double *a, int *lda, double *beta, double *C, int *ldc, int *luplo, int *ltrans);

/** Function dtrsm solves a triangular linear system A.X = alpha B, where A can be transposed or not */
void dtrsm_(char *side, char *uplo, char *transa, char *diag, int *m, int *n, double *alpha, double *A, int *lda, double *B, int *ldb, int *lside, int *luplo, int *ltransa, int *ldiag);

/** Function dgesdd computes singular values by using a divide and conquer strategy.
    The number of singular values is the min of the column dimension and the row dimension. */
void dgesdd_(char *jobz, int *m, int *n, double *A, int *lda, double *S, double *U, int *ldu, double *VT, int *ldvt, double *work, int *lwork, int *iwork, int *info, int *ljobz);

/** Function dtrmv is to be used to compute the product of a triangular
    (upper or lower) matrix with a vector (numerical point) */
void dtrmv_(char *uplo, char *trans, char *diag, int *n, double *A, int *lda, double *X, int *incx, int *luplo, int *ltrans, int *ldiag);

/** Function dtrmm is to be used to compute the product of a triangular (upper or lower) matrix with another matrix  */
void dtrmm_(char *side, char *uplo, char *trans, char *diag, int *m, int *n, double *alpha, double *A, int *lda, double *B, int *ldb, int *lside, int *luplo, int *ltrans, int *ldiag);

/** BLAS routines for complex values */

/** Function zaxpy is to be used to compute the sum or the difference of two complex matrices
    y := alpha*x + y */
void zaxpy_(int *n, std::complex<double> *alpha, std::complex<double> *x, int *incx, std::complex<double> *y, int *incy);

/** Function zscal is to be used to compute the product of a complex matrix by a numerical complex
    x := alpha*x */
void zscal_(int *n, std::complex<double> *alpha, std::complex<double> *x, int *incx);

/** Function zgemm is to be used to compute the product of two complex matrices
    c := alpha*a(trans)*b(trans) + beta*c */
void zgemm_(char *transa, char *transb, int *m, int *n, int *k, std::complex<double> *alpha, std::complex<double> *a, int *lda,
            std::complex<double> *b, int *ldb, std::complex<double> *beta, std::complex<double> *c, int *ldc, int *ltransa, int *ltransb);

/** Function zsymm is to be used to compute the product of two complex matrices
 *  optimization for symmetric matrices
 c := alpha*a*b + beta*c if side is l or alpha*b*a + beta*c if side is r */
void zsymm_(char *side, char *uplo, int *m, int *n, std::complex<double> *alpha, std::complex<double> *a, int *lda,
            std::complex<double> *b, int *ldb, std::complex<double> *beta, std::complex<double> *c, int *ldc, int *lside, int *luplo);

/** Function zhemm is to be used to compute the product of two complex matrices
 *  optimization for hermitian matrices
 c := alpha*a*b + beta*c if side is l or alpha*b*a + beta*c if side is r */
void zhemm_(char *side, char *uplo, int *m, int *n, std::complex<double> *alpha, std::complex<double> *a, int *lda,
            std::complex<double> *b, int *ldb, std::complex<double> *beta, std::complex<double> *c, int *ldc, int *lside, int *luplo);

/** Function zpotrf is used to computes the Cholesky factorization of a complex hermitian positive definite matrix  */
void zpotrf_(char *uplo, int *n, std::complex<double> *a, int *lda, int *info, int *luplo);

/** Function zgemv is to be used to compute the product of a complex matrix with a complex vector
    y := alpha*A(trans)*x + beta*y */
void zgemv_(char *trans, int *m, int *n, std::complex<double> *alpha, std::complex<double> *a, int *lda, std::complex<double> *x, int *incx,
            std::complex<double> *beta, std::complex<double> *y, int *incy, int *ltrans);

/** Function zhemv is to be used to compute the product of a complex matrix with a vector ;  optimization for hermitian matrices
    y := alpha*A(trans)*x + beta*y */
void zhemv_(char *uplo, int *n, std::complex<double> *alpha, std::complex<double> *a, int *lda, std::complex<double> *x, int *incx,
            std::complex<double> *beta, std::complex<double> *y, int *incy, int *luplo);

/** Function zcopy is to be used to copy a vector x to a vector y */
void zcopy_(int *n, std::complex<double> *x, int *incx, std::complex<double> *y, int *incy);

/** Function ztrmv is to be used to compute the product of a triangular (upper or lower) matrix with a vector  */
void ztrmv_(char *uplo, char *trans, char *diag, int *n, std::complex<double> *A, int *lda, std::complex<double> *X, int *incx, int *luplo, int *ltrans, int *ldiag);

/** Function ztrmm is to be used to compute the product of a triangular (upper or lower) matrix with another matrix  */
void ztrmm_(char *side, char *uplo, char *trans, char *diag, int *m, int *n, std::complex<double> *alpha, std::complex<double> *A, int *lda, std::complex<double> *B, int *ldb, int *lside, int *luplo, int *ltrans, int *ldiag);

/** Function ztrmm is to be used to compute the product of a triangular (upper or lower) matrix with another matrix  */
void zgelsy_(int *m, int *n, int *nrhs, std::complex<double> *A, int *lda, std::complex<double> *B, int *ldb, int *jpvt, double *rcond, int *rank, std::complex<double> *work, int *lwork, double *rwork, int *info);

/** Function dposv is to be used to solve a linear system with a symmetric definite positive matrix */
void dposv_(char *uplo, int *n, int *nrhs, double *A, int *lda, double *B, int *ldb, int *info, int *luplo);

END_C_DECLS

#endif /* OPENTURNS_LAPACK_HXX */
