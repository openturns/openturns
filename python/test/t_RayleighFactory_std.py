#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Rayleigh(2.5, -1.0)
    size = 10000
    sample = distribution.getSample(size)
    factory = RayleighFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedRayleigh = factory.buildAsRayleigh(sample)
    print("Rayleigh          =", distribution)
    print("Estimated rayleigh=", estimatedRayleigh)
    estimatedRayleigh = factory.buildAsRayleigh()
    print("Default rayleigh=", estimatedRayleigh)
    estimatedRayleigh = factory.buildAsRayleigh(
        distribution.getParameter())
    print("Rayleigh from parameters=", estimatedRayleigh)
    sample = [[0.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))
    sample = [[1.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_RayleighFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
