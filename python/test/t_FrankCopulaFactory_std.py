#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = FrankCopula(1.5)
    size = 1000
    sample = distribution.getSample(size)
    factory = FrankCopulaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedFrankCopula = factory.buildAsFrankCopula(sample)
    print("FrankCopula          =", distribution)
    print("Estimated frankCopula=", estimatedFrankCopula)
    estimatedFrankCopula = factory.buildAsFrankCopula()
    print("Default frankCopula=", estimatedFrankCopula)
    estimatedFrankCopula = factory.buildAsFrankCopula(
        distribution.getParameter())
    print("FrankCopula from parameters=", estimatedFrankCopula)

except:
    import sys
    print("t_FrankFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
