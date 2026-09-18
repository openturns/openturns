#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.ResourceMap.SetAsUnsignedInteger(
    "LinearCombinationDistribution-DefaultMaxSize", 4000000
)


# 1) Analytical test ==> No FFT
coll = ot.DistributionCollection(3)
coll[0] = ot.Normal(0.0, 1.0)
coll[1] = ot.Uniform(2.0, 5.0)
coll[2] = ot.Uniform(2.0, 5.0)
weights = ot.Matrix([[1.0, 2.0, 4.0], [3.0, 4.0, 5.0], [6.0, 0.0, 1.0]])
distribution = ot.LinearCombinationDistribution(coll, weights)
# Check the moments against the exact values
means = ot.Point([coll[k].getMean()[0] for k in range(3)])
muRef = weights * means
covarianceRef = ot.CovarianceMatrix(3)
for j in range(3):
    for i in range(j + 1):
        covarianceRef[i, j] = sum(
            weights[i, k] * weights[j, k] * coll[k].getCovariance()[0, 0]
            for k in range(3)
        )
ott.assert_almost_equal(distribution.getMean(), muRef, 1e-12, 1e-12, "mean")
ott.assert_almost_equal(
    distribution.getCovariance(), covarianceRef, 1e-12, 1e-12, "covariance"
)
N = 4
points = ot.Indices(3, N)
mean = distribution.getMean()
sigma = distribution.getStandardDeviation()
xMin = mean - 2.9 * sigma
xMax = mean + 2.9 * sigma
grid = ot.Sample()
result, grid = distribution.computePDF(xMin, xMax, points)
for i in range(grid.getSize()):
    pdf = distribution.computePDF(grid[i])
    ott.assert_almost_equal(
        result[i, 0],
        pdf,
        1e-10,
        1e-12,
        "analytical grid vs pointwise pdf at point %d" % i,
    )

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
# Defining LinearCombinationDistribution
dist_3D = ot.LinearCombinationDistribution(collection3D, weights)
mean = dist_3D.getMean()
sigma = dist_3D.getStandardDeviation()
xMin = mean - 2.9 * sigma
xMax = mean + 2.9 * sigma
result, grid = dist_3D.computePDF(xMin, xMax, points)
for i in range(grid.getSize()):
    # The pointwise evaluation of the PDF based on Poisson's summation formula
    # is way too costly in 3D: only the positivity of the gridded values and a
    # comparison with its bounded pointwise counterpart near the center of the
    # distribution are checked
    assert result[i, 0] >= 0.0, "negative pdf value"
# Rank the grid points by their distance to the mean
distances = []
for i in range(grid.getSize()):
    distance = sum((grid[i, j] - mean[j]) ** 2 for j in range(3))
    distances.append((distance, i))
distances.sort()
for distance, i in distances[:4]:
    pdf = dist_3D.computePDF(grid[i])
    ott.assert_almost_equal(
        result[i, 0],
        pdf,
        0.2,
        0.0,
        "fft grid vs pointwise pdf at point %d" % i,
    )
