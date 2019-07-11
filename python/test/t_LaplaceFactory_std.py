#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Laplace(-1.3, 2.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = LaplaceFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedLaplace = factory.buildAsLaplace(sample)
    print("Laplace          =", distribution)
    print("Estimated laplace=", estimatedLaplace)
    estimatedLaplace = factory.buildAsLaplace()
    print("Default laplace=", estimatedLaplace)
    estimatedLaplace = factory.buildAsLaplace(
        distribution.getParameter())
    print("Laplace from parameters=", estimatedLaplace)

except:
    import sys
    print("t_LaplaceFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
