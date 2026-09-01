# Docstrings: Document ResourceMap keys (batch 1 - Distribution and Factory classes)

## Summary

This PR adds documentation for `ResourceMap` keys used by distribution
and factory classes. Each documented entry appears as a bullet list in the
`Notes` section of the corresponding class docstring, following the
convention established in `AGENTS.md`.

## Classes covered

This batch focuses on **distribution factories**, **copula factories**,
and **covariance model** classes:

- `AbsoluteExponential`, `SquaredExponential`, `GeneralizedExponential`,
  `MaternModel`
- `AliMikhailHaqCopulaFactory`, `BernsteinCopulaFactory`,
  `FrankCopulaFactory`, `StudentCopulaFactory`
- `BurrFactory`, `DirichletFactory`, `FrechetFactory`,
  `GeneralizedParetoFactory`, `HistogramFactory`, `InverseNormalFactory`,
  `LogNormalFactory`, `ParetoFactory`, `RiceFactory`, `StudentFactory`,
  `VonMisesFactory`, `WhittleFactory`
- `DiracCovarianceModel`
- `CompositeDistribution`, `CompoundDistribution`,
  `DiscreteCompoundDistribution`, `Dirichlet`, `DiscreteMarkovChain`,
  `GeneralizedExtremeValue`, `JointDistribution`, `KernelMixture`,
  `LinearCombinationDistribution`, `LogNormal`, `MarginalDistribution`,
  `MeixnerDistribution`, `Mixture`, `Normal`, `PointConditionalDistribution`,
  `ProductDistribution`, `Student`, `TruncatedDistribution`,
  `TruncatedOverMesh`
- `DistributionFactory`, `DistributionImplementation`
- `CubaIntegration`, `KFactor`
- `MarginalTransformationEvaluation`, `MarginalUniformOrderStatistics`,
  `MaximumEntropyOrderStatisticsDistribution`, `OrderStatisticsMarginalChecker`,
  `QuantileMatchingFactory`, `TNC`
- `Contour`

## Key ResourceMap entries documented

Each class documents its specific ResourceMap keys, for example:

- `SquaredExponential`: `SquaredExponentialDefaultScale`
- `DirichletFactory`: `DirichletFactoryUseOptimization`
- `DistributionImplementation`: `DistributionImplementationDefaultQuantileEpsilon`,
  `DistributionImplementationDefaultIntegrationNodesNumber`
- `CubaIntegration`: `CubaIntegrationAlgorithm`, `CubaIntegrationAbsEpsilon`,
  `CubaIntegrationRelEpsilon`, `CubaIntegrationMaxEval`,
  `CubaIntegrationMinEval`, `CubaIntegrationSeed`

## Files changed

51 files changed, 456 insertions(+), 15 deletions(-).

## Notes

- This is batch 1 of 3 complementary ResourceMap documentation PRs.
  Batches 2 and 3 cover optimization/calibration classes and
  low-level implementation/utility classes respectively.
- This branch is based on the `missing_docstrings` branch which
  contains foundational docstring fixes.
