#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    dim = 3
    R = CorrelationMatrix(dim)
    for i in range(dim):
        for j in range(i):
            R[i, j] = 0.5 * (1.0 + i) / dim
    distribution = NormalCopula(R)
    size = 10000
    sample = distribution.getSample(size)
    factory = NormalCopulaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", distribution)
    print("Estimated distribution=", estimatedDistribution)

    # non-regression for #572
    estimated_dist = NormalCopulaFactory().build(distribution.getSample(10))
    mydist = ComposedDistribution(
        DistributionCollection(dim, Normal()), estimated_dist)
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedNormalCopula = factory.buildAsNormalCopula(sample)
    print("NormalCopula          =", distribution)
    print("Estimated normalCopula=", estimatedNormalCopula)
    estimatedNormalCopula = factory.buildAsNormalCopula()
    print("Default normalCopula=", estimatedNormalCopula)

except:
    import sys
    print("t_NormalCopulaFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
