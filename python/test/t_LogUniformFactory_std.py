#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = LogUniform(1.0, 2.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = LogUniformFactory()
    estimatedDistribution = factory.build(sample)
    print("Distribution          =", distribution)
    print("Estimated distribution=", estimatedDistribution)
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedLogUniform = factory.buildAsLogUniform(sample)
    print("LogUniform          =", distribution)
    print("Estimated logUniform=", estimatedLogUniform)
    estimatedLogUniform = factory.buildAsLogUniform()
    print("Default logUniform=", estimatedLogUniform)
    estimatedLogUniform = factory.buildAsLogUniform(
        distribution.getParameter())
    print("LogUniform from parameters=", estimatedLogUniform)
    sample = [[1.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_LogUniformFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
