#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = PlackettCopula(1.5)
    size = 1000
    sample = distribution.getSample(size)
    factory = PlackettCopulaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedPlackettCopula = factory.buildAsPlackettCopula(sample)
    print("PlackettCopula          =", distribution)
    print("Estimated plackettCopula=", estimatedPlackettCopula)
    estimatedPlackettCopula = factory.buildAsPlackettCopula()
    print("Default plackettCopula=", estimatedPlackettCopula)
    estimatedPlackettCopula = factory.buildAsPlackettCopula(
        distribution.getParameter())
    print("PlackettCopula from parameters=", estimatedPlackettCopula)

except:
    import sys
    print("t_PlackettFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
