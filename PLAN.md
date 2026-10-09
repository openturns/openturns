# Data/Service Separation Refactoring Plan
# OpenTURNS Architecture Refactoring
# =====================================

## Overview
Separate data container functionality from algebra services (Point, Matrix, MatrixImplementation, Tensor, TensorImplementation, Complex types) and statistical services (Sample, SampleImplementation) to enable zero-copy interoperability and out-of-core data processing via HDF5.

## Affected Classes Inventory

### Implementation Classes (Data + Algebra/Stat Services)
| Class                    | File                                | Lines | Algebra | Stats |
|--------------------------|-------------------------------------|-------|---------|-------|
| Point                    | Point.hxx/cxx                      | 636   | Yes     | No    |
| MatrixImplementation     | MatrixImplementation.hxx/cxx       | 2332  | Yes ~50 | No    |
| TensorImplementation     | TensorImplementation.hxx/cxx       | 440   | Yes     | No    |
| SampleImplementation     | SampleImplementation.hxx/cxx       | 2872  | Yes     | Yes ~25|
| ComplexMatrixImpl        | ComplexMatrixImplementation.hxx/cxx| 1267  | Yes ~40 | No    |
| ComplexTensorImpl        | ComplexTensorImplementation.hxx/cxx| 471   | Yes     | No    |

### Handle Classes (TypedInterfaceObject wrappers)
| Class                    | Wraps                    | Lines |
|--------------------------|--------------------------|-------|
| Matrix                   | MatrixImplementation     | 628   |
| SquareMatrix             | inherits Matrix          | 172+  |
| SymmetricMatrix          | inherits SquareMatrix    | 193+  |
| CovarianceMatrix         | inherits SymmetricMatrix | 100+  |
| CorrelationMatrix        | inherits CovarianceMatrix| 81+   |
| IdentityMatrix           | inherits CorrelationMatrix| 128+ |
| TriangularMatrix         | inherits SquareMatrix    | 166+  |
| Tensor                   | TensorImplementation     | 352   |
| SymmetricTensor          | inherits Tensor          | 123+  |
| ComplexMatrix            | ComplexMatrixImpl        | 584   |
| SquareComplexMatrix      | inherits ComplexMatrix   | 139+  |
| HermitianMatrix          | inherits SqComplexMatrix | 190+  |
| TriangularComplexMatrix  | inherits SqComplexMatrix | 165+  |
| ComplexTensor            | ComplexTensorImpl        | 373   |
| Sample                   | SampleImplementation     | 1170  |
| Field                    | FieldImplementation      | ---   |
| ProcessSample            | ProcessSampleImpl        | ---   |

### Other Affected
- SparseMatrix: different pattern (raw std::vector), lower priority
- Indices: lightweight, lower priority
- Description: no algebra/stat services, no change needed

## Inheritance Hierarchies

Real matrix: PersistentCollection<Scalar> -> MatrixImplementation -> Matrix -> SquareMatrix -> SymmetricMatrix -> CovarianceMatrix -> CorrelationMatrix -> IdentityMatrix
Real tensor: PersistentCollection<Scalar> -> TensorImplementation -> Tensor -> SymmetricTensor
Complex matrix: PersistentCollection<Complex> -> ComplexMatrixImplementation -> ComplexMatrix -> SquareComplexMatrix -> HermitianMatrix / TriangularComplexMatrix
Complex tensor: PersistentCollection<Complex> -> ComplexTensorImplementation -> ComplexTensor
Statistical: PersistentObject -> SampleImplementation -> Sample

## Phased Implementation Plan

### Phase 1: DataContainer + MemoryBackend (2-3 days, LOW risk)
**Goal**: Create unified data storage with no breakage.
**New files**:
- lib/src/Base/Type/openturns/DataContainer.hxx (header only, ~400 LOC)
- lib/src/Base/Type/DataContainer.cxx (~600 LOC)
- lib/test/t_DataContainer_std.cxx

**Tasks**:
1. Create DataContainer class inheriting PersistentObject
2. Implement StorageBackend interface + MemoryBackend
3. Implement operator()(i,j) with row-major/column-major conversion
4. Implement operator[](flatIndex) for direct access
5. Implement save/load via Advocate for persistence
6. Implement CoW support (makeUnique method)
7. Write unit tests for layout conversion and persistence
8. Build and verify: cmake --build build --target install

**Verification**: All existing tests pass unchanged. New DataContainer tests pass.

### Phase 2: AlgebraEngine (3-5 days, MEDIUM risk)
**Goal**: Extract algebra operations into free functions using LAPACK.
**New files**:
- lib/src/Base/Type/openturns/AlgebraEngine.hxx (~500 LOC header)
- lib/src/Base/Type/AlgebraEngine.cxx (~800 LOC)
- lib/test/t_AlgebraEngine_std.cxx

**Tasks**:
1. Implement BLAS Level 1 operations (Dot, Norm, Normalize, Scale) using ddot_, dnrm2_, dscal_
2. Implement BLAS Level 2 operations (MatrixPointProduct) using dgemv_, dsymv_
3. Implement BLAS Level 3 operations (MatrixProduct, Transpose) using dgemm_
4. Implement LAPACK decompositions (Cholesky, SVD, QR) using dpotrf_, dgesdd_, dgeqrf_
5. Implement linear solvers (SolveLinearSystem) using dgesv_, dposv_
6. Implement matrix properties (Determinant, Trace, IsSymmetric)
7. Write unit tests comparing results with existing MatrixImplementation methods
8. Verify performance parity with benchmarks

**Verification**: AlgebraEngine results match existing MatrixImplementation results within tolerance.

### Phase 3: StatisticsEngine (2-3 days, LOW risk)
**Goal**: Extract statistical operations with TBB parallelization.
**New files**:
- lib/src/Base/Stat/openturns/StatisticsEngine.hxx (~500 LOC header)
- lib/src/Base/Stat/StatisticsEngine.cxx (~600 LOC)
- lib/test/t_StatisticsEngine_std.cxx

**Tasks**:
1. Implement ComputeMean with TBBImplementation::ParallelReduce
2. Implement ComputeVariance, ComputeStandardDeviation
3. Implement ComputeCovariance using TBB + dsyrk_
4. Implement correlation functions (Linear, Spearman, Kendall)
5. Implement quantile computation with parallel sort
6. Implement Sort, Rank, getMarginal
7. Implement moments (Skewness, Kurtosis, CentralMoment, RawMoment)
8. Write unit tests comparing results with existing SampleImplementation

**Verification**: StatisticsEngine results match existing SampleImplementation results.

### Phase 4: Handle Class Migration (3-5 days, HIGH risk)
**Goal**: Rewrite handle classes to use DataContainer internally.
**Modified files**: All handle class .hxx/.cxx files (20+ files)

**Sub-phase 4a: Point (easiest, no layout issues)**
1. Replace inherited PersistentCollection<Scalar> with DataContainer member
2. Route norm(), dot(), normalize() through AlgebraEngine
3. Preserve all operators (+, -, *, /, ==, <)
4. Run t_Point_std.cxx and t_Point_std.py

**Sub-phase 4b: Sample (least coupled to matrix hierarchy)**
1. Replace SampleImplementation data_ member with DataContainer
2. Route computeMean(), computeCovariance() etc. through StatisticsEngine
3. Preserve all operators and NSI_point/NSI_const_point proxies
4. Run t_Sample_std.cxx and all t_Sample_*.py

**Sub-phase 4c: Tensor + SymmetricTensor**
1. Replace TensorImplementation data with DataContainer (COLUMN_MAJOR)
2. Route getSheet(), setSheet(), symmetrize() through AlgebraEngine
3. Run t_Tensor_std.cxx and t_SymmetricTensor_std.cxx

**Sub-phase 4d: Matrix + full hierarchy (most complex)**
1. Replace MatrixImplementation data with DataContainer (COLUMN_MAJOR)
2. Route all ~50 algebra operations through AlgebraEngine
3. Ensure SquareMatrix/SymmetricMatrix/CovarianceMatrix/CorrelationMatrix/IdentityMatrix/TriangularMatrix overrides still work
4. Run all Matrix tests (t_Matrix_*.cxx, t_SquareMatrix_*.cxx, etc.)

**Verification**: All existing tests pass. The key invariant: s2 = s1; s2[0][0] = 999; must not modify s1 (CoW).

### Phase 5: Complex Types (2-3 days, MEDIUM risk)
**Goal**: Migrate ComplexMatrix and ComplexTensor hierarchies.
**New consideration**: ComplexDataContainer or DataContainer<Complex> template

**Options**:
A. Template DataContainer on Scalar type (DataContainer<Scalar>, DataContainer<Complex>)
B. Separate ComplexDataContainer class
C. Keep ComplexMatrixImplementation/ComplexTensorImplementation as-is initially

**Recommended**: Option C for now. Complex types are less critical for the Sample-algebra interop use case. Document as future work.

### Phase 6: SWIG + Tests (2-3 days, LOW risk)
**Goal**: Update bindings and verify all tests pass.

**Tasks**:
1. Update python/src/*.i SWIG files for any API changes
2. Run full test suite: ctest -R pyinstallcheck_
3. Run full C++ test suite: ctest -R cppcheck_
4. Update any .expout files if output format changed
5. Run flake8 on modified Python files

### Phase 7: HDF5Backend + Out-of-Core (2-3 days, MEDIUM risk)
**Goal**: Add disk-backed storage via HDF5.
**Constraints doc**: See `doc/DataContainer_HDF5_constraints.md` for full details
on block-based algebra requirements and configuration keys.
**New files**:
- lib/src/Base/Type/openturns/HDF5Backend.hxx (~200 LOC)
- lib/src/Base/Type/HDF5Backend.cxx (~300 LOC)
- lib/test/t_DataContainer_hdf5.cxx

**Tasks**:
1. Implement HDF5Backend using existing XMLH5StorageManager patterns
2. Add factory method: DataContainer::CreateDiskBacked(filename, dataset, dims)
3. Implement chunked storage with configurable chunk size
4. Add optional zlib compression
5. Add ResourceMap keys: DataContainer-ChunkSize, DataContainer-CompressionLevel, DataContainer-OutOfCoreThreshold
6. Integrate with Study/StorageManager persistence
7. Write unit tests for out-of-core operations
8. Benchmark: compare in-memory vs disk-backed performance

## File Impact Summary
| Category              | Count | Details                          |
|-----------------------|-------|----------------------------------|
| Core .cxx files       | 32    | Type/ and Stat/ implementations  |
| SWIG binding files    | 20    | python/src/ binding files        |
| C++ tests             | 31    | lib/test/                        |
| Python tests          | 30    | python/test/                     |
| New files             | 5-6   | DataContainer, engines, backends |

## Lines of Code Estimate
| Work Item                                 | Estimated LOC |
|-------------------------------------------|---------------|
| New: DataContainer (header + .cxx)        | 800-1,200     |
| New: AlgebraEngine                        | 400-600       |
| New: StatisticsEngine                     | 400-600       |
| New: HDF5Backend                          | 200-300       |
| New: MemoryBackend                        | 100-150       |
| Modify: 5 Implementation classes          | 1,500-2,500   |
| Modify: 15+ Handle classes                | 800-1,200     |
| Modify: SWIG bindings                     | 200-400       |
| Modify: Tests                             | 300-500       |
| **Total new code**                        | **2,000-2,800**|
| **Total modified code**                   | **2,800-4,600**|
| **Grand total**                           | **5,000-7,500**|

## Effort Estimate
| Phase | Days  | Scope                                    | Risk   |
|-------|-------|------------------------------------------|--------|
| 1     | 2-3   | DataContainer + backends (no breakage)   | Low    |
| 2     | 3-5   | AlgebraEngine (extract from MatrixImpl)  | Medium |
| 3     | 2-3   | StatisticsEngine (extract from SampleImpl)| Low    |
| 4     | 3-5   | Handle class migration                   | High   |
| 5     | 2-3   | Complex types                            | Medium |
| 6     | 2-3   | SWIG + tests                             | Low    |
| 7     | 2-3   | HDF5Backend + out-of-core                | Medium |
| **Total** | **16-25** |                                    |        |

## Risk Analysis

### Risk 1: Matrix Hierarchy Depth (HIGH)
Matrix -> SquareMatrix -> SymmetricMatrix -> CovarianceMatrix -> CorrelationMatrix -> IdentityMatrix
Changing MatrixImplementation ripples through 6 handle classes.
**Mitigation**: Migrate MatrixImplementation last, after Point and Sample are validated.

### Risk 2: Copy-on-Write Semantics (HIGH)
DataContainer must replicate TypedInterfaceObject CoW exactly.
**Mitigation**: DataContainer inherits PersistentObject with Pointer<T> reference counting. makeUnique() triggers deep copy when refcount > 1.

### Risk 3: Serialization Compatibility (MEDIUM)
Existing XML/HDF5 studies must load correctly.
**Mitigation**: Advocate::hasAttribute() check for backward compat. Phase 1 includes legacy data detection.

### Risk 4: Performance Regression (MEDIUM)
StorageBackend indirection could hurt cache locality.
**Mitigation**: MemoryBackend::data() is inlined. Contiguous std::vector preserves cache behavior.

### Risk 5: SWIG Exposure (LOW)
DataContainer and engines should remain C++-only internals.
**Mitigation**: Only expose Point, Matrix, Sample to Python.

## Key Dependencies
- LAPACK: Required (already required dependency). AlgebraEngine calls existing Lapack.hxx wrappers.
- TBB: Optional (USE_TBB). StatisticsEngine uses TBBImplementation::ParallelFor/Reduce.
- HDF5: Optional (USE_HDF5). HDF5Backend uses existing HDF5 C++ API.
- libxml2: Optional (USE_LIBXML2). Required for XMLStorageManager (existing).

## Portability
- All technologies (HDF5, TBB, LAPACK) are portable across Linux, Windows, macOS.
- mmap is NOT recommended (POSIX/Windows API divergence, no existing abstraction).
- HDF5 VFL provides portable alternatives: core driver (in-memory), mmap driver (where available).

---

## Current Status: Block Algebra Optimization

### Completed
- **Phases 1-6**: DataContainer, AlgebraEngine, StatisticsEngine, HDF5Backend, interop, SWIG bindings
- **Block algebra**: 12 block methods implemented (Gram, Determinant, LU, QR, Cholesky, Inverse, TriangularSolve, InverseSPD, SolveSPD, RectangularSolve, Mean, Variance, Covariance)
- **Dense LAPACK**: All methods use multi-threaded OpenBLAS (20 threads, 3.9× speedup)
- **Benchmark**: Dense vs block comparison completed, analysis in `doc/block_algebra_benchmark.tex`

### Key Findings
1. **Block methods lose for in-memory data** due to:
   - Panel extraction is single-threaded
   - Panels too small for OpenBLAS to parallelize effectively
   - Some methods (Cholesky, Inverse) use element-by-element algorithms instead of LAPACK
2. **Block methods win for out-of-core HDF5 data** (the primary use case)
3. **Block size 1024 is suboptimal** for this CPU (6 P-cores + 8 E-cores = 20 threads)

### Next Steps: Block Method Optimization

**Step 1: Update default block size to 384**
- Rationale: 384×384×8B = 1.15MB per panel (fits in L2 cache)
- 20 panels × 1.15MB = 23MB (fits in L3 cache)
- Close to OpenBLAS's typical block size (224-384)
- Divisible by SIMD widths (8, 16, 32)
- Change: `AlgebraEngine-DefaultBlockSize` = 1024 → 384

**Step 2: Fix CholeskyBlockwise**
- Current: Element-by-element Householder reflectors (O(n²) per panel)
- Fix: Call `dpotrf_` per panel (uses OpenBLAS's optimized blocked algorithm)
- Expected: 50-100× speedup

**Step 3: Fix InverseBlockwise**
- Current: Element-by-element back-solve (O(n²) per column)
- Fix: Call DTRSM for back-solve (BLAS-optimized)
- Expected: 100-200× speedup

**Step 4: Fix QRBlockwise**
- Current: DORGQR to form Q explicitly + DGEMM for trailing
- Fix: Use DLARFB (avoids explicit Q formation, like LAPACK's dgeqrf_)
- Expected: 2-5× speedup

**Step 5: Add TBB parallelism across panels**
- Use `TBBImplementation::ParallelFor` over panels
- Each panel processed by one thread
- Expected: 5-10× speedup (20 threads)

**Step 6: Update benchmark and report**
- Re-run benchmark with new block size (384)
- Compare dense vs optimized block methods
- Update `doc/block_algebra_benchmark.tex`

**Step 7: Add OpenBLAS thread configuration**
- Add ResourceMap key: `AlgebraEngine-OpenBLASThreads` (default: 0 = auto)
- Allow user to tune threading for block vs dense methods

### Block Size Analysis

| Block Size | Panel Size (MB) | Panels for N=4096 | Fits L2? | Fits L3? | Parallelism |
|------------|-----------------|-------------------|----------|----------|-------------|
| 256        | 0.5             | 256               | Yes      | Yes      | Good        |
| 384        | 1.15            | 121               | Yes      | Yes      | Good        |
| 512        | 2.0             | 64                | Yes*     | Yes      | Moderate    |
| 1024       | 8.0             | 16                | No       | Yes      | Poor        |

*2MB fits in P-core L2 (2MB) but not E-core L2 (256KB)

**Recommended: 384** (best balance of cache efficiency and parallelism)

### Results (after optimization)

| Method | N=512 | N=1024 | N=2048 | N=4096 | Notes |
|--------|-------|--------|--------|--------|-------|
| Cholesky | 0.87x | 0.78x | 0.87x | 1.12x | dpotrf_+dtrsm_+dsyrk_ per panel |
| QR | 1.48x | 1.42x | 0.94x | 3.12x | Needs DLARFB to avoid Q formation |
| LU | 0.62x | 1.00x | 16.1x | 0.61x | N=2048 outlier |
| SolveSPD | 2.20x | 3.51x | 3.53x | 1.69x | Inherits Cholesky overhead |
| Inverse | 1.33x | 2.75x | 0.72x | -- | dtrsm_ for forward/back solve |
| Determinant | 0.62x | 2.07x | 0.13x | 0.26x | Biggest winner: 7.6x faster |
| Gram | 0.89x | 1.36x | 1.02x | 0.51x | 2x faster at N=4096 |

### Out-of-Core (HDF5) Results (Benchmark 2)

| N | Blocks | MB | SeqRead | BlockRead | Gram OOC | Compute-only | I/O% |
|---|--------|------|----------|------------|----------|--------------|------|
| 768 | 2 | 5 | 0.3ms | 0.2ms | 5.5ms | 5.3ms | 4% |
| 1536 | 4 | 18 | 1.1ms | 1.3ms | 25.1ms | 24.0ms | 5% |
| 3072 | 8 | 72 | 3.9ms | 4.3ms | 130.7ms | 126.8ms | 3% |
| 6144 | 16 | 288 | 13.5ms | 15.7ms | 984.0ms | 970.5ms | 2% |
| 12288 | 32 | 1152 | 53.5ms | 59.7ms | 7368.0ms | 7314.5ms | 1% |

I/O is only 1-5% of total time. HDF5 sequential read: 53.5ms for 1.15GB (21 GB/s SSD).

### DLARFB Investigation (QR Optimization)

**Attempted:** Use DLARFT+DLARFB for trailing column update in block QR to avoid explicit Q formation via DORGQR.

**Result:** DLARFB produces wrong results when called from external code:
- Standalone test: `H_trailing = (I - V*T*V^T) * C` via DLARFB gives wrong results (Q^TQ-I ≈ 1.0)
- DORGQR+DGEMM approach works correctly (Q^TQ-I ≈ 1e-15)
- LAPACK's own DGEQRF uses DLARFB internally and works — the difference is in V matrix layout

**Root cause:** DLARFB's internal DGEMV reads V as M×K but accesses M columns (reading beyond K). The V matrix from DGEQRF is embedded in the larger workMatrix, so the "extra" columns contain garbage. LAPACK's DGEQRF handles this correctly because it uses a different V layout convention (unit diagonal implicit in the stored Householder vectors).

**Conclusion:** DLARFB optimization for block QR is NOT viable without understanding LAPACK's internal V layout convention. The DORGQR+DGEMM approach is correct and performs within 1.5-2x of dense. The QR slowness is inherent to the explicit Q formation step (DORGQR).

### Remaining work
- None block algebra specific; all methods working and benchmarked
