#! /usr/bin/env python

import math
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
gamma_matrix = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
distribution = otexp.Kent(25.0, 0.5, gamma_matrix)
print("Distribution ", distribution)

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
assert oneRealization.getDimension() == 3
assert abs(oneRealization.norm() - 1.0) < 1e-15

# Define a point on the sphere
point = [1.0, 0.0, 0.0]

# Show PDF and log-PDF of point
LPDF = distribution.computeLogPDF(point)
PDF = distribution.computePDF(point)
assert PDF > 0.0
ott.assert_almost_equal(PDF, 24.9823, 5e-3, 0.0)
ott.assert_almost_equal(LPDF, 3.21817, 5e-4, 0.0)

# PDF at a point off the sphere should be 0
point_off = [0.5, 0.0, 0.0]
assert distribution.computePDF(point_off) == 0.0

# PDF at an orthogonal point
point_orth = [0.0, 1.0, 0.0]
pdf_orth = distribution.computePDF(point_orth)
assert pdf_orth > 0.0
# Ratio should match exp(beta - kappa)
ratio = pdf_orth / PDF
ott.assert_almost_equal(ratio, math.exp(-24.5), 1e-12, 0.0)

# Get parameters
param = distribution.getParameter()
assert param.getDimension() == 11
assert param[0] == 25.0

# Get individual parameters
assert distribution.getKappa() == 25.0
assert distribution.getBeta() == 0.5
gamma = distribution.getGamma()
assert gamma.getDimension() == 3
# Check columns match the original matrix
for i in range(3):
    for j in range(3):
        assert gamma[i, j] == gamma_matrix[i, j]

# Range
range_ = distribution.getRange()
ott.assert_almost_equal(range_.getLowerBound(), [-1.0] * 3)
ott.assert_almost_equal(range_.getUpperBound(), [1.0] * 3)

# Clone via Distribution wrapper
dist = ot.Distribution(distribution)
assert dist.getImplementation().getClassName() == "Kent"

# Test with beta=0 (von Mises-Fisher)
vmf = otexp.Kent(10.0, 0.0, gamma_matrix)
pdf_vmf = vmf.computePDF([1.0, 0.0, 0.0])
# For von Mises-Fisher on S^2: PDF at mode = kappa / (2*pi)
ott.assert_almost_equal(pdf_vmf, 10.0 / (2.0 * math.pi), 1e-4, 0.0)

# Default constructor
default = otexp.Kent()
assert default.getKappa() == 1.0
assert default.getBeta() == 0.25
gamma_default = default.getGamma()
for i in range(3):
    assert gamma_default[i, i] == 1.0


# Test beta=0 (von Mises-Fisher) moments with various kappa values
def coth(x):
    return 1.0 / math.tanh(x)


def vmf_r1(kappa):
    return coth(kappa) - 1.0 / kappa


# Large kappa vMF
vmf_large = otexp.Kent(10.0, 0.0, gamma_matrix)
mean_large = vmf_large.getMean()
ott.assert_almost_equal(mean_large[0], vmf_r1(10.0), 1e-12, 0.0)
ott.assert_almost_equal(mean_large[1], 0.0, 1e-12, 0.0)
ott.assert_almost_equal(mean_large[2], 0.0, 1e-12, 0.0)

# Small kappa vMF (kappa < 1, uses Taylor expansion)
vmf_small = otexp.Kent(0.1, 0.0, gamma_matrix)
mean_small = vmf_small.getMean()
ott.assert_almost_equal(mean_small[0], vmf_r1(0.1), 1e-12, 0.0)
ott.assert_almost_equal(mean_small[1], 0.0, 1e-12, 0.0)
ott.assert_almost_equal(mean_small[2], 0.0, 1e-12, 0.0)

# Very small kappa vMF
vmf_tiny = otexp.Kent(0.001, 0.0, gamma_matrix)
mean_tiny = vmf_tiny.getMean()
ott.assert_almost_equal(mean_tiny[0], vmf_r1(0.001), 0.0, 1e-11)
ott.assert_almost_equal(mean_tiny[1], 0.0, 1e-12, 0.0)
ott.assert_almost_equal(mean_tiny[2], 0.0, 1e-12, 0.0)

# Kappa = 1.0 vMF (boundary case)
vmf_boundary = otexp.Kent(1.0, 0.0, gamma_matrix)
mean_boundary = vmf_boundary.getMean()
ott.assert_almost_equal(mean_boundary[0], vmf_r1(1.0), 1e-12, 0.0)
ott.assert_almost_equal(mean_boundary[1], 0.0, 1e-12, 0.0)
ott.assert_almost_equal(mean_boundary[2], 0.0, 1e-12, 0.0)

# Kappa = 0.9 vMF (just below 1, tests Taylor near the boundary)
vmf_near = otexp.Kent(0.9, 0.0, gamma_matrix)
mean_near = vmf_near.getMean()
ott.assert_almost_equal(mean_near[0], vmf_r1(0.9), 1e-12, 0.0)
ott.assert_almost_equal(mean_near[1], 0.0, 1e-12, 0.0)
ott.assert_almost_equal(mean_near[2], 0.0, 1e-12, 0.0)


# Test general Kent mean for various (kappa, beta) combinations
def kent_r1(kappa, beta):
    # Use the static ComputeMoments indirectly via distribution mean
    d = otexp.Kent(kappa, beta, gamma_matrix)
    return d.getMean()[0]


# Test with very small kappa and moderate beta
d1 = otexp.Kent(0.5, 0.1, gamma_matrix)
m1 = d1.getMean()
assert m1[0] > 0.0

# Test near the boundary of the valid region (2*beta < kappa)
d2 = otexp.Kent(2.1, 1.0, gamma_matrix)
m2 = d2.getMean()
assert m2[0] > 0.0

# Test parameter validation: kappa <= 2*beta should fail
with ott.assert_raises(TypeError):
    otexp.Kent(1.0, 1.0, gamma_matrix)

# Test non-orthogonal matrix validation
with ott.assert_raises(TypeError):
    bad_matrix = ot.SquareMatrix([[2.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
    otexp.Kent(1.0, 0.25, bad_matrix)

# Test non-orthogonal matrix validation
with ott.assert_raises(TypeError):
    bad_matrix = ot.SquareMatrix([[1.0, 1.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
    otexp.Kent(25.0, 0.5, bad_matrix)

# Test wrong dimension validation
with ott.assert_raises(TypeError):
    bad_matrix = ot.SquareMatrix([[1.0, 0.0], [0.0, 1.0]])
    otexp.Kent(25.0, 0.5, bad_matrix)

# Test sampling quality: verify sample mean aligns with gamma_1
ot.RandomGenerator.SetSeed(0)
sample = distribution.getSample(5000)
mean = sample.computeMean()
# Mean should be aligned with gamma_1 = (1,0,0) for this concentrated distribution
assert abs(mean[0] - 1.0) < 0.1, "sample mean x=%.6f" % mean[0]
assert abs(mean[1]) < 0.1, "sample mean y=%.6f" % mean[1]
assert abs(mean[2]) < 0.1, "sample mean z=%.6f" % mean[2]
# All sample points should lie on the sphere
for i in range(10):
    norm = sample[i].norm()
    assert abs(norm - 1.0) < 1e-14

# Test sampling for von Mises-Fisher (beta=0)
ot.RandomGenerator.SetSeed(0)
vmf_sample = vmf.getSample(5000)
vmf_mean = vmf_sample.computeMean()
# For vMF with kappa=10, mean direction should be (1,0,0)
assert vmf_mean[0] > 0.85, "vmf sample mean x=%.6f" % vmf_mean[0]
assert abs(vmf_mean[1]) < 0.15, "vmf sample mean y=%.6f" % vmf_mean[1]
assert abs(vmf_mean[2]) < 0.15, "vmf sample mean z=%.6f" % vmf_mean[2]
for i in range(10):
    norm = vmf_sample[i].norm()
    assert abs(norm - 1.0) < 1e-14
