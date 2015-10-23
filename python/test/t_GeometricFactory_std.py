#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Geometric(0.7)
    size = 10000
    sample = distribution.getSample(size)
    factory = GeometricFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedGeometric = factory.buildAsGeometric(sample)
    print("Geometric          =", distribution)
    print("Estimated geometric=", estimatedGeometric)
    estimatedGeometric = factory.buildAsGeometric()
    print("Default geometric=", estimatedGeometric)
    estimatedGeometric = factory.buildAsGeometric(
        distribution.getParameter())
    print("Geometric from parameters=", estimatedGeometric)

except:
    import sys
    print("t_GeometricFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
