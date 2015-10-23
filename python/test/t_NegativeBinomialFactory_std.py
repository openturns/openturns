#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = NegativeBinomial(15, 0.7)
    size = 10000
    sample = distribution.getSample(size)
    factory = NegativeBinomialFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedNegativeBinomial = factory.buildAsNegativeBinomial(sample)
    print("NegativeBinomial          =", distribution)
    print("Estimated negativeBinomial=", estimatedNegativeBinomial)
    estimatedNegativeBinomial = factory.buildAsNegativeBinomial()
    print("Default negativeBinomial=", estimatedNegativeBinomial)
    estimatedNegativeBinomial = factory.buildAsNegativeBinomial(
        distribution.getParameter())
    print("NegativeBinomial from parameters=", estimatedNegativeBinomial)

except:
    import sys
    print("t_NegativeBinomialFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
