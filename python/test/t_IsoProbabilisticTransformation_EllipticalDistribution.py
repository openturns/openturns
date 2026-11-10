#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
dim = 3
meanPoint = ot.Point(dim, 1.0)
meanPoint[0] = 0.5
meanPoint[1] = -0.5
sigma = ot.Point(dim, 1.0)
sigma[0] = 2.0
sigma[1] = 3.0
R = ot.CorrelationMatrix(dim)
for i in range(1, dim):
    R[i, i - 1] = 0.5
distribution = ot.Normal(meanPoint, sigma, R)

# Test for sampling: empirical moments match the theoretical ones
size = 10000
sample = distribution.getSample(size)
assert sample.getSize() == size
assert sample.getDimension() == dim
ott.assert_almost_equal(sample.computeMean(), distribution.getMean(), 1e-1, 1e-1)
ott.assert_almost_equal(
    sample.computeCovariance(), distribution.getCovariance(), 1e-1, 1e-1
)

transform = distribution.getIsoProbabilisticTransformation()
assert transform.getInputDimension() == dim
assert transform.getOutputDimension() == dim
transformedSample = transform(sample)
# the transformed sample follows a standard normal distribution
ott.assert_almost_equal(transformedSample.computeMean(), [0.0] * dim, 0.0, 5e-2)
ott.assert_almost_equal(
    transformedSample.computeCovariance(), ot.IdentityMatrix(dim), 1e-1, 5e-2
)

# Test for evaluation
inverseTransform = distribution.getInverseIsoProbabilisticTransformation()
assert inverseTransform.getInputDimension() == dim
assert inverseTransform.getOutputDimension() == dim
transformedBackSample = inverseTransform(transformedSample)
# round-trip through the transformation recovers the input sample
ott.assert_almost_equal(transformedBackSample, sample, 1e-10, 1e-10)
ott.assert_almost_equal(
    transformedBackSample.computeMean(), distribution.getMean(), 1e-1, 1e-1
)
ott.assert_almost_equal(
    transformedBackSample.computeCovariance(),
    distribution.getCovariance(),
    1e-1,
    1e-1,
)

point = ot.Point(dim, 1.0)
transformedPoint = transform(point)
ott.assert_almost_equal(
    transformedPoint, [0.25, 0.4330127018922193, -0.3061862178478972], 1e-12, 1e-12
)
gradient = transform.gradient(point)
ott.assert_almost_equal(
    gradient,
    ot.Matrix(
        [
            [0.5, -0.28867513459481287, 0.20412414523193148],
            [0.0, 0.3849001794597505, -0.2721655269759086],
            [0.0, 0.0, 1.224744871391589],
        ]
    ),
    1e-12,
    1e-12,
)
gradientFD = ot.CenteredFiniteDifferenceGradient(
    1.0e-5, transform.getEvaluation()
).gradient(point)
ott.assert_almost_equal(gradient, gradientFD, 1e-8, 1e-8)
# the transformation is linear so its hessian is zero
hessian = transform.hessian(point)
hessianFD = ot.CenteredFiniteDifferenceHessian(
    1.0e-4, transform.getEvaluation()
).hessian(point)
for i in range(dim):
    for j in range(dim):
        for k in range(dim):
            assert abs(hessian[i, j, k]) < 1e-10
            assert abs(hessianFD[i, j, k]) < 1e-4

ott.assert_almost_equal(inverseTransform(transformedPoint), point, 1e-12, 1e-12)
inverseGradient = inverseTransform.gradient(transformedPoint)
ott.assert_almost_equal(
    inverseGradient,
    ot.Matrix(
        [
            [2.0, 1.5, 0.0],
            [0.0, 2.598076211353316, 0.5773502691896257],
            [0.0, 0.0, 0.816496580927726],
        ]
    ),
    1e-12,
    1e-12,
)
inverseGradientFD = ot.CenteredFiniteDifferenceGradient(
    1.0e-5, inverseTransform.getEvaluation()
).gradient(transformedPoint)
ott.assert_almost_equal(inverseGradient, inverseGradientFD, 1e-8, 1e-8)
inverseHessian = inverseTransform.hessian(transformedPoint)
inverseHessianFD = ot.CenteredFiniteDifferenceHessian(
    1.0e-4, inverseTransform.getEvaluation()
).hessian(transformedPoint)
for i in range(dim):
    for j in range(dim):
        for k in range(dim):
            assert abs(inverseHessian[i, j, k]) < 1e-10
            assert abs(inverseHessianFD[i, j, k]) < 1e-4

# Test for parameters
ott.assert_almost_equal(
    transform.parameterGradient(point),
    ot.Matrix(
        [
            [-0.5, 0.28867513459481287, -0.20412414523193148],
            [0.0, -0.3849001794597505, 0.2721655269759086],
            [0.0, 0.0, -1.224744871391589],
            [-0.125, 0.0, 0.0],
            [0.0, -0.22222222222222227, 0.0],
            [0.0, 0.0, 0.0],
        ]
    ),
    1e-10,
    1e-10,
)
