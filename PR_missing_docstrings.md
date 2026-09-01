# Docstrings: Fix stale entries, move misplaced docs, add missing methods

## Summary

This PR cleans up and corrects the SWIG docstring documentation across the
OpenTURNS Python bindings. It removes stale entries referencing non-existent
methods, moves misplaced docstrings to the correct classes, adds missing
documentation for newly introduced methods, and fixes remaining Sphinx
undocumented entries.

## Changes

### Remove stale docstring entries (4 entries)

Remove docstrings for methods that no longer exist in the C++ API:

- `FieldImplementation::getSample` (removed from codebase)
- `FunctionImplementation::getImplementation` (removed)
- `HMatrixImplementation::assembleReal` / `assembleTensor` (removed)

### Move misplaced entries to correct classes (3 moves)

- Move `FieldToPointFunctionalChaosAlgorithm::setBlockIndices` and
  `PointToFieldFunctionalChaosAlgorithm::setBlockIndices` from
  `FieldFunctionalChaosAlgorithm_doc.i` to their respective class files.
- Move `HMatrix` docstrings from `Function_doc.i` to `HMatrix_doc.i`.
- Move `RandomVector` accessor docstrings from
  `RandomVectorImplementation_doc.i` to `RandomVector_doc.i`.

### Add docstrings for LeastSquaresMethod (computeDesign / solveNormalGram)

Add `%feature("docstring")` entries for `computeDesign` and `solveNormalGram`
on `LeastSquaresMethod` and its SWIG subclasses (`CholeskyMethod`,
`QRMethod`, `SVDMethod`, `SparseMethod`). These methods were undocumented
despite being part of the public interface.

### Add activeFunctions constructors to LeastSquaresExpansion / IntegrationExpansion

Add a new constructor accepting an `Indices` argument
(`activeFunctions`) to both `LeastSquaresExpansion` and
`IntegrationExpansion`. This allows the user to select a subset of
input variables for the expansion. Corresponding C++ code, SWIG
bindings, docstrings, and ChangeLog entry are included.

### Fix stale entries and add missing docs

- Add docstrings for `BipartiteGraph::getSize` / `getStride`.
- Add docstring for `Sample::find` and `DistFunc::dNonCentralStudentAlt0`.
- Remove stale Sphinx entries for deprecated aliases and methods that
  were moved to other classes.
- Fix remaining Sphinx undocumented entries (reduced from 12 to 2).

## Files changed

78 files changed, 302 insertions(+), 589 deletions(-).

## Notes

- The two remaining Sphinx undocumented entries are `SoizeGhanemFactory`
  (deprecated alias, intentionally skipped) and `base.TESTPREAMBLE`
  (test utility, not user-facing).
