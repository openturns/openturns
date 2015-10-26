#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = ClaytonCopula(1.5)
    size = 1000
    sample = distribution.getSample(size)
    factory = ClaytonCopulaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedClaytonCopula = factory.buildAsClaytonCopula(sample)
    print("ClaytonCopula          =", distribution)
    print("Estimated claytonCopula=", estimatedClaytonCopula)
    estimatedClaytonCopula = factory.buildAsClaytonCopula()
    print("Default claytonCopula=", estimatedClaytonCopula)
    estimatedClaytonCopula = factory.buildAsClaytonCopula(
        distribution.getParameter())
    print("ClaytonCopula from parameters=", estimatedClaytonCopula)

except:
    import sys
    print("t_ClaytonCopulaFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
