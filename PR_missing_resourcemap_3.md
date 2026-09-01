# Docstrings: Document ResourceMap keys (batch 3 - Implementation, Utility, and Low-level classes)

## Summary

This PR adds documentation for `ResourceMap` keys used by
implementation-level classes, utility classes, sequences, and other
low-level components. Each documented entry appears as a bullet list in
the `Notes` section of the corresponding class docstring.

## Classes covered

This batch focuses on **implementation classes**, **utility classes**,
**low-discrepancy sequences**, **Sobol indices**, and **miscellaneous
infrastructure**:

### Implementation classes

- `ClassifierImplementation`, `CovarianceModelFactoryImplementation`
- `DomainImplementation`, `DrawableImplementation`
- `DualLinearCombinationEvaluation`, `EvaluationImplementation`
- `GraphImplementation`, `HMatrixImplementation`
- `LinearCombinationEvaluation`
- `LowDiscrepancySequenceImplementation`
- `Matrix`, `Mesh`
- `SparseMethod`, `SpectralGaussianProcess`
- `Tensor`, `UniVariatePolynomialImplementation`

### Utility and algorithm classes

- `Compact`, `ComplexTensor`
- `DesignProxy`, `DirectionalSampling`
- `EnclosingSimplexAlgorithmImplementation`
- `FieldToPointConnection`
- `FunctionalChaosAlgorithm`, `FunctionalChaosResult`,
  `FunctionalChaosSobolIndices`
- `GaussianProcess`, `GaussianProcessConditionalCovariance`,
  `GaussianProcessFitter`, `GaussianProcessRegressionCrossValidation`
- `HSICEstimatorConditionalSensitivity`
- `HyperbolicAnisotropicEnumerateFunction`
- `KFold`
- `LeastSquaresMetaModelSelectionFactory`
- `Os`, `Pie`, `Text`, `VisualTest`
- `PiecewiseHermiteEvaluation`, `PiecewiseLinearEvaluation`
- `PointToFieldFunctionalChaosAlgorithm`, `PointToPointEvaluation`
- `RandomWalkMetropolisHastings`
- `SobolIndicesAlgorithmImplementation`
- `SymbolicParserExprTk`, `SymbolicParserMuParser`
- `Ridgway`, `Fehlberg`

### Low-discrepancy sequences

- `FaureSequence`, `HaltonSequence`, `HaselgroveSequence`,
  `ReverseHaltonSequence`, `SobolSequence`

### Genz integration

- `Genz`

### Other

- `Sample`, `SpecFunc`
- `PolyaFactory`
- `PolaFactory`

## Key ResourceMap entries documented

Each class documents its specific ResourceMap keys, for example:

- `GraphImplementation`: `GraphImplementationDefaultHorizontalMargin`,
  `GraphImplementationDefaultVerticalMargin`,
  `GraphImplementationDefaultLegendFontSize`,
  `GraphImplementationDefaultTitleFontSize`
- `Sample`: `SampleHashSize`
- `Genz`: `GenzAlgorithmSamplingSize`
- `SymbolicParserExprTk`: `SymbolicParserExprTkPrecision`

## Files changed

57 files changed, 518 insertions(+), 19 deletions(-).

## Notes

- This is batch 3 of 3 complementary ResourceMap documentation PRs.
- This branch is based on the `missing_docstrings` branch.
- Together, batches 1, 2, and 3 document ResourceMap keys across the
  full OpenTURNS class hierarchy.
