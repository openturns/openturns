#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = ChiSquare(0.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = ChiSquareFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    distribution = ChiSquare(1.0)
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    distribution = ChiSquare(2.5)
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedChiSquare = factory.buildAsChiSquare(sample)
    print("ChiSquare          =", distribution)
    print("Estimated chiSquare=", estimatedChiSquare)
    estimatedChiSquare = factory.buildAsChiSquare()
    print("Default chiSquare=", estimatedChiSquare)
    estimatedChiSquare = factory.buildAsChiSquare(
        distribution.getParameter())
    print("ChiSquare from parameters=", estimatedChiSquare)

except:
    import sys
    print("t_ChiSquareFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
