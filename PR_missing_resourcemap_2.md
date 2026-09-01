# Docstrings: Document ResourceMap keys (batch 2 - Optimization, Calibration, and Algorithm classes)

## Summary

This PR adds documentation for `ResourceMap` keys used by optimization
algorithms, calibration methods, simulation algorithms, and related
classes. Each documented entry appears as a bullet list in the `Notes`
section of the corresponding class docstring.

## Classes covered

This batch focuses on **optimization algorithms**, **calibration**,
**integration**, **simulation**, and **model fitting** classes:

- `AbdoRackwitz`, `Cobyla`, `SQP` (optimization algorithms)
- `AdaptiveDirectionalStratification`, `AdaptiveStieltjesAlgorithm`
- `BlendedStep`, `NonCenteredFiniteDifferenceGradient`
- `CalibrationResult`, `GaussianLinearCalibration`,
  `LinearLeastSquaresCalibration`
- `BoxCoxFactory`, `ARMALikelihoodFactory`
- `EfficientGlobalOptimization`
- `FittingTest`, `MaximumLikelihoodFactory`, `MethodOfMomentsFactory`
- `LeastSquaresDistributionFactory`
- `GaussKronrod`, `GaussLegendre` (integration)
- `GeneralLinearModelAlgorithm`, `KrigingAlgorithm`
- `KarhunenLoeveP1Algorithm`, `KarhunenLoeveQuadratureAlgorithm`,
  `KarhunenLoeveSVDAlgorithm`
- `LinearModelAnalysis`, `LinearModelResult`,
  `LinearModelStepwiseAlgorithm`, `LinearModelTest`
- `ProfileLikelihoodResult`
- `AnalyticalResult`, `Analytical`
- `FAST`, `MultiFORM`, `SystemFORM`
- `NAIS`, `SubsetSampling`, `SimulationAlgorithm`
- `SimulationSensitivityAnalysis`, `StrongMaximumTest`
- `PenalizedLeastSquaresAlgorithm`
- `SimplicialCubature`
- `Dlib`
- `PosteriorDistribution`
- `CholeskyMethod`
- `ProbabilitySimulationResult`
- `RootStrategyImplementation`
- `Pagmo`
- `Last`

## Key ResourceMap entries documented

Each class documents its specific ResourceMap keys, for example:

- `AbdoRackwitz`: `AbdoRackwitzMaximumIterationNumber`,
  `AbdoRackwitzMaximumAbsoluteError`, `AbdoRackwitzMaximumRelativeError`,
  `AbdoRackwitzMaximumConstraintError`
- `Cobyla`: `CobylaMaximumIterationNumber`, `CobylaMaximumAbsoluteError`,
  `CobylaMaximumRelativeError`, `CobylaMaximumConstraintError`
- `SQP`: `SQPMaximumIterationNumber`, `SQPMaximumAbsoluteError`,
  `SQPMaximumRelativeError`, `SQPMaximumConstraintError`
- `Pagmo`: 80 keys covering algorithm selection, island configuration,
  and evolution parameters

## Files changed

50 files changed, 475 insertions(+), 14 deletions(-).

## Notes

- This is batch 2 of 3 complementary ResourceMap documentation PRs.
- This branch is based on the `missing_docstrings` branch.
- The `Pagmo` class alone documents 80 ResourceMap keys covering the
  full configuration of the pagmo2 optimization framework interface.
