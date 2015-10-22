#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Gumbel(2.0, 2.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = GumbelFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameters())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedGumbel = factory.buildAsGumbel(sample)
    print("Gumbel          =", distribution)
    print("Estimated gumbel=", estimatedGumbel)
    estimatedGumbel = factory.buildAsGumbel()
    print("Default gumbel=", estimatedGumbel)
    estimatedGumbel = factory.buildAsGumbel(
        distribution.getParameters())
    print("Gumbel from parameters=", estimatedGumbel)

except:
    import sys
    print("t_GumbelFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
