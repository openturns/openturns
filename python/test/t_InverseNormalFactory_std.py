#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = InverseNormal(2.5, 1.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = InverseNormalFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedInverseNormal = factory.buildAsInverseNormal(sample)
    print("InverseNormal          =", distribution)
    print("Estimated inverseNormal=", estimatedInverseNormal)
    estimatedInverseNormal = factory.buildAsInverseNormal()
    print("Default inverseNormal=", estimatedInverseNormal)
    estimatedInverseNormal = factory.buildAsInverseNormal(
        distribution.getParameter())
    print("InverseNormal from parameters=", estimatedInverseNormal)

except:
    import sys
    print("t_InverseNormalFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
