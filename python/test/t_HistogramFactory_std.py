#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    collectionSize = 4
    collection = HistogramPairCollection(collectionSize)
    collection[0] = HistogramPair(1.0, 0.5)
    collection[1] = HistogramPair(0.7, 1.5)
    collection[2] = HistogramPair(1.2, 3.5)
    collection[3] = HistogramPair(0.9, 2.5)
    distribution = Histogram(-1.5, collection)
    size = 10000
    sample = distribution.getSample(size)
    factory = HistogramFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedHistogram = factory.buildAsHistogram(sample)
    print("Histogram          =", distribution)
    print("Estimated histogram=", estimatedHistogram)
    estimatedHistogram = factory.buildAsHistogram()
    print("Default histogram=", estimatedHistogram)
    sample = [[0.0]]*size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))
    sample = [[1.0]]*size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

except:
    import sys
    print("t_HistogramFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
