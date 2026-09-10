# Plan: HMAT support for SparseGaussianProcess

Status: **not implemented yet**. This document is the design plan; implementation
proceeds in the phases listed at the end.

## Motivation

The dense GP machinery (`GaussianProcessFitter`, `GaussianProcessRegression`,
`GeneralLinearModelAlgorithm`, `KrigingAlgorithm`, `GaussianProcess`) exposes a
linear-algebra backend selector `enum LinearAlgebra { LAPACK, HMAT }`, controlled
by a `*-LinearAlgebra` ResourceMap key, and uses the hierarchical-matrix library
(`libhmat`) to assemble/factorize/solve the N x N covariance matrix when `HMAT` is
selected. This keeps dense GP usable for large N by trading exact dense algebra for
an approximation with bounded error.

The sparse GP classes (`SparseGaussianProcessFitter`, `SparseGaussianProcessRegression`,
`SparseGaussianProcessEvaluation`, `SparseGaussianProcessHessian`) are currently
**LAPACK-only**: they were written with dense `TriangularMatrix`/`CovarianceMatrix`
members and no HMatrix. This plan extends the same `LAPACK/HMAT` switch to the
sparse GP family.

## Where the sparse GP does linear algebra

The collapsed-ELBO computation (`SparseGaussianProcessFitter::computeELBOValue`,
`computeELBOGradient`) works on the following matrices (N = number of observations,
M = number of inducing points, M <= N):

| Quantity   | Size | Usage |
|------------|------|-------|
| Kuu        | M x M | Cholesky `Luu` (whitening factor), `KuuBar` in gradient |
| Kfu/Kuf    | N x M | cross covariance between input sample and inducing points |
| A = Kuf Luu^-1 | N x M | whitened cross covariance |
| G = A^T A  | M x M | quadratic form, Cholesky `Lg` |
| B = G + sigma^2 I | M x M | Cholesky `Lb` (variational posterior) |
| Kff        | N x N | used **only** through its diagonal (trace term) |

Key point: the sparse ELBO never assembles the N x N matrix Kff (only its diagonal
is needed), so the large object in a sparse fit is the **N x M** cross covariance,
which is inherently dense. The M x M matrices are the ones factorized. HMAT support
therefore targets the **M x M Kuu factorization** (when M is large) and the
**whitened solve for prediction**; the N x M assembly remains dense, exactly as in
the dense GP where the cross-covariance with the training sample is also dense.

## Design (mirroring the dense GPF)

### SparseGaussianProcessFitterResult

- Add `enum LinearAlgebra { LAPACK, HMAT };` (same values/meaning as
  `GaussianProcessFitterResult::LinearAlgebra`).
- Add member `LinearAlgebra linearAlgebraMethod_` (default `LAPACK`), a constructor
  parameter, an accessor `getLinearAlgebraMethod()`, and persist it in
  `save`/`load` (as an int, like the dense result does with `linearAlgebraMethod_`).
- When `HMAT`, also store the HMatrix whitening factor (the factorized Kuu) so that
  prediction can reuse it. The dense `whiteningFactor_` (TriangularMatrix) remains
  the LAPACK representation. Note: like the dense `GaussianProcessFitterResult`,
  the HMatrix itself is a runtime-only object and is **not** persisted; after a
  reload the model must be refit under LAPACK (documented limitation, identical to
  the dense HMAT path).

### SparseGaussianProcessFitter

- Add `method_` member + `getMethod()/setMethod(LinearAlgebra)`.
- `initializeMethod()` reads `ResourceMap::GetAsString("SparseGaussianProcessFitter-LinearAlgebra")`
  (new ResourceMap key, values `LAPACK`/`HMAT`, default `LAPACK`), called from the
  constructors, mirroring `GaussianProcessFitter::initializeMethod()`.
- `run()` passes the method to the result.
- `computeELBOValue`: HMAT branch (guarded by `#ifdef OPENTURNS_HAVE_HMAT`):
  - Build/factorize Kuu via `HMatrixFactory::build(inducingPoints, 1, true, params)`,
    `assemble(CovarianceAssemblyFunction(reducedCovarianceModel_, inducingPoints), 'L')`,
    `factorize(params.getFactorizationMethod())`.
  - `A^T = L^-1 Kuf` via `hmatrix.solveLower(Kuf)` (Kuu = L L^T, A = Kuf L^-T).
  - log-det of Kuu is not needed (only B = G + sigma^2 I is factorized for the
    ELBO), so the M x M B/Lb/G/Lg algebra is unchanged and stays dense.
  - Store the HMatrix factor as the whitening factor for prediction.
- `computeELBOGradient`: HMAT branch throws `NotYetImplementedException`, exactly as
  `GaussianProcessFitter`'s reduced-log-likelihood gradient is LAPACK-only. The
  analytic ELBO gradient (LAPACK) is unchanged.

### SparseGaussianProcessEvaluation

- Add an `HMatrix` member (guarded by `OPENTURNS_HAVE_HMAT`) holding the whitened
  factor, and dispatch `computeWhitenedCovariance` between
  `whiteningFactor_.solveLinearSystem(kZX)` (LAPACK) and
  `whiteningFactorHMatrix_.solveLower(kZX)` (HMAT). `SparseGaussianProcessGradient`
  and `SparseGaussianProcessHessian` use `computeWhitenedCovariance`, so they are
  covered automatically.

### SparseGaussianProcessRegression

- `run()` forwards the fitter result (method + factors) into the
  `SparseGaussianProcessEvaluation`.

## Wiring

- CMake: the HMatrix classes are already built when `USE_HMAT=ON`; the sparse
  classes only need the `#ifdef OPENTURNS_HAVE_HMAT` guards and the hmat includes.
- SWIG: expose `getMethod()/setMethod()` on the sparse fitter and
  `getLinearAlgebraMethod()` on its result (int, like the dense classes).
- Tests: add a Python test `t_SparseGaussianProcessFitter_std_hmat.py` (gated on
  `HMAT_FOUND` in `python/test/CMakeLists.txt`) comparing HMAT vs LAPACK ELBO,
  posterior mean/covariance and metamodel evaluations, mirroring the existing
  `t_GaussianProcessFitter_std_hmat.py`.

## Phases

1. **Plumbing** (this document + enum/method/ResourceMap/SWIG/persistence). Builds
   under LAPACK and HMAT builds; behaviour unchanged when `LAPACK` is selected.
2. **HMAT ELBO value** in `computeELBOValue` (guarded) + HMAT whitening factor.
3. **HMAT prediction** dispatch in `SparseGaussianProcessEvaluation` (+ gradient and
   hessian through `computeWhitenedCovariance`) and `SparseGaussianProcessRegression`.
4. **Tests + docs**: hmat python test, ChangeLog, class docstrings.