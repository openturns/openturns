#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object: mean = I_2, covariance = I_3
mean = ot.SymmetricMatrix(2)
mean[0, 0] = 1.0
mean[1, 1] = 1.0
sigma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
distribution = otexp.RiemannianGaussian(mean, sigma)
print("Distribution ", distribution)

# Is this distribution continuous ?
assert distribution.isContinuous()

# Dimension: n (n + 1) / 2 for n = 2
assert distribution.getDimension() == 3

# The normalized log-density at the mean is -0.5 d log(2 pi) - 0.5 log|Sigma|
d = distribution.getDimension()
ott.assert_almost_equal(
    distribution.computeLogPDF([1.0, 0.0, 1.0]),
    -0.5 * d * math.log(2.0 * math.pi) - 0.5 * math.log(1.0),
    1e-12,
    0.0,
)

# PDF times the Chernov-ish volume factor drawn from exp of gaussian:
# the realization must be a symmetric positive definite matrix
ot.RandomGenerator.SetSeed(0)
realization = distribution.getRealization()
assert realization.getDimension() == 3
a = realization[0]
b = realization[1]
c = realization[2]
assert a > 0.0
assert a * c - b * b > 0.0

# Sample stays on the manifold of symmetric positive definite matrices
ot.RandomGenerator.SetSeed(0)
sample = distribution.getSample(1000)
for i in range(10):
    a = sample[i, 0]
    b = sample[i, 1]
    c = sample[i, 2]
    assert a > 0.0
    assert a * c - b * b > 0.0

# The sample mean is a symmetric positive definite matrix. For the identity
# mean the two diagonal entries are equal by symmetry and the off-diagonal
# entry vanishes.
sample_mean = sample.computeMean()
assert sample_mean[0] > 0.0
assert sample_mean[0] * sample_mean[2] - sample_mean[1] ** 2 > 0.0
ott.assert_almost_equal(sample_mean[0], sample_mean[2], 0.1, 0.0)
ott.assert_almost_equal(sample_mean[1], 0.0, 0.0, 0.05)

# Mean accessors: the flattened mean matches the upper-triangle of the matrix
mean_matrix = distribution.getMeanMatrix()
assert mean_matrix.getDimension() == 2
point_mean = distribution.getMean()
expected_point = [mean_matrix[0, 0], mean_matrix[0, 1],
                  mean_matrix[1, 1]]
ott.assert_almost_equal(point_mean, expected_point, 1e-12, 0.0)

# setMean / getMeanMatrix round-trip
mean2 = ot.SymmetricMatrix([[2.0, 0.3], [0.3, 3.0]])
dist_setter = otexp.RiemannianGaussian(mean, sigma)
dist_setter.setMean(mean2)
m2 = dist_setter.getMeanMatrix()
for i in range(2):
    for j in range(i, 2):
        ott.assert_almost_equal(m2[i, j], mean2[i, j], 1e-12, 0.0)

# setSigma / getSigma round-trip
sigma2 = ot.SquareMatrix([[2.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 0.5]])
dist_setter.setSigma(sigma2)
s2 = dist_setter.getSigma()
for i in range(3):
    for j in range(3):
        ott.assert_almost_equal(s2[i, j], sigma2[i, j], 1e-12, 0.0)
ott.assert_almost_equal(
    dist_setter.getEpsilon(),
    ot.ResourceMap.GetAsScalar("RiemannianGaussian-PositiveDefiniteThreshold"),
)

# Invalid parameters
non_pd_mean = ot.SymmetricMatrix([[1.0, 2.0], [2.0, 1.0]])
with ott.assert_raises(TypeError):
    otexp.RiemannianGaussian(non_pd_mean, sigma)
with ott.assert_raises(TypeError):
    otexp.RiemannianGaussian(mean, ot.SquareMatrix(4))

# Parameter round-trip: mean upper-triangle (3) + sigma row-major (9)
param = distribution.getParameter()
assert param.getDimension() == 3 + 9
new_dist = otexp.RiemannianGaussian()
new_dist.setParameter(param)
ott.assert_almost_equal(new_dist.getParameter(), param, 1e-12, 0.0)
with ott.assert_raises(TypeError):
    distribution.setParameter([1.0] * 5)

# getParameterDescription
desc = distribution.getParameterDescription()
assert desc.getSize() == 3 + 9

# computePDF with wrong dimension
with ott.assert_raises(TypeError):
    distribution.computePDF([1.0, 0.0])
with ott.assert_raises(TypeError):
    distribution.computeLogPDF([1.0, 0.0, 1.0, 0.0])

# Default constructor
default = otexp.RiemannianGaussian()
assert default.getDimension() == 3
mean_default = default.getMeanMatrix()
ott.assert_almost_equal(mean_default[0, 0], 1.0, 1e-12, 0.0)
ott.assert_almost_equal(mean_default[1, 1], 1.0, 1e-12, 0.0)

# Clone via Distribution wrapper
dist = ot.Distribution(distribution)
assert dist.getImplementation().getClassName() == "RiemannianGaussian"

# __repr__ and __str__
assert "RiemannianGaussian" in repr(distribution)
assert "RiemannianGaussian" in str(distribution)
