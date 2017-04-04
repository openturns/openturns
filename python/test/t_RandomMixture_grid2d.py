#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)
PlatformInfo.SetNumericalPrecision(6)
ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000)

try:
    # 2D test using FFT
    # Collection of distributions
    coll = DistributionCollection(0)
    coll.add(Uniform(0, 1))
    coll.add(Uniform(0, 1))
    coll.add(Uniform(0, 1))
    # Set weights
    weights = Matrix([[1.0, -2.0, 1.0], [1.0, 1.0, -3.0]])
    # Build the RandomMixture
    dist_2D = RandomMixture(coll, weights)
    # Defining RandomMixture
    N = 16
    points = Indices(2, N)
    mean = dist_2D.getMean()
    sigma = dist_2D.getStandardDeviation()
    xMin = mean - 2.9 * sigma
    xMax = mean + 2.9 * sigma
    grid = Sample()
    print("distribution = ", repr(dist_2D))
    print("range = ", dist_2D.getRange())
    print("mean = ",  dist_2D.getMean())
    print("cov = ", dist_2D.getCovariance())
    print("sigma = ", dist_2D.getStandardDeviation())
    print("xMin = ", xMin)
    print("xMax = ", xMax)
    result, grid = dist_2D.computePDF(xMin, xMax, points)
    for i in range(grid.getSize()):
        print("%.6g;%.6g;%.6g" % (grid[i][0], grid[i][1], result[i][0]))
    # Defining new case, involving Normal distributions
    collection = DistributionCollection(0)
    collection.add(Normal(2.0, 3.0))
    collection.add(Normal(1.0, 4.0))
    weights = Matrix([[4.0, 1.0], [2.0, 1.4]])
    # Build the RandomMixture
    distribution2D = RandomMixture(collection, weights)
    print("distribution = ", repr(distribution2D))
    print("range = ", distribution2D.getRange())
    print("mean = ",  distribution2D.getMean())
    print("cov = ", distribution2D.getCovariance())
    print("sigma = ", distribution2D.getStandardDeviation())
    xMin = distribution2D.getMean() - 2.9 * \
        distribution2D.getStandardDeviation()
    xMax = distribution2D.getMean() + 2.9 * \
        distribution2D.getStandardDeviation()
    result, grid = distribution2D.computePDF(xMin, xMax, points)
    print("x;y;PDF")
    for i in range(grid.getSize()):
        print("%.6g;%.6g;%.6g" % (grid[i][0], grid[i][1], result[i][0]))

except:
    import sys
    print("t_RandomMixture_grid2d.py", sys.exc_info()[0], sys.exc_info()[1])
