#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
gamma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
distribution = otexp.Bingham([0.5, 0.3, 0.0], gamma)
print("Distribution ", distribution)

# Is this distribution continuous ?
assert distribution.isContinuous()

# Dimension
assert distribution.getDimension() == 3

# Test for realization of distribution: points must lie on the sphere
ot.RandomGenerator.SetSeed(0)
realization = distribution.getRealization()
assert realization.getDimension() == 3
ott.assert_almost_equal(realization.norm(), 1.0, 1e-12, 0.0)

# PDF at a point on the sphere
point = [1.0, 0.0, 0.0]
pdf = distribution.computePDF(point)
logpdf = distribution.computeLogPDF(point)
ott.assert_almost_equal(logpdf, math.log(pdf), 1e-12, 0.0)

# PDF at a point off the sphere is zero
ott.assert_almost_equal(
    distribution.computePDF([1.0, 1.0, 0.0]), 0.0, 1e-12, 0.0
)

# Uniform distribution on the sphere S^2: PDF = 1 / (4 * pi)
uniform = otexp.Bingham([0.0, 0.0, 0.0], gamma)
ott.assert_almost_equal(
    uniform.computePDF([1.0, 0.0, 0.0]), 1.0 / (4.0 * math.pi), 1e-6, 0.0
)
ott.assert_almost_equal(
    uniform.computePDF([1.0 / math.sqrt(3.0)] * 3),
    1.0 / (4.0 * math.pi),
    1e-6,
    0.0,
)

# Uniform distribution on the circle S^1: PDF = 1 / (2 * pi)
circle = otexp.Bingham(
    [0.0, 0.0], ot.SquareMatrix([[1.0, 0.0], [0.0, 1.0]])
)
ott.assert_almost_equal(
    circle.computePDF([1.0, 0.0]), 1.0 / (2.0 * math.pi), 1e-6, 0.0
)

# PDF ratio along the axes: f(e_i) / f(e_j) = exp(zeta_i - zeta_j)
pdf_x = distribution.computePDF([1.0, 0.0, 0.0])
pdf_y = distribution.computePDF([0.0, 1.0, 0.0])
pdf_z = distribution.computePDF([0.0, 0.0, 1.0])
ott.assert_almost_equal(pdf_x / pdf_y, math.exp(0.2), 1e-6, 0.0)
ott.assert_almost_equal(pdf_x / pdf_z, math.exp(0.5), 1e-6, 0.0)
ott.assert_almost_equal(pdf_y / pdf_z, math.exp(0.3), 1e-6, 0.0)

# The mean is zero by symmetry
mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.0, 0.0, 0.0], 1e-12, 0.0)

# Covariance: trace must be 1 since all points lie on the sphere
covariance = distribution.getCovariance()
trace = covariance[0, 0] + covariance[1, 1] + covariance[2, 2]
ott.assert_almost_equal(trace, 1.0, 1e-5, 0.0)

# Uniform covariance: diagonal = 1/n and zero off-diagonal
cov_uniform = uniform.getCovariance()
for i in range(3):
    ott.assert_almost_equal(cov_uniform[i, i], 1.0 / 3.0, 1e-6, 0.0)
    for j in range(i + 1, 3):
        ott.assert_almost_equal(cov_uniform[i, j], 0.0, 1e-6, 0.0)

# Entropy: uniform on S^{n-1} has entropy log(area(S^{n-1}))
ott.assert_almost_equal(
    uniform.computeEntropy(), math.log(4.0 * math.pi), 1e-6, 0.0
)

# Get parameters: zeta then gamma flattened row-major
param = distribution.getParameter()
assert param.getDimension() == 3 + 9
ott.assert_almost_equal(param[0], 0.5, 1e-12, 0.0)
ott.assert_almost_equal(param[1], 0.3, 1e-12, 0.0)
ott.assert_almost_equal(param[2], 0.0, 1e-12, 0.0)
ott.assert_almost_equal(param[3], 1.0, 1e-12, 0.0)

# Get individual parameters
zeta = distribution.getZeta()
ott.assert_almost_equal(zeta, [0.5, 0.3, 0.0], 1e-12, 0.0)
g = distribution.getGamma()
for i in range(3):
    for j in range(3):
        ott.assert_almost_equal(g[i, j], gamma[i, j], 1e-12, 0.0)
ott.assert_almost_equal(
    distribution.getEpsilon(),
    ot.ResourceMap.GetAsScalar("Bingham-OrthogonalityThreshold"),
)

# Range
range_ = distribution.getRange()
ott.assert_almost_equal(range_.getLowerBound(), [-1.0] * 3)
ott.assert_almost_equal(range_.getUpperBound(), [1.0] * 3)

# Clone via Distribution wrapper
dist = ot.Distribution(distribution)
assert dist.getImplementation().getClassName() == "Bingham"

# Default constructor
default = otexp.Bingham()
assert default.getDimension() == 3
ott.assert_almost_equal(default.getZeta(), [0.0, -0.5, -1.0], 1e-12, 0.0)

# setZeta / getZeta round-trip
dist_setter = otexp.Bingham([0.5, 0.3, 0.0], gamma)
dist_setter.setZeta([1.0, 0.5, 0.0])
ott.assert_almost_equal(dist_setter.getZeta(), [1.0, 0.5, 0.0], 1e-12, 0.0)
with ott.assert_raises(TypeError):
    dist_setter.setZeta([1.0, 2.0])

# setGamma / getGamma round-trip
theta = 0.5
rotated = ot.SquareMatrix(
    [[math.cos(theta), -math.sin(theta), 0.0],
     [math.sin(theta), math.cos(theta), 0.0],
     [0.0, 0.0, 1.0]]
)
dist_setter.setGamma(rotated)
sdim = dist_setter.getDimension()
for i in range(sdim):
    for j in range(sdim):
        ott.assert_almost_equal(
            dist_setter.getGamma()[i, j], rotated[i, j], 1e-12, 0.0
        )

# Invalid gamma: non-orthogonal or wrong dimension
with ott.assert_raises(TypeError):
    otexp.Bingham(
        [0.5, 0.3, 0.0],
        ot.SquareMatrix([[2.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]),
    )
with ott.assert_raises(TypeError):
    otexp.Bingham([0.5], ot.SquareMatrix([[1.0, 0.0], [0.0, 1.0]]))

# setParameter / getParameter round-trip
param = distribution.getParameter()
new_dist = otexp.Bingham()
new_dist.setParameter(param)
ott.assert_almost_equal(new_dist.getParameter(), param, 1e-12, 0.0)
with ott.assert_raises(TypeError):
    distribution.setParameter([1.0, 2.0])

# getParameterDescription
desc = distribution.getParameterDescription()
assert desc.getSize() == 3 + 9

# computePDF with wrong dimension
with ott.assert_raises(TypeError):
    distribution.computePDF([1.0, 0.0])
with ott.assert_raises(TypeError):
    distribution.computeLogPDF([1.0, 0.0])

# Sampling quality: sample mean is zero and points lie on the sphere
ot.RandomGenerator.SetSeed(0)
sample = distribution.getSample(5000)
mean = sample.computeMean()
ott.assert_almost_equal(mean, [0.0, 0.0, 0.0], 0.0, 5e-2)
for i in range(10):
    ott.assert_almost_equal(sample[i].norm(), 1.0, 1e-12, 0.0)

# Sampling quality in the concentrated case
ot.RandomGenerator.SetSeed(0)
concentrated = otexp.Bingham([3.0, 1.0, 0.0], gamma)
sample = concentrated.getSample(5000)
mean = sample.computeMean()
ott.assert_almost_equal(mean, [0.0, 0.0, 0.0], 0.0, 5e-2)
# The density is peaked around the equator orthogonal to the first axis
var_sample = sample.computeCovariance()
ott.assert_almost_equal(var_sample[0, 0], concentrated.getCovariance()[0, 0],
                        0.1, 0.0)

# __repr__ and __str__
assert "Bingham" in repr(distribution)
assert "Bingham" in str(distribution)
