# Docstrings: Add missing docstrings for 95 undocumented methods and classes

## Summary

This PR adds NumPydoc-style SWIG docstrings for 95 previously undocumented
methods and classes exposed to Python through the SWIG interface. The
documentation covers evaluation/function classes, gradient/hessian classes,
collection/container classes, least-squares/model classes,
distribution/factory classes, and miscellaneous utility classes.

## Classes and methods documented

### Evaluation / Function classes

- `BoxCoxEvaluation`, `InverseBoxCoxEvaluation`
- `LinearCombinationEvaluation`
- `MarginalEvaluation`, `ParametricEvaluation`
- `InverseTrendEvaluation`, `TrendEvaluation`
- `DatabaseEvaluation`
- `InverseTrendTransform`

### Gradient / Hessian classes

- `LinearGradient`, `MarginalGradient`, `ParametricGradient`
- `ParametricHessian`
- `SymbolicGradient`, `SymbolicHessian`

### Collection / Container classes

- `ARMACoefficients`
- `BasisSequence` (6 methods: `getClassName`, `getSize`, `getCoefficients`,
  `getBasis`, `add`, `isRecommended`)
- `Collection`, `Description`, `Indices`
- `IndicesCollectionImplementation` (3 entries)
- `Point`, `PointWithDescription`

### Least-squares / Model classes

- `LinearLeastSquares`, `QuadraticLeastSquares`
- `LinearModelResult`
- `TensorizedUniVariateFunctionFactory` (4 methods)
- `WelchFactory`

### Distribution / Factory classes

- `CovarianceModelFactory`, `CovarianceModelFactoryImplementation`
- `MaximumEntropyOrderStatisticsDistribution`
- `CrossEntropyResult`, `NAISResult`
- `ProbabilitySimulationResult`, `SubsetSamplingResult`

### Miscellaneous

- `ComparisonOperatorImplementation`
- `GaussianProcessFitterResult`
- `SpectralGaussianProcess`
- `StorageManager`

## Files changed

53 files changed, 851 insertions(+), 2 deletions(-).

## Notes

- All new docstrings follow the NumPydoc convention with
  `Parameters`, `Returns`, and `Notes` sections where appropriate.
- LaTeX formulas use the `R"RAW(...)RAW"` delimiter convention.
- No `**(TODO)**` placeholders remain in the new documentation.
