#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)
PlatformInfo.SetNumericalPrecision(6)
ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000)

try:
    # 1) Analytical test ==> No FFT
    coll = DistributionCollection(3)
    coll[0] = Normal(0.0, 1.0)
    coll[1] = Uniform(2.0, 5.0)
    coll[2] = Uniform(2.0, 5.0)
    weights = Matrix([[1.0, 2.0, 4.0], [3.0, 4.0, 5.0], [6.0, 0.0, 1.0]])
    distribution = RandomMixture(coll, weights)
    print("distribution=", repr(distribution))
    print("range = ", distribution.getRange())
    print("mean = ",  distribution.getMean())
    print("cov = ", distribution.getCovariance())
    print("sigma = ", distribution.getStandardDeviation())
    N = 4
    points = Indices(3, N)
    mean = distribution.getMean()
    sigma = distribution.getStandardDeviation()
    xMin = mean - 2.9 * sigma
    xMax = mean + 2.9 * sigma
    grid = Sample()
    result, grid = distribution.computePDF(xMin, xMax, points)
    print("x;y;z;PDF")
    for i in range(grid.getSize()):
        print("%.6g;%.6g;%.6g;%.6g" %
              (grid[i][0], grid[i][1], grid[i][2], result[i][0]))

    # 2) 3D test using FFT
    collection = DistributionCollection(0)
    mixture = Mixture([Normal(2.0, 1.0), Normal(-2.0, 1.0)])
    collection3D = DistributionCollection(0)
    collection3D.add(Normal(0.0, 1.0))
    collection3D.add(mixture)
    collection3D.add(Uniform(0, 1))
    collection3D.add(Uniform(0, 1))
    #/ Set weights
    weights = Matrix(
        [[1.0, -0.05, 1.0, -0.5], [0.5, 1.0, -0.05, 0.3], [-0.5, -0.1, 1.2, -0.8]])
    # Defining RandomMixture
    dist_3D = RandomMixture(collection3D, weights)
    mean = dist_3D.getMean()
    sigma = dist_3D.getStandardDeviation()
    print("distribution = ", repr(dist_3D))
    print("range = ", dist_3D.getRange())
    print("mean = ",  dist_3D.getMean())
    print("cov = ", dist_3D.getCovariance())
    print("sigma = ", dist_3D.getStandardDeviation())
    xMin = mean - 2.9 * sigma
    xMax = mean + 2.9 * sigma
    print("xMin = ", xMin)
    print("xMax = ", xMax)
    result, grid = dist_3D.computePDF(xMin, xMax, points)
    for i in range(grid.getSize()):
        print("%.6g;%.6g;%.6g;%.6g" %
              (grid[i][0], grid[i][1], grid[i][2], result[i][0]))

except:
    import sys
    print("t_RandomMixture_grid3d.py", sys.exc_info()[0], sys.exc_info()[1])
