#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Skellam(10.0, 5.0)
    size = 10000
    sample = distribution.getSample(size)
    factory = SkellamFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedSkellam = factory.buildAsSkellam(sample)
    print("Skellam          =", distribution)
    print("Estimated skellam=", estimatedSkellam)
    estimatedSkellam = factory.buildAsSkellam()
    print("Default skellam=", estimatedSkellam)
    estimatedSkellam = factory.buildAsSkellam(
        distribution.getParameter())
    print("Skellam from parameters=", estimatedSkellam)

except:
    import sys
    print("t_SkellamFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
