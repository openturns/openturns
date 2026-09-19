#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
F = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 0.5, 0.0], [0.0, 0.0, 0.1]])
distribution = otexp.MatrixFisher(F)
print("Distribution ", distribution)

# Is this distribution continuous ?
assert distribution.isContinuous()

# Dimension: 3x3 matrix flattened
assert distribution.getDimension() == 9

# Test for realization: must be a rotation matrix in SO(3)
ot.RandomGenerator.SetSeed(0)
realization = ot.SquareMatrix(3)
flat = distribution.getRealization()
for i in range(3):
    for j in range(3):
        realization[i, j] = flat[i * 3 + j]
prod = realization * realization.transpose()
for i in range(3):
    for j in range(3):
        ott.assert_almost_equal(prod[i, j], 1.0 if i == j else 0.0,
                                1e-12, 1e-15)
ott.assert_almost_equal(realization.computeDeterminant(), 1.0, 1e-12, 0.0)

# Uniform distribution: F=0 gives the Haar measure on SO(3), PDF = 1 / (8 pi^2)
uniform = otexp.MatrixFisher(ot.SquareMatrix(3))
identity = ot.SquareMatrix(3)
identity[0, 0] = 1.0
identity[1, 1] = 1.0
identity[2, 2] = 1.0
pdf_id = uniform.computePDF([1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0])
ott.assert_almost_equal(pdf_id, 1.0 / (8.0 * math.pi**2), 1e-6, 0.0)

# PDF ratio: for axial F = diag(k, 0, 0), f(I) / f(Rz(pi)) = exp(2 k)
axial = otexp.MatrixFisher(ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 0.0, 0.0],
                                            [0.0, 0.0, 0.0]]))
rot180 = ot.SquareMatrix(
    [[-1.0, 0.0, 0.0], [0.0, -1.0, 0.0], [0.0, 0.0, 1.0]]
)
flat180 = [rot180[i // 3, i % 3] for i in range(9)]
ratio = axial.computePDF([1.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                          0.0, 0.0, 1.0]) / axial.computePDF(flat180)
ott.assert_almost_equal(ratio, math.exp(2.0), 1e-6, 0.0)

# Entropy of the uniform distribution: log(8 pi^2)
ott.assert_almost_equal(
    uniform.computeEntropy(), math.log(8.0 * math.pi**2), 1e-6, 0.0
)

# Mean of the uniform distribution is the zero matrix
mean = uniform.getMean()
ott.assert_almost_equal(mean, [0.0] * 9, 0.0, 1e-3)

# Get parameters: F flattened row-major
param = distribution.getParameter()
assert param.getDimension() == 9
ott.assert_almost_equal(param[0], 1.0, 1e-12, 0.0)
ott.assert_almost_equal(param[4], 0.5, 1e-12, 0.0)
ott.assert_almost_equal(param[8], 0.1, 1e-12, 0.0)

# Get individual parameters
g = distribution.getF()
for i in range(3):
    for j in range(3):
        ott.assert_almost_equal(g[i, j], F[i, j], 1e-12, 0.0)
ott.assert_almost_equal(
    distribution.getEpsilon(),
    ot.ResourceMap.GetAsScalar("MatrixFisher-OrthogonalityThreshold"),
)

# Range: entries of rotation matrices lie in [-1, 1]
range_ = distribution.getRange()
ott.assert_almost_equal(range_.getLowerBound(), [-1.0] * 9)
ott.assert_almost_equal(range_.getUpperBound(), [1.0] * 9)

# Clone via Distribution wrapper
dist = ot.Distribution(distribution)
assert dist.getImplementation().getClassName() == "MatrixFisher"

# Default constructor
default = otexp.MatrixFisher()
assert default.getDimension() == 9

# setF / getF round-trip
dist_setter = otexp.MatrixFisher(F)
F2 = ot.SquareMatrix([[2.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 0.5]])
dist_setter.setF(F2)
g2 = dist_setter.getF()
for i in range(3):
    for j in range(3):
        ott.assert_almost_equal(g2[i, j], F2[i, j], 1e-12, 0.0)

# Invalid F: wrong dimension
with ott.assert_raises(TypeError):
    otexp.MatrixFisher(ot.SquareMatrix(4))

# setParameter / getParameter round-trip
new_dist = otexp.MatrixFisher()
new_dist.setParameter(param)
ott.assert_almost_equal(new_dist.getParameter(), param, 1e-12, 0.0)
with ott.assert_raises(TypeError):
    distribution.setParameter([1.0] * 8)

# setParameter preserves a custom epsilon
custom_eps = 1e-9
new_dist.setEpsilon(custom_eps)
new_dist.setParameter(param)
ott.assert_almost_equal(new_dist.getEpsilon(), custom_eps, 1e-12, 0.0)

# getParameterDescription
desc = distribution.getParameterDescription()
assert desc.getSize() == 9

# computePDF with wrong dimension
with ott.assert_raises(TypeError):
    distribution.computePDF([1.0] * 8)
with ott.assert_raises(TypeError):
    distribution.computeLogPDF([1.0] * 10)

# Sampling quality: the sample mean aligns with U diag(r) V^T, all rotations
ot.RandomGenerator.SetSeed(0)
sample = distribution.getSample(2000)
mean = sample.computeMean()
ott.assert_almost_equal(mean, distribution.getMean(), 0.0, 2e-2)
for i in range(10):
    R = ot.SquareMatrix(3)
    for r in range(3):
        for c in range(3):
            R[r, c] = sample[i, r * 3 + c]
    prod = R * R.transpose()
    for r in range(3):
        for c in range(3):
            ott.assert_almost_equal(prod[r, c], 1.0 if r == c else 0.0,
                                    1e-9, 1e-15)
    ott.assert_almost_equal(R.computeDeterminant(), 1.0, 1e-9, 0.0)

# Distribution validation
validation = ott.DistributionValidation(distribution)
validation.skipCDF()
validation.skipCorrelation()
validation.skipDependenceMeasures()
validation.skipGradient()
validation.skipMoments()
validation.skipDDF()
validation.skipQuantile()
validation.skipProbability()
validation.skipMinimumVolumeLevelSet()
validation.skipMinimumVolumeInterval()
validation.skipConfidenceInterval()
validation.skipInverseSurvival()
validation.skipComplementaryCDF()
validation.skipConditional()
validation.skipTransformation()
validation.skipParameters()
validation.run()

# __repr__ and __str__
assert "MatrixFisher" in repr(distribution)
assert "MatrixFisher" in str(distribution)
