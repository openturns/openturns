#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = MeixnerDistribution(1.5, 0.5, 2.5, -0.5)
    size = 10000
    sample = distribution.getSample(size)
    factory = MeixnerDistributionFactory()
    covariance = CovarianceMatrix()
    # Distribution estimatedDistribution(factory.build(sample, covariance))
    estimatedDistribution = factory.build(sample)
    print("Distribution          =", distribution)
    print("Estimated distribution=", estimatedDistribution)
    # print "Covariance=", covariance
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameters())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution(sample)
    print("MeixnerDistribution          =", distribution)
    print("Estimated meixnerDistribution=", estimatedMeixnerDistribution)
    estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution()
    print("Default meixnerDistribution=", estimatedMeixnerDistribution)
    estimatedMeixnerDistribution = factory.buildAsMeixnerDistribution(
        distribution.getParameters())
    print("MeixnerDistribution from parameters=", estimatedMeixnerDistribution)

except:
    import sys
    print("t_MeixnerDistributionFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
