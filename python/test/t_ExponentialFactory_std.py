#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Exponential(2.5, -1.3)
    size = 10000
    sample = distribution.getSample(size)
    factory = ExponentialFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedExponential = factory.buildAsExponential(sample)
    print("Exponential          =", distribution)
    print("Estimated exponential=", estimatedExponential)
    estimatedExponential = factory.buildAsExponential()
    print("Default exponential=", estimatedExponential)
    estimatedExponential = factory.buildAsExponential(
        distribution.getParameter())
    print("Exponential from parameters=", estimatedExponential)
    sample = [[0.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))
    sample = [[1.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_ExponentialFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
