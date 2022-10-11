#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(6)
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000)

# Defining RandomMixture
weights = ot.Point(0)
coll = ot.DistributionCollection(0)
coll.add(ot.Gamma(0.5, 1.0))
weights.add(1.0)
coll.add(ot.Gamma(0.5, 1.0))
weights.add(1.0)
coll.add(ot.Gamma(0.5, 1.0))
weights.add(1.0)
coll.add(ot.Gamma(0.5, 1.0))
weights.add(1.0)
coll.add(ot.Gamma(1.0, 1.0))
weights.add(1.0)
distribution = ot.RandomMixture(coll, weights)
referenceDistribution = ot.Gamma(3.0, 1.0)
# Compute PDF on regular grid
N = 256
points = ot.Indices(1, N)
mean = distribution.getMean()
sigma = distribution.getStandardDeviation()
xMin = mean - 3.9 * sigma
xMax = mean + 3.9 * sigma
grid = ot.Sample()
print("distribution = ", repr(distribution))
print("range = ", distribution.getRange())
print("mean = ", distribution.getMean())
print("cov = ", distribution.getCovariance())
print("sigma = ", distribution.getStandardDeviation())
print("xMin = ", xMin)
print("xMax = ", xMax)
result, grid = distribution.computePDF(xMin, xMax, points)
print("x;PDF;reference")
for i in range(grid.getSize()):
    ref = referenceDistribution.computePDF(grid[i])
    print("%.6g;%.6g;%.6g" % (grid[i][0], result[i][0], ref))
