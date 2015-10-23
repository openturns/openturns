#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = GumbelCopula(1.5)
    size = 1000
    sample = distribution.getSample(size)
    factory = GumbelCopulaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedGumbelCopula = factory.buildAsGumbelCopula(sample)
    print("GumbelCopula          =", distribution)
    print("Estimated gumbelCopula=", estimatedGumbelCopula)
    estimatedGumbelCopula = factory.buildAsGumbelCopula()
    print("Default gumbelCopula=", estimatedGumbelCopula)
    estimatedGumbelCopula = factory.buildAsGumbelCopula(
        distribution.getParameter())
    print("GumbelCopula from parameters=", estimatedGumbelCopula)

except:
    import sys
    print("t_GumbelCopulaFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
