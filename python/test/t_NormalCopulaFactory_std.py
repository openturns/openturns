#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dim = 3
R = ot.CorrelationMatrix(dim)
for i in range(dim):
    for j in range(i):
        R[i, j] = 0.5 * (1.0 + i) / dim
distribution = ot.NormalCopula(R)
size = 10000
sample = distribution.getSample(size)
factory = ot.NormalCopulaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", distribution)
print("Estimated distribution=", estimatedDistribution)

# non-regression for #572
estimated_dist = ot.NormalCopulaFactory().build(distribution.getSample(10))
mydist = ot.JointDistribution(
    ot.DistributionCollection(dim, ot.Normal()), estimated_dist
)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedNormalCopula = factory.buildAsNormalCopula(sample)
print("NormalCopula          =", distribution)
print("Estimated normalCopula=", estimatedNormalCopula)
estimatedNormalCopula = factory.buildAsNormalCopula()
print("Default normalCopula=", estimatedNormalCopula)

# Kendall estimate is singular here: fallback on Spearman's rho
singular_sample = ot.Sample(
    [[0.10, 0.10], [0.11, 0.11], [0.12, 0.12], [0.13, 0.13], [0.14, 0.14]]
)
print("Fallback estimate=", factory.build(singular_sample).getCorrelation())

# Neither Kendall nor Spearman estimates are definite positive
bad_sample = ot.Sample(
    [
        [49.0, 11.0, 6.0],
        [20.0, 49.0, 45.0],
        [4.0, 9.0, 32.0],
        [32.0, 20.0, 38.0],
        [42.0, 19.0, 18.0],
    ]
)
with ott.assert_raises(TypeError):
    factory.build(bad_sample)

# Invalid parameters
with ott.assert_raises(TypeError):
    factory.buildAsNormalCopula([2.0])
