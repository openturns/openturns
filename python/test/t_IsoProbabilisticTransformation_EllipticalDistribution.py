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
            [-0.125, 0.07216878364870322, -0.05103103630798288],
            [0.0, -0.19245008972987526, 0.13608276348795434],
            [0.0, 0.0, 0.0],
        ]
    ),
    1e-10,
    1e-10,
)
# parameterGradient matches centered finite differences with fresh distributions
eps = 1e-5
factor = 1.0 / (2.0 * eps)
for side, trans, evalPoint in (("forward", transform, point), ("inverse", inverseTransform, transformedPoint)):
    fd = ot.Matrix(6, dim)
    for k in range(2 * dim):
        muP = list(meanPoint)
        sigmaP = list(sigma)
        muM = list(meanPoint)
        sigmaM = list(sigma)
        if k < dim:
            muP[k] += eps
            muM[k] -= eps
        else:
            sigmaP[k - dim] += eps
            sigmaM[k - dim] -= eps
        if side == "forward":
            left = ot.Normal(muP, sigmaP, R).getIsoProbabilisticTransformation()
            right = ot.Normal(muM, sigmaM, R).getIsoProbabilisticTransformation()
        else:
            left = ot.Normal(muP, sigmaP, R).getInverseIsoProbabilisticTransformation()
            right = ot.Normal(muM, sigmaM, R).getInverseIsoProbabilisticTransformation()
        dTdp = (left(evalPoint) - right(evalPoint)) * factor
        for i in range(dim):
            fd[k, i] = dTdp[i]
    # mu block
    ott.assert_almost_equal(trans.parameterGradient(evalPoint), fd, 1e-5, 1e-5)

# setParameter updates the evaluation and the gradient
oneDimTransform = ot.Normal(1.0, 2.0).getIsoProbabilisticTransformation()
ott.assert_almost_equal(oneDimTransform.getParameter(), [1.0, 2.0])
ott.assert_almost_equal(oneDimTransform([0.0]), [-0.5])
oneDimTransform.setParameter([5.0, 7.0])
ott.assert_almost_equal(oneDimTransform.getParameter(), [5.0, 7.0])
ott.assert_almost_equal(oneDimTransform([0.0]), [-5.0 / 7.0])
ott.assert_almost_equal(oneDimTransform.gradient([0.0]), ot.Matrix([[1.0 / 7.0]]))
oneDimInverseTransform = ot.Normal(1.0, 2.0).getInverseIsoProbabilisticTransformation()
oneDimInverseTransform.setParameter([5.0, 7.0])
ott.assert_almost_equal(oneDimInverseTransform([0.5]), [5.0 + 7.0 * 0.5])
ott.assert_almost_equal(oneDimInverseTransform.gradient([0.5]), ot.Matrix([[7.0]]))

# the transformation parameters are (mu, sigma), even with extra marginal
# parameters such as the degrees of freedom of a Student distribution
student = ot.Student(4.5, 3)
ott.assert_almost_equal(
    student.getIsoProbabilisticTransformation().getParameter(), [0.0] * 3 + [1.0] * 3
)
assert list(student.getIsoProbabilisticTransformation().getParameterDescription()) == [
    "X0_mu_0",
    "X1_mu_1",
    "X2_mu_2",
    "X0_sigma_0",
    "X1_sigma_1",
    "X2_sigma_2",
]

# parameters updated through setParameter match a fresh transformation
transform.setParameter([5.0, 6.0, 7.0, 8.0, 9.0, 10.0])
reference = ot.Normal([5.0, 6.0, 7.0], [8.0, 9.0, 10.0], R)
referenceTransform = reference.getIsoProbabilisticTransformation()
ott.assert_almost_equal(transform(point), referenceTransform(point))
ott.assert_almost_equal(transform.gradient(point), referenceTransform.gradient(point))
ott.assert_almost_equal(
    transform.parameterGradient(point), referenceTransform.parameterGradient(point)
)
inverseTransform.setParameter([5.0, 6.0, 7.0, 8.0, 9.0, 10.0])
referenceInverseTransform = reference.getInverseIsoProbabilisticTransformation()
ott.assert_almost_equal(
    inverseTransform(transformedPoint), referenceInverseTransform(transformedPoint)
)
ott.assert_almost_equal(
    inverseTransform.gradient(transformedPoint),
    referenceInverseTransform.gradient(transformedPoint),
)
# direct construction: diagonal maps honor setParameter
L = ot.TriangularMatrix(1)
L[0, 0] = 0.5
e = ot.NatafEllipticalDistributionEvaluation([1.0], L)
ott.assert_almost_equal(e.getParameter(), [1.0, 2.0])
e.setParameter([5.0, 7.0])
ott.assert_almost_equal(e.getParameter(), [5.0, 7.0])
ott.assert_almost_equal(e([0.0]), [-5.0 / 7.0])
C = ot.TriangularMatrix(1)
C[0, 0] = 2.0
ie = ot.InverseNatafEllipticalDistributionEvaluation([1.0], C)
ie.setParameter([5.0, 7.0])
ott.assert_almost_equal(ie([0.5]), [5.0 + 7.0 * 0.5])
# direct construction: correlated maps reject setParameter without recorded scales
gammaCorr = ot.TriangularMatrix(2)
gammaCorr[0, 0] = 1.0
gammaCorr[1, 0] = 0.5
gammaCorr[1, 1] = 1.0
eCorr = ot.NatafEllipticalDistributionEvaluation([0.0, 0.0], gammaCorr)
with ott.assert_raises(Exception):
    eCorr.setParameter([0.0, 0.0, 1.0, 2.0])
# degenerate sigma: the transformation is well-defined with zero rows/columns
degenerate = ot.Normal([0.0], [0.0])
ott.assert_almost_equal(degenerate.computeCDF([0.0]), 1.0)
degenerateTransform = degenerate.getIsoProbabilisticTransformation()
ott.assert_almost_equal(degenerateTransform.getParameter(), [0.0, 0.0])
ott.assert_almost_equal(degenerateTransform([1.0]), [0.0])
ott.assert_almost_equal(degenerateTransform.parameterGradient([1.0]), ot.Matrix([[0.0], [0.0]]))
degenerateInverse = degenerate.getInverseIsoProbabilisticTransformation()
ott.assert_almost_equal(degenerateInverse([1.0]), [0.0])
ott.assert_almost_equal(degenerateInverse.parameterGradient([1.0]), ot.Matrix([[1.0], [0.0]]))
# re-setting the same degenerate parameters keeps the zero map
degenerateTransform.setParameter([0.0, 0.0])
ott.assert_almost_equal(degenerateTransform([1.0]), [0.0])
# transitions to/from degeneracy require a rebuild
with ott.assert_raises(Exception):
    degenerateTransform.setParameter([0.0, 2.0])
with ott.assert_raises(Exception):
    oneDimTransform.setParameter([1.0, 0.0])
# mixed degenerate case: non-degenerate coordinates keep the usual Nataf map
mixed = ot.Normal([0.0, 0.0], [1.0, 0.0])
mixedTransform = mixed.getIsoProbabilisticTransformation()
ott.assert_almost_equal(mixedTransform([1.0, 1.0]), [1.0, 0.0])
mixedInverse = mixed.getInverseIsoProbabilisticTransformation()
ott.assert_almost_equal(mixedInverse([1.0, 0.0]), [1.0, 0.0])
ott.assert_almost_equal(mixedInverse(mixedTransform([1.0, 1.0])), [1.0, 0.0])
