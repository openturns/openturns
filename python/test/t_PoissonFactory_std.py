#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Poisson(2.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = PoissonFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedPoisson = factory.buildAsPoisson(sample)
    print("Poisson          =", distribution)
    print("Estimated poisson=", estimatedPoisson)
    estimatedPoisson = factory.buildAsPoisson()
    print("Default poisson=", estimatedPoisson)
    estimatedPoisson = factory.buildAsPoisson(
        distribution.getParameter())
    print("Poisson from parameters=", estimatedPoisson)

except:
    import sys
    print("t_PoissonFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
