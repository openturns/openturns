#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

try:
    l = [1.0, 0.7, 1.2, 0.9]
    h = [0.5, 1.5, 3.5, 2.5]
    distribution = ot.Histogram(-1.5, l, h)
    size = 10000
    sample = distribution.getSample(size)
    factory = ot.HistogramFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedHistogram = factory.buildAsHistogram(sample)
    print("Histogram          =", distribution)
    print("Estimated histogram=", estimatedHistogram)
    estimatedHistogram = factory.buildAsHistogram(sample, 0.1)
    print("Histogram          =", distribution)
    print("Estimated histogram=", estimatedHistogram)
    estimatedHistogram = factory.buildAsHistogram(sample, 15)
    print("Histogram          =", distribution)
    print("Estimated histogram=", estimatedHistogram)
    first = -2.
    width = ot.Point(5, 1.)
    estimatedHistogram = factory.buildAsHistogram(sample, first, width)
    print("Estimated histogram=", estimatedHistogram)
    estimatedHistogram = factory.buildAsHistogram()
    print("Default histogram=", estimatedHistogram)
    sample = [[0.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))
    sample = [[1.0]] * size
    estimatedDistribution = factory.build(sample)
    print("Estimated distribution=", repr(estimatedDistribution))

    sample = ot.ChiSquare(0.0120637).getSample(32)
    for useQuantile in [True, False]:
        print('silverman useQuantile=', useQuantile, ot.Point(
            1, ot.HistogramFactory().computeSilvermanBandwidth(sample, useQuantile)))
except:
    import sys
    print("t_HistogramFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
