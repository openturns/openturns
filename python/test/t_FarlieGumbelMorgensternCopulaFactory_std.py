#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = FarlieGumbelMorgensternCopula(0.7)
    size = 1000
    sample = distribution.getSample(size)
    factory = FarlieGumbelMorgensternCopulaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedFarlieGumbelMorgensternCopula = factory.buildAsFarlieGumbelMorgensternCopula(
        sample)
    print("FarlieGumbelMorgensternCopula          =", distribution)
    print("Estimated farlieGumbelMorgensternCopula=",
          estimatedFarlieGumbelMorgensternCopula)
    estimatedFarlieGumbelMorgensternCopula = factory.buildAsFarlieGumbelMorgensternCopula(
    )
    print("Default farlieGumbelMorgensternCopula=",
          estimatedFarlieGumbelMorgensternCopula)
    estimatedFarlieGumbelMorgensternCopula = factory.buildAsFarlieGumbelMorgensternCopula(
        distribution.getParameter())
    print("FarlieGumbelMorgensternCopula from parameters=",
          estimatedFarlieGumbelMorgensternCopula)

except:
    import sys
    print("t_FarlieGumbelMorgensternFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
