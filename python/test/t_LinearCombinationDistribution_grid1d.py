#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.ResourceMap.SetAsUnsignedInteger(
    "LinearCombinationDistribution-DefaultMaxSize", 4000000
)

# Defining LinearCombinationDistribution
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
distribution = ot.LinearCombinationDistribution(coll, weights)
referenceDistribution = ot.Gamma(3.0, 1.0)
# Compute PDF on regular grid and compare with the reference Gamma PDF
N = 256
points = ot.Indices(1, N)
mean = distribution.getMean()
sigma = distribution.getStandardDeviation()
xMin = mean - 3.9 * sigma
xMax = mean + 3.9 * sigma
grid = ot.Sample()
result, grid = distribution.computePDF(xMin, xMax, points)
for i in range(grid.getSize()):
    ref = referenceDistribution.computePDF(grid[i])
    ott.assert_almost_equal(
        result[i, 0],
        ref,
        1e-4,
        1e-8,
        "grid pdf at x=%.6g" % grid[i][0],
    )
