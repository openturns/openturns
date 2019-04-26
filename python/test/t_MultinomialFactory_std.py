#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Multinomial(5, Point(3, 0.25))
    size = 10000
    sample = distribution.getSample(size)
    sample.exportToCSVFile("multinomial.csv")
    factory = MultinomialFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedMultinomial = factory.buildAsMultinomial(sample)
    print("Multinomial          =", distribution)
    print("Estimated multinomial=", estimatedMultinomial)
    estimatedMultinomial = factory.buildAsMultinomial()
    print("Default multinomial=", estimatedMultinomial)

except:
    import sys
    print("t_MultinomialFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
