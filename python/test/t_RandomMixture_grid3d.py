#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(6)
ot.ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000)


# 1) Analytical test ==> No FFT
coll = ot.DistributionCollection(3)
coll[0] = ot.Normal(0.0, 1.0)
coll[1] = ot.Uniform(2.0, 5.0)
coll[2] = ot.Uniform(2.0, 5.0)
weights = ot.Matrix([[1.0, 2.0, 4.0], [3.0, 4.0, 5.0], [6.0, 0.0, 1.0]])
distribution = ot.RandomMixture(coll, weights)
print("distribution=", repr(distribution))
print("range = ", distribution.getRange())
print("mean = ", distribution.getMean())
print("cov = ", distribution.getCovariance())
print("sigma = ", distribution.getStandardDeviation())
N = 4
points = ot.Indices(3, N)
mean = distribution.getMean()
sigma = distribution.getStandardDeviation()
xMin = mean - 2.9 * sigma
xMax = mean + 2.9 * sigma
grid = ot.Sample()
result, grid = distribution.computePDF(xMin, xMax, points)
print("x;y;z;PDF")
for i in range(grid.getSize()):
    print("%.6g;%.6g;%.6g;%.6g" % (grid[i][0], grid[i][1], grid[i][2], result[i][0]))

# 2) 3D test using FFT
collection = ot.DistributionCollection(0)
mixture = ot.Mixture([ot.Normal(2.0, 1.0), ot.Normal(-2.0, 1.0)])
collection3D = ot.DistributionCollection(0)
collection3D.add(ot.Normal(0.0, 1.0))
collection3D.add(mixture)
collection3D.add(ot.Uniform(0, 1))
collection3D.add(ot.Uniform(0, 1))
# / Set weights
weights = ot.Matrix(
    [[1.0, -0.05, 1.0, -0.5], [0.5, 1.0, -0.05, 0.3], [-0.5, -0.1, 1.2, -0.8]]
)
# Defining RandomMixture
dist_3D = ot.RandomMixture(collection3D, weights)
mean = dist_3D.getMean()
sigma = dist_3D.getStandardDeviation()
print("distribution = ", repr(dist_3D))
print("range = ", dist_3D.getRange())
print("mean = ", dist_3D.getMean())
print("cov = ", dist_3D.getCovariance())
print("sigma = ", dist_3D.getStandardDeviation())
xMin = mean - 2.9 * sigma
xMax = mean + 2.9 * sigma
print("xMin = ", xMin)
print("xMax = ", xMax)
result, grid = dist_3D.computePDF(xMin, xMax, points)
for i in range(grid.getSize()):
    print("%.6g;%.6g;%.6g;%.6g" % (grid[i][0], grid[i][1], grid[i][2], result[i][0]))
