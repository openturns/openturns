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
ott.assert_almost_equal(oneRealization.norm(), 1.0)

# Define a point on the sphere
point = [1.0, 0.0, 0.0]

# Show PDF and log-PDF of point
LPDF = distribution.computeLogPDF(point)
PDF = distribution.computePDF(point)
assert PDF > 0.0
ott.assert_almost_equal(PDF, 3.9760566228, 5e-3, 0.0)
ott.assert_almost_equal(LPDF, 1.3802905298, 5e-4, 0.0)

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
    ott.assert_almost_equal(norm, 1.0)

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
    ott.assert_almost_equal(norm, 1.0)

# Test getEpsilon
ott.assert_almost_equal(
    distribution.getEpsilon(), ot.ResourceMap.GetAsScalar("Kent-OrthogonalityThreshold")
)

# Test __repr__ and __str__
repr_str = repr(distribution)
assert "Kent" in repr_str
assert "kappa=" in repr_str
str_str = str(distribution)
assert "Kent" in str_str
assert "kappa =" in str_str

# Test getParameterDescription
desc = distribution.getParameterDescription()
assert desc.getSize() == 11
assert desc[0] == "kappa"
assert desc[1] == "beta"

# Test setParameter / getParameter round-trip
param = distribution.getParameter()
new_dist = otexp.Kent()
new_dist.setParameter(param)
ott.assert_almost_equal(new_dist.getKappa(), distribution.getKappa())
ott.assert_almost_equal(new_dist.getBeta(), distribution.getBeta())
new_gamma = new_dist.getGamma()
for i in range(3):
    for j in range(3):
        ott.assert_almost_equal(new_gamma[i, j], gamma_matrix[i, j])

# Test setParameter with wrong size
with ott.assert_raises(TypeError):
    distribution.setParameter([1.0, 2.0])

# Test setKappa
dist_setter = otexp.Kent(5.0, 0.5, gamma_matrix)
dist_setter.setKappa(10.0)
ott.assert_almost_equal(dist_setter.getKappa(), 10.0)
# setKappa to same value should be a no-op
dist_setter.setKappa(10.0)
ott.assert_almost_equal(dist_setter.getKappa(), 10.0)
# setKappa with invalid values
with ott.assert_raises(TypeError):
    dist_setter.setKappa(0.0)
with ott.assert_raises(TypeError):
    dist_setter.setKappa(-1.0)
# setKappa that violates 2*beta < kappa
with ott.assert_raises(TypeError):
    dist_setter.setKappa(0.5)  # beta=0.5, 2*beta=1.0 > 0.5

# Test setBeta
dist_setter2 = otexp.Kent(10.0, 0.5, gamma_matrix)
dist_setter2.setBeta(1.0)
ott.assert_almost_equal(dist_setter2.getBeta(), 1.0)
# setBeta to same value should be a no-op
dist_setter2.setBeta(1.0)
ott.assert_almost_equal(dist_setter2.getBeta(), 1.0)
# setBeta with invalid values
with ott.assert_raises(TypeError):
    dist_setter2.setBeta(-0.1)
# setBeta that violates 2*beta < kappa
with ott.assert_raises(TypeError):
    dist_setter2.setBeta(6.0)  # 2*6=12 > 10

# Test setBeta to 0 (von Mises-Fisher)
dist_setter2.setBeta(0.0)
ott.assert_almost_equal(dist_setter2.getBeta(), 0.0)

# Test setGamma
identity = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
theta = 0.5
cos_t = math.cos(theta)
sin_t = math.sin(theta)
rotated = ot.SquareMatrix([[cos_t, -sin_t, 0.0], [sin_t, cos_t, 0.0], [0.0, 0.0, 1.0]])
dist_setter3 = otexp.Kent(10.0, 0.5, identity)
dist_setter3.setGamma(rotated)
g = dist_setter3.getGamma()
ott.assert_almost_equal(g[0, 0], cos_t)
ott.assert_almost_equal(g[0, 1], -sin_t)
ott.assert_almost_equal(g[1, 0], sin_t)
ott.assert_almost_equal(g[1, 1], cos_t)
# setGamma to same value should be a no-op
dist_setter3.setGamma(rotated)
# setGamma with wrong dimension
with ott.assert_raises(TypeError):
    dist_setter3.setGamma(ot.SquareMatrix([[1.0, 0.0], [0.0, 1.0]]))
# setGamma with non-orthogonal matrix
with ott.assert_raises(TypeError):
    dist_setter3.setGamma(
        ot.SquareMatrix([[2.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
    )

# Test computePDF with wrong dimension
with ott.assert_raises(TypeError):
    distribution.computePDF([1.0, 0.0])
with ott.assert_raises(TypeError):
    distribution.computeLogPDF([1.0, 0.0])

# Test with a rotated gamma distribution
theta = 0.7
cos_t = math.cos(theta)
sin_t = math.sin(theta)
rot_gamma = ot.SquareMatrix(
    [[cos_t, -sin_t, 0.0], [sin_t, cos_t, 0.0], [0.0, 0.0, 1.0]]
)
rot_dist = otexp.Kent(15.0, 0.3, rot_gamma)
rot_sample = rot_dist.getSample(5000)
rot_mean = rot_sample.computeMean()
# Mean should be aligned with gamma_1 = (cos(0.7), sin(0.7), 0)
expected_gamma1_x = cos_t
expected_gamma1_y = sin_t
norm_mean = math.sqrt(rot_mean[0] ** 2 + rot_mean[1] ** 2)
if norm_mean > 0.01:
    dir_x = rot_mean[0] / norm_mean
    dir_y = rot_mean[1] / norm_mean
    ott.assert_almost_equal(dir_x, expected_gamma1_x, 0.15, 0.0)
    ott.assert_almost_equal(dir_y, expected_gamma1_y, 0.15, 0.0)

# Test with very small kappa (diffuse distribution)
diffuse = otexp.Kent(0.01, 0.0, gamma_matrix)
diffuse_sample = diffuse.getSample(5000)
diffuse_mean = diffuse_sample.computeMean()
# For very small kappa, mean should be close to 0
assert abs(diffuse_mean[0]) < 0.1

# Test with beta very close to kappa/2 (maximally elliptical, 2*beta < kappa)
near_boundary = otexp.Kent(10.0, 4.9, gamma_matrix)
near_sample = near_boundary.getSample(1000)
assert near_sample.getSize() == 1000

# Test dimension
assert distribution.getDimension() == 3

# Test isContinuous
assert distribution.isContinuous()

# Test entropy
entropy = distribution.computeEntropy()
# Ground truth from high-precision series: H = log(C) - kappa*r1 - beta*r2
ott.assert_almost_equal(
    entropy,
    23.61970947016114 - 25.0 * 0.9599468514772579 - 0.5 * 0.002834469807834422,
    1e-12,
    0.0,
)
# Entropy of a Kent(10, 0.5)
entropy_k = otexp.Kent(10.0, 0.5, gamma_matrix).computeEntropy()
ott.assert_almost_equal(entropy_k, 0.5377389709666636, 1e-12, 0.0)
# For beta=0 the entropy must match the von Mises-Fisher entropy
vmf_k10 = otexp.Kent(10.0, 0.0, gamma_matrix).computeEntropy()
ott.assert_almost_equal(vmf_k10, 0.5352919301310737, 1e-12, 0.0)

# Test covariance
covariance = distribution.getCovariance()
# Ground truth from high-precision series: Gamma*diag(sigma_i^2)*Gamma^T
ott.assert_almost_equal(covariance[0, 0], 0.0016059690357003, 1e-11, 0.0)
ott.assert_almost_equal(covariance[1, 1], 0.0398652715555167, 1e-11, 0.0)
ott.assert_almost_equal(covariance[2, 2], 0.0370308017476823, 1e-11, 0.0)
ott.assert_almost_equal(covariance[0, 1], 0.0, 1e-11, 0.0)
ott.assert_almost_equal(covariance[0, 2], 0.0, 1e-11, 0.0)

# Covariance for Kent(10, 0.5)
cov_k = otexp.Kent(10.0, 0.5, gamma_matrix).getCovariance()
ott.assert_almost_equal(cov_k[0, 0], 0.0101504873472573, 1e-12, 0.0)
ott.assert_almost_equal(cov_k[1, 1], 0.0978018104131944, 1e-12, 0.0)
ott.assert_almost_equal(cov_k[2, 2], 0.0831483940183857, 1e-12, 0.0)

# Covariance for a von Mises-Fisher (beta=0): sig2^2 = sig3^2 = r1/kappa
vmf_cov = otexp.Kent(1.0, 0.0, gamma_matrix).getCovariance()
ott.assert_almost_equal(vmf_cov[0, 0], 0.2759383390336895, 1e-12, 0.0)
ott.assert_almost_equal(vmf_cov[1, 1], 0.3130352854993313, 1e-12, 0.0)
ott.assert_almost_equal(vmf_cov[2, 2], 0.3130352854993313, 1e-12, 0.0)

# Test covariance transformation under a rotation of the basis
sigma = otexp.Kent(10.0, 0.5, rot_gamma).getCovariance()
ott.assert_almost_equal(sigma[0, 0], 0.0465272264037538, 1e-11, 0.0)
ott.assert_almost_equal(sigma[1, 1], 0.0614250713566972, 1e-11, 0.0)
ott.assert_almost_equal(sigma[2, 2], 0.0831483940183858, 1e-11, 0.0)
ott.assert_almost_equal(sigma[0, 1], -0.0431879863242299, 1e-11, 0.0)
ott.assert_almost_equal(sigma[0, 2], 0.0, 1e-11, 0.0)

# Test that mean for rotated distribution is correctly transformed
mean_rot = rot_dist.getMean()
r1 = mean_rot.norm()
# gamma_1 = (cos(theta), sin(theta), 0)
ott.assert_almost_equal(mean_rot[0], r1 * cos_t, 1e-12, 0.0)
ott.assert_almost_equal(mean_rot[1], r1 * sin_t, 1e-12, 0.0)
ott.assert_almost_equal(mean_rot[2], 0.0, 1e-12, 0.0)
