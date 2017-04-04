#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)
PlatformInfo.SetNumericalPrecision(6)
ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000)

try:

    # Defining RandomMixture
    weights = Point(0)
    coll = DistributionCollection(0)
    coll.add(Gamma(0.5, 1.0))
    weights.add(1.0)
    coll.add(Gamma(0.5, 1.0))
    weights.add(1.0)
    coll.add(Gamma(0.5, 1.0))
    weights.add(1.0)
    coll.add(Gamma(0.5, 1.0))
    weights.add(1.0)
    coll.add(Gamma(1.0, 1.0))
    weights.add(1.0)
    distribution = RandomMixture(coll, weights)
    referenceDistribution = Gamma(3.0, 1.0)
    # Compute PDF on regular grid
    N = 256
    points = Indices(1, N)
    mean = distribution.getMean()
    sigma = distribution.getStandardDeviation()
    xMin = mean - 3.9 * sigma
    xMax = mean + 3.9 * sigma
    grid = Sample()
    print("distribution = ", repr(distribution))
    print("range = ", distribution.getRange())
    print("mean = ",  distribution.getMean())
    print("cov = ", distribution.getCovariance())
    print("sigma = ", distribution.getStandardDeviation())
    print("xMin = ", xMin)
    print("xMax = ", xMax)
    result, grid = distribution.computePDF(xMin, xMax, points)
    print("x;PDF;reference")
    for i in range(grid.getSize()):
        ref = referenceDistribution.computePDF(grid[i])
        print("%.6g;%.6g;%.6g" % (grid[i][0], result[i][0], ref))

except:
    import sys
    print("t_RandomMixture_grid1d.py", sys.exc_info()[0], sys.exc_info()[1])
