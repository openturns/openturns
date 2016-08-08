#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Gamma(0.2, 1.0, 1.0)
    size = 10000
    sample = distribution.getSample(size)
    factory = GammaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    distribution = Gamma(2.3, 1.0, 1.0)
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    distribution = Gamma(2.3, 1.0, 1.0)
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("Distribution          =", distribution)
    print("Estimated distribution=", estimatedDistribution)
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedGamma = factory.buildAsGamma(sample)
    print("Gamma          =", distribution)
    print("Estimated gamma=", estimatedGamma)
    estimatedGamma = factory.buildAsGamma()
    print("Default gamma=", estimatedGamma)
    estimatedGamma = factory.buildAsGamma(
        distribution.getParameter())
    print("Gamma from parameters=", estimatedGamma)
    sample = [[0.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))
    sample = [[1.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_GammaFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
