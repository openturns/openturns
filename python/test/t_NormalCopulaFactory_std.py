#! /usr/bin/env python

import openturns as ot

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
