#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Beta(0.2, 0.4, -1.0, 2.0)
    size = 10000
    sample = distribution.getSample(size)
    factory = BetaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    distribution = Beta(0.5, 0.8, -1.0, 2.0)
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    distribution = Beta(0.5, 1.8, -1.0, 2.0)
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    distribution = Beta(1.5, 2.8, -1.0, 2.0)
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedBeta = factory.buildAsBeta(sample)
    print("Beta          =", distribution)
    print("Estimated Beta=", estimatedBeta)
    estimatedBeta = factory.buildAsBeta()
    print("Default Beta=", estimatedBeta)
    estimatedBeta = factory.buildAsBeta(distribution.getParameter())
    print("Beta from parameters=", estimatedBeta)
    sample = [[0.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))
    sample = [[1.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_BetaFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
