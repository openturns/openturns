#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.ResourceMap.SetAsUnsignedInteger(
    "LinearCombinationDistribution-DefaultMaxSize", 4000000
)


# 2D test using FFT
# Collection of distributions
coll = ot.DistributionCollection(0)
coll.add(ot.Uniform(0, 1))
coll.add(ot.Uniform(0, 1))
coll.add(ot.Uniform(0, 1))
# Set weights
weights = ot.Matrix([[1.0, -2.0, 1.0], [1.0, 1.0, -3.0]])
# Build the LinearCombinationDistribution
dist_2D = ot.LinearCombinationDistribution(coll, weights)
# Check the moments against the exact values
ott.assert_almost_equal(dist_2D.getMean(), [0.0, -0.5], 1e-12, 1e-12, "mean")
covarianceRef = ot.CovarianceMatrix(2)
covarianceRef[0, 0] = (1.0 + 4.0 + 1.0) / 12.0
covarianceRef[1, 1] = (1.0 + 1.0 + 9.0) / 12.0
covarianceRef[1, 0] = (1.0 - 2.0 - 3.0) / 12.0
ott.assert_almost_equal(
    dist_2D.getCovariance(), covarianceRef, 1e-12, 1e-12, "covariance"
)
# Compute the PDF on a regular grid and cross-check it with the pointwise PDF
N = 16
points = ot.Indices(2, N)
mean = dist_2D.getMean()
sigma = dist_2D.getStandardDeviation()
xMin = mean - 2.9 * sigma
xMax = mean + 2.9 * sigma
grid = ot.Sample()
result, grid = dist_2D.computePDF(xMin, xMax, points)
for i in range(grid.getSize()):
    # The grid covers points outside of the support, where the pointwise
    # evaluation returns zero while the FFT introduces small ringing values
    if not dist_2D.getRange().contains(grid[i]):
        continue
    pdf = dist_2D.computePDF(grid[i])
    # The grid based evaluation is only intended for drawing purpose, so
    # only a coarse agreement can be expected
    ott.assert_almost_equal(
        result[i, 0],
        pdf,
        1e-2,
        3e-3,
        "grid vs pointwise pdf at (%.6g, %.6g)" % (grid[i][0], grid[i][1]),
    )

# Defining new case, involving Normal distributions: compare with the exact
# Gaussian density
collection = ot.DistributionCollection(0)
collection.add(ot.Normal(2.0, 3.0))
collection.add(ot.Normal(1.0, 4.0))
weights = ot.Matrix([[4.0, 1.0], [2.0, 1.4]])
# Build the LinearCombinationDistribution
distribution2D = ot.LinearCombinationDistribution(collection, weights)
mu = distribution2D.getMean()
sigma = distribution2D.getStandardDeviation()
covariance = distribution2D.getCovariance()
reference = ot.Normal(mu, covariance)
ott.assert_almost_equal(mu, [9.0, 5.4], 1e-12, 1e-12, "mean")
xMin = mu - 2.9 * sigma
xMax = mu + 2.9 * sigma
result, grid = distribution2D.computePDF(xMin, xMax, points)
for i in range(grid.getSize()):
    ref = reference.computePDF(grid[i])
    # Very small values are not very accurate, skip them
    if ref < 1.0e-12:
        continue
    ott.assert_almost_equal(
        result[i, 0],
        ref,
        1e-5,
        1e-12,
        "gaussian grid pdf at (%.6g, %.6g)" % (grid[i][0], grid[i][1]),
    )
