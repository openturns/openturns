#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = LogNormal(1.5, 2.5, -1.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = LogNormalFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameters())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedLogNormal = factory.buildAsLogNormal(sample)
    print("LogNormal          =", distribution)
    print("Estimated logNormal=", estimatedLogNormal)
    estimatedLogNormal = factory.buildAsLogNormal()
    print("Default logNormal=", estimatedLogNormal)
    estimatedLogNormal = factory.buildAsLogNormal(
        distribution.getParameters())
    print("LogNormal from parameters=", estimatedLogNormal)

except:
    import sys
    print("t_LogNormalFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
