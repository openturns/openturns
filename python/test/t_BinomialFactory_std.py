#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Binomial(15, 0.7)
    size = 10000
    sample = distribution.getSample(size)
    factory = BinomialFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", repr(estimatedDistribution))
    estimatedBinomial = factory.buildAsBinomial(sample)
    print("Binomial          =", distribution)
    print("Estimated binomial=", estimatedBinomial)
    estimatedBinomial = factory.buildAsBinomial()
    print("Default binomial=", estimatedBinomial)
    estimatedBinomial = factory.buildAsBinomial(
        distribution.getParameter())
    print("Binomial from parameters=", estimatedBinomial)

except:
    import sys
    print("t_BinomialFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
