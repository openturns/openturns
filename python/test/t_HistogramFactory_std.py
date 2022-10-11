#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ll = [1.0, 0.7, 1.2, 0.9]
h = [0.5, 1.5, 3.5, 2.5]
distribution = ot.Histogram(-1.5, ll, h)
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
first = -2.0
width = ot.Point(5, 1.0)
estimatedHistogram = factory.buildAsHistogram(sample, first, width)
print("Estimated histogram=", estimatedHistogram)
estimatedHistogram = factory.buildAsHistogram()
print("Default histogram=", estimatedHistogram)

sample = ot.ChiSquare(0.0120637).getSample(32)
for useQuantile in [True, False]:
    print(
        "useQuantile=",
        useQuantile,
        ot.Point(1, ot.HistogramFactory().computeBandwidth(sample, useQuantile)),
    )

# from quantiles
ref_dist = ot.Normal()
lowerBound = -3.0
N = 10
probabilities = [(i + 1) / N for i in range(N)]
quantiles = [ref_dist.computeQuantile(pi)[0] for pi in probabilities]
inf_distribution = ot.HistogramFactory().buildFromQuantiles(
    lowerBound, probabilities, quantiles
)
inf_quantiles = [inf_distribution.computeQuantile(pi)[0] for pi in probabilities]
ott.assert_almost_equal(inf_quantiles, quantiles)
