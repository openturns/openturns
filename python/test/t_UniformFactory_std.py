#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Uniform(1.0, 2.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = UniformFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedUniform = factory.buildAsUniform(sample)
    print("Uniform          =", distribution)
    print("Estimated uniform=", estimatedUniform)
    estimatedUniform = factory.buildAsUniform()
    print("Default uniform=", estimatedUniform)
    estimatedUniform = factory.buildAsUniform(
        distribution.getParameter())
    print("Uniform from parameters=", estimatedUniform)
    sample = [[0.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))
    sample = [[1.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_UniformFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
