# DataContainer HDF5 Backend Constraints

## Status
- DataContainer + MemoryBackend: DONE
- AlgebraEngine (LAPACK/BLAS): DONE
- StatisticsEngine (TBB parallel): DONE
- HDF5Backend: DONE (create, Open, readBlock, writeBlock, resize, clone)
- Block-based statistics (Mean, Variance, Covariance): DONE
- Block-based algebra (DGEMM, DSYRK, DPOTRF, SPD solve): DONE
- Block-based QR (DGEQRF + DORGQR + trailing DGEMM): DONE
- Block-based LU (DGETRF + trailing DTRSM + DGER/DGEMM): DONE
- Block-based triangular solve (DTRSM): DONE
- Block-based Gram (A^T*A / A*A^T via DSYRK): DONE

## HDF5 Backend Requirements

HDF5 is a storage format only. It does NOT provide algebra operations.
The HDF5 backend will enable out-of-core processing of large matrices.

### Storage Backend Interface (already defined)
```cpp
class StorageBackend {
  virtual Scalar& operator[](UnsignedInteger index) = 0;
  virtual const Scalar& operator[](UnsignedInteger index) const = 0;
  virtual Scalar& operator()(UnsignedInteger i, UnsignedInteger j) = 0;
  virtual void makeUnique() = 0;
  virtual UnsignedInteger stride(UnsignedInteger axis) const = 0;
};
```

### Block-Based Algebra Constraints
For LAPACK operations on HDF5-resident data, chunks must be loaded
into memory, processed, and written back:

1. **Matrix-matrix multiply (DGEMM):** Split A and B into tiles that fit
   in memory. Accumulate results tile-by-tile. O(n) temp buffer needed
   for result row/column.

2. **Cholesky (DPOTRF):** Block Cholesky - factorize one panel at a time,
   update trailing submatrix. Each panel load/store is an HDF5 slab I/O.

3. **SVD/QR:** Blocked Householder reflectors need careful tile management.
   TSQR for QR, divide-and-conquer for SVD are natural block algorithms.

4. **Linear solve:** Blocked LU with partial pivoting. Row swaps require
   random-access HDF5 slab writes (expensive).

### Key Constraints
- HDF5 random-access slab writes are slow (internal open/close/seek)
- LAPACK needs contiguous memory - cannot pass HDF5 dataset handles
- CoW on HDF5 requires copy-on-write page management (similar to OS VM)
- Threshold-based dispatch: small matrices use in-memory LAPACK,
  large matrices use block-HDF5

### Configuration Keys (future)
- DataContainer-OutOfCoreThreshold: Size above which HDF5 backend is used
- DataContainer-ChunkSize: HDF5 chunk dimensions
- DataContainer-CompressionLevel: zlib level 0-9
- DataContainer-BlockSize: Number of rows per block for block algebra

### Affected AlgebraEngine Methods (all need block variants)
- MatrixProduct (DGEMM)
- ComputeCholesky (DPOTRF)
- ComputeSVD (DGESDD)
- ComputeQR (DGEQRF)
- SolveLinearSystem (DGESV/DPOSV)
- ComputeDeterminant (DGETRF+DGETRI)
- Transpose (block transpose with slab I/O)

### Affected StatisticsEngine Methods (need block variants)
- ComputeMean
- ComputeVariance / ComputeStandardDeviation
- ComputeCovariance (with dsyrk_)
- ComputeQuantile (needs sorted data - block sort)
- ComputeSkewness / ComputeKurtosis
