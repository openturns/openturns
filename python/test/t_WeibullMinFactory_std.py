#! /usr/bin/env python

from openturns import *
import openturns.testing as ott

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = WeibullMin(1.0, 2.5, -1.0)
    size = 10000
    sample = distribution.getSample(size)
    factory = WeibullMinFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedWeibull = factory.buildAsWeibullMin(sample)
    print("Estimated weibull=", estimatedWeibull)
    estimatedWeibull = factory.buildAsWeibullMin()
    print("Default weibull=", estimatedWeibull)
    estimatedWeibull = factory.buildAsWeibullMin(
        distribution.getParameter())
    print("WeibullMin from parameters=", estimatedWeibull)

except:
    import sys
    print("t_WeibullFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
