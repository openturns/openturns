#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = WeibullMax(1.0, 2.5, -1.0)
    size = 10000
    sample = distribution.getSample(size)
    factory = WeibullMaxFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedWeibull = factory.buildAsWeibullMax(sample)
    print("Estimated weibull=", estimatedWeibull)
    estimatedWeibull = factory.buildAsWeibullMax()
    print("Default weibull=", estimatedWeibull)
    estimatedWeibull = factory.buildAsWeibullMax(
        distribution.getParameter())
    print("WeibullMax from parameters=", estimatedWeibull)
    sample = [[0.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))
    sample = [[1.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_WeibullFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
