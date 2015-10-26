#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Rice(0.5, 1.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = RiceFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedRice = factory.buildAsRice(sample)
    print("Rice          =", distribution)
    print("Estimated rice=", estimatedRice)
    estimatedRice = factory.buildAsRice(distribution.getParameter())
    print("Rice from parameters=", estimatedRice)
    estimatedRice = factory.buildAsRice()
    print("Default rice=", estimatedRice)

except:
    import sys
    print("t_RiceFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
