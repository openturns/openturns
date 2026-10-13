#! /usr/bin/env python

import os
import tempfile
from math import sqrt

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()


def test_model(myModel, test_partial_grad=True, x1=None, x2=None):
    inputDimension = myModel.getInputDimension()
    dimension = myModel.getOutputDimension()

    if x1 is None and x2 is None:
        x1 = ot.Point(inputDimension)
        x2 = ot.Point(inputDimension)
        for j in range(inputDimension):
            x1[j] = -1.0 - j
            x2[j] = 3.0 + 2.0 * j
    else:
        x1 = ot.Point(x1)
        x2 = ot.Point(x2)

    if myModel.isStationary():
        ott.assert_almost_equal(myModel(x1 - x2), myModel(x1, x2), 1e-14, 1e-14)
        ott.assert_almost_equal(myModel(x2 - x1), myModel(x1, x2), 1e-14, 1e-14)

    eps = 1e-3

    # a coarse mesh is enough: all the checks below are cross-method
    # consistency checks, independent of the mesh size
    mesh = ot.IntervalMesher([3] * inputDimension).build(
        ot.Interval([-10] * inputDimension, [10] * inputDimension)
    )

    C = myModel.discretize(mesh)
    if dimension == 1:
        # Check that discretize & computeAsScalar provide the
        # same values
        vertices = mesh.getVertices()
        for j in range(len(vertices)):
            for i in range(j, len(vertices)):
                ott.assert_almost_equal(
                    C[i, j],
                    myModel.computeAsScalar(vertices[i], vertices[j]),
                    1e-14,
                    1e-14,
                )
    else:
        # Check that discretize & operator() provide the same values
        vertices = mesh.getVertices()
        localMatrix = ot.SquareMatrix(dimension)
        for j in range(len(vertices)):
            for i in range(j, len(vertices)):
                for localJ in range(dimension):
                    for localI in range(dimension):
                        localMatrix[localI, localJ] = C[
                            i * dimension + localI, j * dimension + localJ
                        ]
                ott.assert_almost_equal(
                    localMatrix, myModel(vertices[i], vertices[j]), 1e-14, 1e-14
                )

    # Now we suppose that discretize is ok
    # we look at crossCovariance of (vertices, vertices) which should return the same values
    C.getImplementation().symmetrize()
    crossCov = myModel.computeCrossCovariance(vertices, vertices)
    ott.assert_almost_equal(
        crossCov,
        C,
        1e-14,
        1e-14,
        "in " + myModel.getClassName() + "::computeCrossCovariance",
    )

    # Now crossCovariance(sample, sample) is ok
    # Let us validate crossCovariance(Sample, point) with 1st column(s) of previous calculations
    crossCovSamplePoint = myModel.computeCrossCovariance(vertices, vertices[0])
    crossCovCol = crossCov.reshape(crossCov.getNbRows(), dimension)
    ott.assert_almost_equal(
        crossCovSamplePoint,
        crossCovCol,
        1e-14,
        1e-14,
        "in " + myModel.getClassName() + "::computeCrossCovarianceSamplePoint",
    )

    if test_partial_grad:
        grad = myModel.partialGradient(x1, x2)

        if dimension == 1:
            gradfd = ot.Matrix(inputDimension, 1)
            for j in range(inputDimension):
                x1_g = ot.Point(x1)
                x1_d = ot.Point(x1)
                x1_g[j] = x1_d[j] + eps
                x1_d[j] = x1_d[j] - eps
                gradfd[j, 0] = (
                    myModel.computeAsScalar(x1_g, x2)
                    - myModel.computeAsScalar(x1_d, x2)
                ) / (2 * eps)
        else:
            gradfd = ot.Matrix(inputDimension, dimension * dimension)
            covarianceX1X2 = myModel(x1, x2)
            centralValue = ot.Point(covarianceX1X2.getImplementation())
            # Loop over the shifted points
            for i in range(inputDimension):
                currentPoint = ot.Point(x1)
                currentPoint[i] += eps
                localCovariance = myModel(currentPoint, x2)
                currentValue = ot.Point(localCovariance.getImplementation())
                for j in range(currentValue.getSize()):
                    gradfd[i, j] = (currentValue[j] - centralValue[j]) / eps

        ott.assert_almost_equal(
            grad, gradfd, 1e-5, 1e-5, "in " + myModel.getClassName() + " grad"
        )

    assert len(repr(myModel)) > 0
    assert len(str(myModel)) > 0
    assert myModel == myModel
    assert not myModel != myModel
    fullParameter = myModel.getFullParameter()
    assert len(myModel.getFullParameterDescription()) == len(fullParameter)
    myModel.setFullParameter(fullParameter)
    ott.assert_almost_equal(myModel.getFullParameter(), fullParameter, 0, 0)
    activeParameter = myModel.getActiveParameter()
    parameter = myModel.getParameter()
    assert len(parameter) == len(activeParameter)
    if len(activeParameter) > 0:
        myModel.setParameter(parameter)
        ott.assert_almost_equal(myModel.getParameter(), parameter, 0, 0)
    if dimension == 1:
        with ott.assert_raises(Exception):
            myModel.computeAsScalar(ot.Point(inputDimension + 1))
    if inputDimension != 1:
        with ott.assert_raises(Exception):
            myModel.computeAsScalar(1.0)
        with ott.assert_raises(Exception):
            myModel.computeAsScalar(1.0, 2.0)


def test_scalar_model(myModel, x1=None, x2=None):
    if x1 is None and x2 is None:
        x1 = 2.0
        x2 = -3.0
    # Check that computeAsScalar(Scalar) == computeAsScalar(Point)
    ott.assert_almost_equal(
        myModel.computeAsScalar([x1], [x2]),
        myModel.computeAsScalar(x1, x2),
        1.0e-14,
        1.0e-14,
    )

    # Gradient testing
    eps = 1e-5

    grad = myModel.partialGradient([x1], [x2])[0, 0]

    x1_g = x1 + eps
    x1_d = x1 - eps
    gradfd = (myModel.computeAsScalar(x1_g, x2) - myModel.computeAsScalar(x1_d, x2)) / (
        2.0 * eps
    )
    ott.assert_almost_equal(gradfd, grad, 1e-5, 1e-5)


inputDimension = 2

# 1) SquaredExponential
myModel = ot.SquaredExponential([2.0], [3.0])
ott.assert_almost_equal(myModel.getScale(), [2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
test_model(myModel)

myModel = ot.SquaredExponential([2.0] * inputDimension, [3.0])
ott.assert_almost_equal(myModel.getScale(), [2, 2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
test_model(myModel)


# 2) GeneralizedExponential
myModel = ot.GeneralizedExponential([2.0], [3.0], 1.5)
ott.assert_almost_equal(myModel.getScale(), [2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
ott.assert_almost_equal(myModel.getP(), 1.5, 0, 0)
test_model(myModel)

myModel = ot.GeneralizedExponential([2.0] * inputDimension, [3.0], 1.5)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
ott.assert_almost_equal(myModel.getP(), 1.5, 0, 0)
test_model(myModel)

# 3) AbsoluteExponential
myModel = ot.AbsoluteExponential([2.0], [3.0])
ott.assert_almost_equal(myModel.getScale(), [2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
test_model(myModel)

myModel = ot.AbsoluteExponential([2.0] * inputDimension, [3.0])
ott.assert_almost_equal(myModel.getScale(), [2, 2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
test_model(myModel)

# 4) MaternModel
myModel = ot.MaternModel([2.0], [3.0], 1.5)
ott.assert_almost_equal(myModel.getScale(), [2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
ott.assert_almost_equal(myModel.getNu(), 1.5, 0, 0)
test_model(myModel)

myModel = ot.MaternModel([2.0] * inputDimension, [3.0], 1.5)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
ott.assert_almost_equal(myModel.getNu(), 1.5, 0, 0)
test_model(myModel)

# Retrieve a copy of the actual implementation from the interface class
myModelInterface = ot.CovarianceModel(myModel)
myModelImplementation = myModelInterface.getImplementation()

# Works because myModelImplementation is a MaternModel
myModelImplementation.setNu(2.5)
ott.assert_almost_equal(myModelImplementation.getNu(), 2.5, 0, 0)

# Original myModel still has the original nu because in Python
# getImplementation clones the underlying implementation
ott.assert_almost_equal(myModel.getNu(), 1.5, 0, 0)

# 5) ExponentiallyDampedCosineModel
myModel = ot.ExponentiallyDampedCosineModel([2.0], [3.0], 1)
ott.assert_almost_equal(myModel.getScale(), [2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
ott.assert_almost_equal(myModel.getFrequency(), 1, 0, 0)
test_model(myModel)
myModel.setFrequency(3)
ott.assert_almost_equal(myModel.getFrequency(), 3, 0, 0)

myModel = ot.ExponentiallyDampedCosineModel([2.0] * inputDimension, [3.0], 1)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
ott.assert_almost_equal(myModel.getFrequency(), 1, 0, 0)
test_model(myModel)

# 6) SphericalModel
myModel = ot.SphericalModel([2.0], [3.0], 4.5)
ott.assert_almost_equal(myModel.getScale(), [2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
ott.assert_almost_equal(myModel.getRadius(), 4.5, 0, 0)
test_model(myModel)

myModel = ot.SphericalModel([2.0] * inputDimension, [3.0], 4.5)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 0, 0)
ott.assert_almost_equal(myModel.getRadius(), 4.5, 0, 0)
test_model(myModel)
myModel.setRadius(1.5)
ott.assert_almost_equal(myModel.getRadius(), 1.5, 0, 0)

defaultModel = ot.SphericalModel()
ott.assert_almost_equal(defaultModel.getScale(), [1.0], 0, 0)
ott.assert_almost_equal(defaultModel.getAmplitude(), [1.0], 0, 0)
ott.assert_almost_equal(defaultModel.getRadius(), 1.0, 0, 0)
assert defaultModel != ot.SquaredExponential([1.0], [1.0])
assert ot.SquaredExponential([1.0], [1.0]) != defaultModel
assert not defaultModel == ot.SquaredExponential([1.0], [1.0])
with ott.assert_raises(Exception):
    ot.SphericalModel([2.0], [3.0], 0.0)
with ott.assert_raises(Exception):
    ot.SphericalModel([2.0], [3.0], -1.0)
with ott.assert_raises(Exception):
    ot.SphericalModel([2.0], [3.0], 4.5).setRadius(0.0)
with ott.assert_raises(Exception):
    ot.SphericalModel([2.0], [3.0], 4.5).setRadius(-2.0)
refModel = ot.SphericalModel([2.0], [3.0], 4.5)
refModel.setNuggetFactor(0.0)
ott.assert_almost_equal(refModel.computeAsScalar([0.0]), 9.0, 1e-14, 1e-14)
ott.assert_almost_equal(refModel.computeAsScalar(0.0), 9.0, 1e-14, 1e-14)
ott.assert_almost_equal(refModel.computeAsScalar([4.5]), 2.8125, 1e-14, 1e-14)
ott.assert_almost_equal(refModel.computeAsScalar(4.5), 2.8125, 1e-14, 1e-14)
ott.assert_almost_equal(refModel.computeAsScalar([9.0]), 0.0, 1e-14, 1e-14)
ott.assert_almost_equal(refModel.computeAsScalar(20.0), 0.0, 1e-14, 1e-14)
ott.assert_almost_equal(refModel.computeAsScalar([2.0], [2.0]), 9.0, 1e-14, 1e-14)
ott.assert_almost_equal(refModel.computeAsScalar(2.0, -8.0), 0.0, 1e-14, 1e-14)
ott.assert_almost_equal(refModel.computeAsScalar([2.0], [-2.5]), 2.8125, 1e-14, 1e-14)
nuggetModel = ot.SphericalModel([2.0], [3.0], 4.5)
nuggetModel.setNuggetFactor(0.5)
ott.assert_almost_equal(nuggetModel.computeAsScalar([0.0]), 13.5, 1e-14, 1e-14)
ott.assert_almost_equal(nuggetModel.computeAsScalar(0.0), 13.5, 1e-14, 1e-14)
ott.assert_almost_equal(nuggetModel.computeAsScalar([0.0], [0.0]), 13.5, 1e-14, 1e-14)
model2d = ot.SphericalModel([2.0, 2.0], [3.0], 4.5)
model2d.setNuggetFactor(0.0)
ott.assert_almost_equal(model2d.computeAsScalar([0.0, 0.0]), 9.0, 1e-14, 1e-14)
ott.assert_almost_equal(model2d.computeAsScalar([0.0, 0.0], [0.0, 0.0]), 9.0, 1e-14, 1e-14)
ott.assert_almost_equal(model2d.computeAsScalar([20.0, 20.0]), 0.0, 1e-14, 1e-14)
fp = refModel.getFullParameter()
ott.assert_almost_equal(fp, [2.0, 0.0, 3.0, 4.5], 0, 0)
assert refModel.getFullParameterDescription() == ["scale_0", "nuggetFactor", "amplitude_0", "radius"]
refModel.setFullParameter([1.0, 0.1, 2.0, 3.0])
ott.assert_almost_equal(refModel.getScale(), [1.0], 0, 0)
ott.assert_almost_equal(refModel.getAmplitude(), [2.0], 0, 0)
ott.assert_almost_equal(refModel.getRadius(), 3.0, 0, 0)
ott.assert_almost_equal(refModel.getNuggetFactor(), 0.1, 0, 0)
ott.assert_almost_equal(refModel.getFullParameter(), [1.0, 0.1, 2.0, 3.0], 0, 0)
with ott.assert_raises(Exception):
    refModel.setFullParameter([1.0, 0.1, 2.0, 0.0])
with ott.assert_raises(Exception):
    refModel.setFullParameter([1.0, 0.1, 2.0, -1.0])
model2dfp = ot.SphericalModel([2.0, 3.0], [4.0], 5.0)
ott.assert_almost_equal(model2dfp.getFullParameter(), [2.0, 3.0, 1e-12, 4.0, 5.0], 1e-14, 1e-14)
assert model2dfp.getFullParameterDescription() == ["scale_0", "scale_1", "nuggetFactor", "amplitude_0", "radius"]
model2dfp.setFullParameter([2.5, 3.5, 0.01, 4.5, 5.5])
ott.assert_almost_equal(model2dfp.getFullParameter(), [2.5, 3.5, 0.01, 4.5, 5.5], 0, 0)
ott.assert_almost_equal(model2dfp.getRadius(), 5.5, 0, 0)
paramModel = ot.SphericalModel([2.0], [3.0], 4.5)
ott.assert_almost_equal(paramModel.getParameter(), [2.0, 3.0], 0, 0)
assert paramModel.getParameterDescription() == ["scale_0", "amplitude_0"]
paramModel.setParameter([1.0, 2.0])
ott.assert_almost_equal(paramModel.getFullParameter(), [1.0, 1e-12, 2.0, 4.5], 0, 0)
paramModel.setActiveParameter([0, 1, 2, 3])
ott.assert_almost_equal(paramModel.getParameter(), [1.0, 1e-12, 2.0, 4.5], 0, 0)
paramModel.setParameter([1.0, 0.02, 2.0, 3.0])
ott.assert_almost_equal(paramModel.getFullParameter(), [1.0, 0.02, 2.0, 3.0], 0, 0)
storage = ot.XMLStorageManager("t_CovarianceModel_std_Spherical.xml")
study = ot.Study()
study.setStorageManager(storage)
study.add("model", ot.CovarianceModel(paramModel))
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("t_CovarianceModel_std_Spherical.xml"))
loadedStudy.load()
loadedModel = ot.CovarianceModel()
loadedStudy.fillObject("model", loadedModel)
assert loadedModel == ot.CovarianceModel(paramModel)
os.remove("t_CovarianceModel_std_Spherical.xml")

# 7) FractionalBrownianMotionModel
myModel = ot.FractionalBrownianMotionModel([2.0], [3.0], [0.25])
test_model(myModel)

# 8) DiracCovarianceModel
myModel = ot.DiracCovarianceModel()
# Should not check the partialGradient Dirac model
test_model(myModel, test_partial_grad=False)

amplitude = [1.5 + 2.0 * k for k in range(2)]
dimension = 2
spatialCorrelation = ot.CorrelationMatrix(dimension)
for j in range(dimension):
    for i in range(j + 1, dimension):
        spatialCorrelation[i, j] = (i + 1.0) / dimension - (j + 1.0) / dimension
myModel = ot.DiracCovarianceModel(inputDimension, amplitude, spatialCorrelation)
ott.assert_almost_equal(myModel.getScale(), [1, 1], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), amplitude, 0, 0)
test_model(myModel, test_partial_grad=False, x1=[0.5, 0.0], x2=[0.5, 0.0])

# 9) StationaryFunctionalCovarianceModel
rho = ot.SymbolicFunction(["tau"], ["exp(-abs(tau))*cos(2*pi_*abs(tau))"])
myModel = ot.StationaryFunctionalCovarianceModel([1.0], [1.0], rho)
ott.assert_almost_equal(myModel.getScale(), [1], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [1], 0, 0)
test_model(myModel)

# 10) ProductCovarianceModel
myModel = ot.ProductCovarianceModel()
test_model(myModel)

cov1 = ot.AbsoluteExponential([2.0], [3.0])
cov2 = ot.SquaredExponential([2.0], [3.0])
myModel = ot.ProductCovarianceModel([cov1, cov2])
test_model(myModel)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [9], 0, 0)
point = [0.50, -6]
x = [point[0]]
y = [point[1]]
ott.assert_almost_equal(
    myModel.computeAsScalar(point),
    cov1.computeAsScalar(x) * cov2.computeAsScalar(y),
    1.0e-15,
    1.0e-15,
)

# 11) TensorizedCovarianceModel

# Collection ==> add covariance models
myAbsoluteExponential = ot.AbsoluteExponential([2.0] * inputDimension, [3.0])
mySquaredExponential = ot.SquaredExponential([2.0] * inputDimension, [3.0])
myGeneralizedExponential = ot.GeneralizedExponential([2.0] * inputDimension, [3.0], 1.5)
# Build TensorizedCovarianceModel with scale = [1,..,1]
# Tensorized ignore scales
myModel = ot.TensorizedCovarianceModel(
    [myAbsoluteExponential, mySquaredExponential, myGeneralizedExponential]
)
ott.assert_almost_equal(myModel.getScale(), [1, 1], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3, 3, 3], 0, 0)
test_model(myModel)

# Define new scale
scale = [2.5, 1.5]
myModel.setScale(scale)
ott.assert_almost_equal(myModel.getScale(), [2.5, 1.5], 0, 0)
ott.assert_almost_equal(myModel.getAmplitude(), [3, 3, 3], 0, 0)
test_model(myModel)

# new test for tensorized covariance model
output_dimension = 1  # 2 is ok

f = ot.SymbolicFunction(["x"], ["x * sin(x)"] * output_dimension)
sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0], [7.0], [8.0]]
sampleY = f(sampleX)

basis = ot.Basis(
    [ot.SymbolicFunction(["x"], ["x"]), ot.SymbolicFunction(["x"], ["x^2"])]
)
covarianceModel = ot.TensorizedCovarianceModel(
    [ot.SquaredExponential([1.0]) for _ in range(output_dimension)]
)
algo = ot.GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
lh = algo.getReducedLogLikelihoodFunction()
# Using 1d graph we get the optimum around 1.5625
max_lh = lh([1.5625])
ott.assert_almost_equal(max_lh, [-14.1698], 1e-4, 1e-4)


# 12) Testing 1d in/out dimension & stationary

# Test scalar input models
coll = [ot.AbsoluteExponential(), ot.SquaredExponential(), ot.GeneralizedExponential()]
coll += [ot.MaternModel(), ot.SphericalModel(), ot.ExponentiallyDampedCosineModel()]
for model in coll:
    test_scalar_model(model)

# 13) Isotropic covariance model

myIsotropicKernel = ot.IsotropicCovarianceModel()
test_model(myIsotropicKernel)

scale = 3.5
amplitude = 1.5
myOneDimensionalKernel = ot.SquaredExponential([scale], [amplitude])
myIsotropicKernel = ot.IsotropicCovarianceModel(myOneDimensionalKernel, inputDimension)

# Test consistency of isotropic model with underlying 1D kernel
ott.assert_almost_equal(myIsotropicKernel.getAmplitude()[0], amplitude, 1e-12, 0.0)
ott.assert_almost_equal(myIsotropicKernel.getScale()[0], scale, 1e-12, 0.0)
ott.assert_almost_equal(
    myIsotropicKernel.getKernel().getAmplitude()[0], amplitude, 1e-12, 0.0
)
ott.assert_almost_equal(myIsotropicKernel.getKernel().getScale()[0], scale, 1e-12, 0.0)

# Standard tests applied
test_model(myIsotropicKernel)

# Test consistency of isotropic kernel's discretization
inputVector = ot.Point([0.3, 1.7])
inputVectorNorm = ot.Point([inputVector.norm()])
ott.assert_almost_equal(
    myOneDimensionalKernel(inputVectorNorm)[0, 0], 1.992315565746, 1e-12, 0.0
)
ott.assert_almost_equal(
    myIsotropicKernel(inputVector)[0, 0], 1.992315565746, 1e-12, 0.0
)
inputSample = ot.Sample([ot.Point(2), inputVector])
inputSampleNorm = ot.Sample([ot.Point(1), inputVectorNorm])
oneDimensionalCovMatrix = myOneDimensionalKernel.discretize(inputSampleNorm)
isotropicCovMatrix = myIsotropicKernel.discretize(inputSample)
ott.assert_almost_equal(oneDimensionalCovMatrix[0, 0], 2.250000000002, 1e-12, 0.0)
ott.assert_almost_equal(oneDimensionalCovMatrix[1, 1], 2.250000000002, 1e-12, 0.0)
ott.assert_almost_equal(isotropicCovMatrix[0, 0], 2.250000000002, 1e-12, 0.0)
ott.assert_almost_equal(isotropicCovMatrix[1, 1], 2.250000000002, 1e-12, 0.0)
ott.assert_almost_equal(oneDimensionalCovMatrix[0, 1], 1.992315565746, 1e-12, 0.0)
ott.assert_almost_equal(isotropicCovMatrix[0, 1], 1.992315565746, 1e-12, 0.0)

# Exponential covariance model
inputDimension = 2
scale = [4, 5]
spatialCovariance = ot.CovarianceMatrix(inputDimension)
spatialCovariance[0, 0] = 4
spatialCovariance[1, 1] = 5
spatialCovariance[1, 0] = 1.2
myModel = ot.ExponentialModel(scale, spatialCovariance)
test_model(myModel)
# assert that spatialCovariance is taken into account
checkDiag = spatialCovariance.isDiagonal() == myModel.isDiagonal()
if not checkDiag:
    raise Exception("isDiagonal differ between spatial covariance & covariance model")
rho = spatialCovariance[1, 0] / sqrt(spatialCovariance[0, 0] * spatialCovariance[1, 1])
ott.assert_almost_equal(
    myModel.getOutputCorrelation()[0, 1], rho, 0, 0, "in ExponentialModel correlation"
)

# Kronecker covariance model

# rho correlation
scale = [4, 5]
rho = ot.GeneralizedExponential(scale, 1)

# Amplitude values
amplitude = [1, 2]
myModel = ot.KroneckerCovarianceModel(rho, amplitude)
test_model(myModel)

outputCorrelation = ot.CorrelationMatrix(2)
outputCorrelation[0, 1] = 0.8
myModel = ot.KroneckerCovarianceModel(rho, amplitude, outputCorrelation)
test_model(myModel)

outputCovariance = ot.CovarianceMatrix(2)
outputCovariance[0, 0] = 4.0
outputCovariance[1, 1] = 5.0
outputCovariance[1, 0] = 1.2

myModel = ot.KroneckerCovarianceModel(rho, outputCovariance)
test_model(myModel)

# New kronecker model involving isotropic cov model
rho = ot.IsotropicCovarianceModel(ot.MaternModel(), 3)
outputCorrelation = ot.CorrelationMatrix(2)
outputCorrelation[0, 1] = 0.8
amplitude = [1, 2]
myModel = ot.KroneckerCovarianceModel(rho, amplitude, outputCorrelation)
test_model(myModel)
ott.assert_almost_equal(
    myModel.getInputDimension(), 3, 0, 0, "in kronecker dimension check"
)
ott.assert_almost_equal(myModel.getScale(), [1], 0, 0, "in kronecker scale check")
# full param size = 6 (scale(1), nuggetFactor(1), amplitude(2), spatialCorrelation(1), Matern nu(1))
ott.assert_almost_equal(
    myModel.getFullParameter(),
    [1, 1e-12, 1, 2, 0.8, 1.5],
    0,
    0,
    "in kronecker full param check",
)
ott.assert_almost_equal(
    myModel.getFullParameter().getSize(), 6, 0, 0, "in kronecker param size check"
)
ott.assert_almost_equal(
    myModel.getFullParameterDescription().getSize(),
    6,
    0,
    0,
    "in kronecker param description size check",
)
assert myModel.getActiveParameter() == [0, 2, 3], "in kronecker active param check"
myModel.setFullParameter([2, 0.01, 1, 2, 0.5, 2.5])
ott.assert_almost_equal(
    myModel.getFullParameter(),
    [2, 0.01, 1, 2, 0.5, 2.5],
    0,
    0,
    "in kronecker param check",
)
myModel.setActiveParameter([0, 1, 2, 3, 5])
assert myModel.getActiveParameter() == [
    0,
    1,
    2,
    3,
    5,
], "in kronecker active param check"
# Now we should get all values except correlation
ott.assert_almost_equal(
    myModel.getParameter(), [2, 0.01, 1, 2, 2.5], 0, 0, "in kronecker param check"
)
myModel.activateAmplitude(False)
ott.assert_almost_equal(
    myModel.getParameter(),
    [2, 0.01, 2.5],
    0,
    0,
    "in kronecker deactivate amplitude check",
)
myModel.activateScale(False)
ott.assert_almost_equal(
    myModel.getParameter(), [0.01, 2.5], 0, 0, "in kronecker deactivate scale check"
)
myModel.activateNuggetFactor(False)
ott.assert_almost_equal(
    myModel.getParameter(), [2.5], 0, 0, "in kronecker deactivate nuggetFactor check"
)
myModel.activateScale(True)
ott.assert_almost_equal(
    myModel.getParameter(), [2, 2.5], 0, 0, "in kronecker activate scale check"
)
myModel.activateNuggetFactor(True)
ott.assert_almost_equal(
    myModel.getParameter(),
    [2, 0.01, 2.5],
    0,
    0,
    "in kronecker activate nuggetFactor check",
)
myModel.activateAmplitude(True)
ott.assert_almost_equal(
    myModel.getParameter(),
    [2, 0.01, 1, 2, 2.5],
    0,
    0,
    "in kronecker activate amplitude check",
)
assert myModel.getFullParameterDescription() == [
    "scale_0",
    "nuggetFactor",
    "amplitude_0",
    "amplitude_1",
    "R_1_0",
    "nu",
]

# 14) equality tests
# SquaredExponential
assert ot.SquaredExponential([1.0], [2.0]) == ot.SquaredExponential([1.0], [2.0]), "SE==SE"
assert not ot.SquaredExponential([1.0], [2.0]) == ot.SquaredExponential([2.0], [2.0]), "SE!=SE scale"
assert not ot.SquaredExponential([1.0], [2.0]) == ot.SquaredExponential([1.0], [3.0]), "SE!=SE amplitude"
assert not ot.SquaredExponential([1.0], [2.0]) != ot.SquaredExponential([1.0], [2.0]), "SE!=SE neg"
assert ot.SquaredExponential([1.0], [2.0]) != ot.ExponentialModel([1.0], [2.0]), "SE!=Exp"

# ExponentialModel
assert ot.ExponentialModel([1.0], [2.0]) == ot.ExponentialModel([1.0], [2.0]), "Exp==Exp"
assert not ot.ExponentialModel([1.0], [2.0]) == ot.SquaredExponential([1.0], [2.0]), "Exp!=SE"
assert ot.ExponentialModel([1.0], [3.0]) != ot.ExponentialModel([1.0], [2.0]), "Exp!=Exp amplitude"

# AbsoluteExponential
assert ot.AbsoluteExponential([1.0], [2.0]) == ot.AbsoluteExponential([1.0], [2.0]), "AE==AE"
assert ot.AbsoluteExponential([1.0], [2.0]) != ot.SquaredExponential([1.0], [2.0]), "AE!=SE"

# GeneralizedExponential
assert ot.GeneralizedExponential([1.0], [2.0], 1.5) == ot.GeneralizedExponential([1.0], [2.0], 1.5), "GE==GE"
assert ot.GeneralizedExponential([1.0], [2.0], 1.5) != ot.GeneralizedExponential([1.0], [2.0], 2.0), "GE!=GE p"

# MaternModel
assert ot.MaternModel([1.0], [2.0], 1.5) == ot.MaternModel([1.0], [2.0], 1.5), "MM==MM"
assert ot.MaternModel([1.0], [2.0], 1.5) != ot.MaternModel([1.0], [2.0], 2.5), "MM!=MM nu"

# ExponentiallyDampedCosineModel
assert ot.ExponentiallyDampedCosineModel([1.0], [2.0], 0.5) == ot.ExponentiallyDampedCosineModel([1.0], [2.0], 0.5), "EDCM==EDCM"
assert ot.ExponentiallyDampedCosineModel([1.0], [2.0], 0.5) != ot.ExponentiallyDampedCosineModel([1.0], [2.0], 1.0), "EDCM!=EDCM freq"

# SphericalModel
assert ot.SphericalModel([1.0], [2.0], 3.0) == ot.SphericalModel([1.0], [2.0], 3.0), "SM==SM"
assert ot.SphericalModel([1.0], [2.0], 3.0) != ot.SphericalModel([1.0], [2.0], 4.0), "SM!=SM radius"

# FractionalBrownianMotionModel
assert ot.FractionalBrownianMotionModel([2.0], [3.0], [0.25]) == ot.FractionalBrownianMotionModel([2.0], [3.0], [0.25]), "FBMM==FBMM"
assert ot.FractionalBrownianMotionModel([2.0], [3.0], [0.25]) != ot.FractionalBrownianMotionModel([3.0], [3.0], [0.25]), "FBMM!=FBMM scale"

# DiracCovarianceModel
assert ot.DiracCovarianceModel(2, [1.0, 2.0]) == ot.DiracCovarianceModel(2, [1.0, 2.0]), "DCM==DCM"
assert ot.DiracCovarianceModel(2, [1.0, 2.0]) != ot.DiracCovarianceModel(2, [3.0, 2.0]), "DCM!=DCM amplitude"

# IsotropicCovarianceModel
kernel1 = ot.SquaredExponential([1.0], [2.0])
kernel2 = ot.SquaredExponential([1.0], [2.0])
assert ot.IsotropicCovarianceModel(kernel1, 2) == ot.IsotropicCovarianceModel(kernel2, 2), "ICM==ICM"
assert ot.IsotropicCovarianceModel(kernel1, 2) != ot.IsotropicCovarianceModel(ot.MaternModel([1.0], [2.0], 1.5), 2), "ICM!=ICM kernel"

# ProductCovarianceModel
coll1 = [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([3.0], [4.0])]
coll2 = [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([3.0], [4.0])]
assert ot.ProductCovarianceModel(coll1) == ot.ProductCovarianceModel(coll2), "PCM==PCM"
coll3 = [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([5.0], [4.0])]
assert ot.ProductCovarianceModel(coll1) != ot.ProductCovarianceModel(coll3), "PCM!=PCM coll"

# TensorizedCovarianceModel
coll1 = [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([1.0], [3.0])]
coll2 = [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([1.0], [3.0])]
assert ot.TensorizedCovarianceModel(coll1) == ot.TensorizedCovarianceModel(coll2), "TCM==TCM"
coll3 = [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([1.0], [4.0])]
assert ot.TensorizedCovarianceModel(coll1) != ot.TensorizedCovarianceModel(coll3), "TCM!=TCM coll"

# KroneckerCovarianceModel
rho1 = ot.GeneralizedExponential([1.0], 1.0)
rho2 = ot.GeneralizedExponential([1.0], 1.0)
assert ot.KroneckerCovarianceModel(rho1, [1.0, 2.0]) == ot.KroneckerCovarianceModel(rho2, [1.0, 2.0]), "KCM==KCM"
rho3 = ot.GeneralizedExponential([2.0], 1.0)
assert ot.KroneckerCovarianceModel(rho1, [1.0, 2.0]) != ot.KroneckerCovarianceModel(rho3, [1.0, 2.0]), "KCM!=KCM rho"

# StationaryFunctionalCovarianceModel
f1 = ot.SymbolicFunction(["tau"], ["exp(-abs(tau))"])
f2 = ot.SymbolicFunction(["tau"], ["exp(-abs(tau))"])
assert ot.StationaryFunctionalCovarianceModel([1.0], [2.0], f1) == ot.StationaryFunctionalCovarianceModel([1.0], [2.0], f2), "SFCM==SFCM"
f3 = ot.SymbolicFunction(["tau"], ["exp(-2*abs(tau))"])
assert ot.StationaryFunctionalCovarianceModel([1.0], [2.0], f1) != ot.StationaryFunctionalCovarianceModel([1.0], [2.0], f3), "SFCM!=SFCM rho"

# Self equality
se = ot.SquaredExponential([1.0], [2.0])
assert se == se, "self=="
assert not se != se, "self!="

# Nugget factor equality
assert ot.SquaredExponential([1.0], [2.0]) == ot.SquaredExponential([1.0], [2.0]), "SE==SE no nugget"
with_nugget = ot.SquaredExponential([1.0], [2.0])
with_nugget.setNuggetFactor(0.1)
assert with_nugget != ot.SquaredExponential([1.0], [2.0]), "SE!=SE nugget"

# DiracCovarianceModel extra coverage
dirac = ot.DiracCovarianceModel(1, [2.0])
assert dirac.isStationary()
assert abs(dirac([0.0])[0, 0] - 4.0) < 1e-8
assert abs(dirac([1.0])[0, 0]) < 1e-12
assert abs(dirac([0.0], [0.0])[0, 0] - 4.0) < 1e-8
assert abs(dirac([0.0], [1.0])[0, 0]) < 1e-12
assert abs(dirac.computeAsScalar([0.0]) - 4.0) < 1e-8
assert abs(dirac.computeAsScalar([1.0])) < 1e-12
assert abs(dirac.computeAsScalar([0.0], [0.0]) - 4.0) < 1e-8
assert abs(dirac.computeAsScalar([0.0], [1.0])) < 1e-12
assert abs(dirac.computeAsScalar(0.0) - 4.0) < 1e-8
assert abs(dirac.computeAsScalar(1.0)) < 1e-12
_ = dirac.partialGradient([0.0], [1.0])
ott.assert_almost_equal(dirac.getFullParameter(), [1.0, 0.0, 2.0], 1e-12, 1e-12)
assert len(dirac.getFullParameterDescription()) == 3
dirac.setFullParameter([1.0, 0.0, 3.0])
ott.assert_almost_equal(dirac.getAmplitude(), [3.0], 1e-12, 1e-12)
with ott.assert_raises(Exception):
    dirac.setFullParameter([1.0, 0.0])
dirac.setScale([5.0])
ott.assert_almost_equal(dirac.getScale(), [5.0], 1e-14, 1e-14)
with ott.assert_raises(Exception):
    dirac.setAmplitude([1.0, 2.0])
with ott.assert_raises(Exception):
    dirac.setAmplitude([-1.0])
with ott.assert_raises(Exception):
    dirac.setOutputCorrelation(ot.CorrelationMatrix(2))
with ott.assert_raises(Exception):
    dirac.computeAsScalar([0.0, 0.0])
cov1 = ot.CovarianceMatrix(1)
cov1[0, 0] = 4.0
dirac2 = ot.DiracCovarianceModel(1, cov1)
assert abs(dirac2([0.0])[0, 0] - 4.0) < 1e-8
_ = repr(dirac)
_ = str(dirac)
assert dirac == dirac
assert not (dirac != dirac)
# discretize error branches
with ott.assert_raises(Exception):
    dirac.discretize(ot.Sample([[0.0, 1.0]]))
with ott.assert_raises(Exception):
    dirac.discretize(ot.Sample(0, 1))
with ott.assert_raises(Exception):
    dirac.discretizeRow(ot.Sample([[0.0, 1.0]]), 0)
with ott.assert_raises(Exception):
    dirac.discretizeRow(ot.Sample(0, 1), 0)
with ott.assert_raises(Exception):
    dirac.discretizeRow(ot.Sample([[0.0], [1.0]]), 5)
# repeated points trigger generic path when CheckUnique is on
ot.ResourceMap.SetAsBool("DiracCovarianceModel-CheckUnique", True)
dup = ot.Sample([[0.0], [0.0], [1.0]])
_ = dirac.discretize(dup)
with ott.assert_raises(Exception):
    dirac.discretizeAndFactorize(dup)
_ = dirac.discretizeRow(ot.Sample([[0.0], [1.0]]), 0)
ot.ResourceMap.SetAsBool("DiracCovarianceModel-CheckUnique", False)
# computeAsScalar with output dim > 1 must raise
diracMV = ot.DiracCovarianceModel(1, [1.0, 2.0])
with ott.assert_raises(Exception):
    diracMV.computeAsScalar([0.0])
with ott.assert_raises(Exception):
    diracMV.computeAsScalar([0.0], [0.0])
with ott.assert_raises(Exception):
    diracMV.computeAsScalar(0.0)
# non-diagonal covariance ctor
cov = ot.CovarianceMatrix(2)
cov[0, 0] = 4.0
cov[1, 1] = 9.0
cov[1, 0] = 1.0
_ = ot.DiracCovarianceModel(1, cov)

# --- Reduced state of the evaluation kernels survives a save/load round trip ---
# The scalar evaluation entry points of the models cache reduced quantities (the
# closed form selected by the Matern smoothness, the inverse scales of the
# exponential model). They are rebuilt by the constructors, by the setters and
# by load(); a forgotten refresh silently evaluates a wrong kernel afterwards.
# The check goes through a HODLR assembly because that is the path which uses
# the reduced state, and compares the solves with the ones of a freshly built
# model.
# Short correlation lengths on the unit square, so that the assembled problem
# stays well conditioned and the comparison of the solves is meaningful
models = [
    ot.MaternModel([0.1, 0.15], [2.0], 2.5),
    ot.MaternModel([0.1, 0.15], [2.0], 1.5),
    ot.MaternModel([0.1, 0.15], [2.0], 0.5),
    ot.MaternModel([0.1, 0.15], [2.0], 3.7),  # general (Bessel) form
    ot.ExponentialModel([0.1, 0.15], [2.0]),
    ot.IsotropicCovarianceModel(ot.MaternModel([0.12], [2.0], 2.5), 2),
    ot.SquaredExponential([0.3, 0.35], [2.0]),
]
vertices = ot.IntervalMesher([15, 15]).build(ot.Interval([0.0, 0.0], [1.0, 1.0])).getVertices()
parameters = otexp.HODLRMatrixParameters()
parameters.setMinLeafSize(32)
rhs = ot.Point(vertices.getSize())
for i in range(vertices.getSize()):
    rhs[i] = 1.0 + 0.01 * i
for model in models:
    for nugget in [0.0, 1e-6]:
        model.setNuggetFactor(nugget)
        reference = ot.CovarianceModel(model)
        with tempfile.TemporaryDirectory() as directory:
            fileName = os.path.join(directory, "model.xml")
            study = ot.Study()
            study.setStorageManager(ot.XMLStorageManager(fileName))
            study.add("model", model)
            study.save()
            study = ot.Study()
            study.setStorageManager(ot.XMLStorageManager(fileName))
            study.load()
            loaded = ot.CovarianceModel()
            study.fillObject("model", loaded)
        assert loaded == reference, "reduced kernel state lost by save/load"
        hodlrLoaded = reference.discretizeHODLRMatrix(vertices, parameters)
        hodlrLoaded.factorize()
        hodlrReference = loaded.discretizeHODLRMatrix(vertices, parameters)
        hodlrReference.factorize()
        assert hodlrLoaded.getRegularizationShift() == hodlrReference.getRegularizationShift()
        if nugget > 0.0:
            # The comparison below is only meaningful on a well conditioned
            # problem, i.e. without a large healing shift
            assert hodlrLoaded.getRegularizationShift() < 1.0e-3, hodlrLoaded.getRegularizationShift()
        ott.assert_almost_equal(
            ot.Point(hodlrReference.solve(rhs)),
            ot.Point(hodlrLoaded.solve(rhs)),
            1.0e-12,
            1.0e-12,
        )
print("Study round trip of the reduced kernel state: PASS")

# KroneckerCovarianceModel extra coverage: constructors and error branches
kroneckerRho = ot.GeneralizedExponential([4.0, 5.0], 1.0)
kroneckerDim = ot.KroneckerCovarianceModel(2)
assert kroneckerDim.getInputDimension() == 2
assert kroneckerDim.getOutputDimension() == 1
kronecker1D = ot.KroneckerCovarianceModel(
    ot.GeneralizedExponential([1.0], 1.0), [2.0]
)
assert kronecker1D.isParallel()
ott.assert_almost_equal(
    kronecker1D.computeAsScalar([0.0], [0.0]),
    kronecker1D.computeAsScalar([0.0]),
    1e-14,
    1e-14,
)
with ott.assert_raises(Exception):
    ot.KroneckerCovarianceModel(ot.ExponentialModel([1.0], [1.0, 2.0]), [1.0, 2.0])
zeroDiag = ot.CovarianceMatrix(2)
zeroDiag[0, 0] = 4.0
zeroDiag[1, 1] = 0.0
with ott.assert_raises(Exception):
    ot.KroneckerCovarianceModel(kroneckerRho, zeroDiag)
diagSigma = ot.CovarianceMatrix(2)
diagSigma[0, 0] = 4.0
diagSigma[1, 1] = 9.0
kroneckerDiag = ot.KroneckerCovarianceModel(kroneckerRho, diagSigma)
ott.assert_almost_equal(kroneckerDiag.getAmplitude(), [2.0, 3.0], 0, 0)
assert kroneckerDiag.isDiagonal()
assert len(repr(kroneckerDiag)) > 0
assert len(str(kroneckerDiag)) > 0
with ott.assert_raises(Exception):
    kronecker1D.discretize(ot.Sample([[0.0, 0.0]]))
with ott.assert_raises(Exception):
    kronecker1D.partialGradient([0.0, 0.0], [0.0])
with ott.assert_raises(Exception):
    kronecker1D.computeAsScalar([0.0, 0.0], [0.0])
with ott.assert_raises(Exception):
    kroneckerDiag.computeAsScalar([0.0, 0.0])
with ott.assert_raises(Exception):
    kronecker1D.setScale([1.0, 2.0])
with ott.assert_raises(Exception):
    kronecker1D.setScale([-1.0])
with ott.assert_raises(Exception):
    kronecker1D.setNuggetFactor(-1.0)
with ott.assert_raises(Exception):
    kronecker1D.setFullParameter([1.0])
with ott.assert_raises(Exception):
    kronecker1D.setFullParameter([1.0, 0.0, -2.0])
factorized = kroneckerDiag.discretizeAndFactorize(
    ot.Sample([[0.0, 0.0], [1.0, 1.0]])
)
assert factorized.getDimension() == 4
kroneckerStudy = ot.Study()
kroneckerStudy.setStorageManager(
    ot.XMLStorageManager("t_CovarianceModel_std_Kronecker.xml")
)
kroneckerStudy.add("model", ot.CovarianceModel(kronecker1D))
kroneckerStudy.save()
kroneckerLoadedStudy = ot.Study()
kroneckerLoadedStudy.setStorageManager(
    ot.XMLStorageManager("t_CovarianceModel_std_Kronecker.xml")
)
kroneckerLoadedStudy.load()
kroneckerLoaded = ot.CovarianceModel()
kroneckerLoadedStudy.fillObject("model", kroneckerLoaded)
assert kroneckerLoaded == ot.CovarianceModel(kronecker1D)
os.remove("t_CovarianceModel_std_Kronecker.xml")

# TensorizedCovarianceModel extra coverage: constructors and error branches
tensorizedDefault = ot.TensorizedCovarianceModel(1)
assert tensorizedDefault.getOutputDimension() == 1
tensorized2 = ot.TensorizedCovarianceModel(2)
assert tensorized2.getOutputDimension() == 2
assert tensorized2.getAmplitude() == [1.0, 1.0]
assert tensorized2.getCollection().getSize() == 2
tensorized3 = ot.TensorizedCovarianceModel(3)
assert tensorized3.getOutputDimension() == 3
assert tensorizedDefault.isStationary()
assert tensorizedDefault.isDiagonal()
assert tensorizedDefault.isParallel()
assert tensorizedDefault.getCollection().getSize() == 1
assert tensorizedDefault.getMarginal(0).getOutputDimension() == 1
with ott.assert_raises(Exception):
    tensorizedDefault.getMarginal(5)
with ott.assert_raises(Exception):
    ot.TensorizedCovarianceModel([])
with ott.assert_raises(Exception):
    ot.TensorizedCovarianceModel(
        [ot.AbsoluteExponential([1.0], [1.0]),
         ot.AbsoluteExponential([1.0, 2.0], [1.0])]
    )
with ott.assert_raises(Exception):
    ot.TensorizedCovarianceModel(
        [ot.ExponentialModel([1.0], [1.0, 2.0]),
         ot.AbsoluteExponential([1.0], [1.0])]
    )
tensorized1D = ot.TensorizedCovarianceModel(
    [ot.AbsoluteExponential([1.0], [1.0])]
)
ott.assert_almost_equal(
    tensorized1D.computeAsScalar([0.0], [0.0]),
    tensorized1D.computeAsScalar([0.0]),
    1e-14,
    1e-14,
)
ott.assert_almost_equal(
    tensorized1D.computeAsScalar([0.0], [0.0]),
    tensorized1D.computeAsScalar(0.0),
    1e-14,
    1e-14,
)
assert tensorized1D.partialGradient([0.0], [1.0]).getNbRows() == 1
tensorized2D = ot.TensorizedCovarianceModel(
    [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([1.0], [3.0])]
)
with ott.assert_raises(Exception):
    tensorized2D.computeAsScalar([0.0], [0.0])
with ott.assert_raises(Exception):
    tensorized2D([0.0, 1.0], [0.0])
with ott.assert_raises(Exception):
    tensorized2D.setScale([1.0])
with ott.assert_raises(Exception):
    tensorized2D.setAmplitude([1.0])
with ott.assert_raises(Exception):
    tensorized2D.setFullParameter([1.0])
tensorizedScaled = ot.TensorizedCovarianceModel(
    [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([1.0], [3.0])],
    [2.0],
)
ott.assert_almost_equal(tensorizedScaled.getScale(), [2.0], 0, 0)
assert len(repr(tensorized2D)) > 0
assert len(str(tensorized2D)) > 0
assert tensorized2D == ot.TensorizedCovarianceModel(
    [ot.AbsoluteExponential([1.0], [2.0]), ot.SquaredExponential([1.0], [3.0])]
)
tensorizedStudy = ot.Study()
tensorizedStudy.setStorageManager(
    ot.XMLStorageManager("t_CovarianceModel_std_Tensorized.xml")
)
tensorizedStudy.add("model", ot.CovarianceModel(tensorized2D))
tensorizedStudy.save()
tensorizedLoadedStudy = ot.Study()
tensorizedLoadedStudy.setStorageManager(
    ot.XMLStorageManager("t_CovarianceModel_std_Tensorized.xml")
)
tensorizedLoadedStudy.load()
tensorizedLoaded = ot.CovarianceModel()
tensorizedLoadedStudy.fillObject("model", tensorizedLoaded)
assert tensorizedLoaded == ot.CovarianceModel(tensorized2D)
os.remove("t_CovarianceModel_std_Tensorized.xml")

# 15) RankMCovarianceModel
# default ctor
defaultRankM = ot.RankMCovarianceModel()
assert defaultRankM.getInputDimension() == 1
assert defaultRankM.getOutputDimension() == 1

basis = ot.Basis(
    [ot.SymbolicFunction(["x"], ["1.0"]), ot.SymbolicFunction(["x"], ["x"])]
)
# variance ctor
rankM = ot.RankMCovarianceModel([2.0, 3.0], basis)
ott.assert_almost_equal(rankM.getVariance(), [2.0, 3.0], 0, 0)
assert rankM.getCovariance().getDimension() == 0
assert len(rankM.getBasis()) == 2
assert len(rankM.getFunctions()) == 2
assert rankM.getInputDimension() == 1
assert rankM.getOutputDimension() == 1
# operator() against the closed form sum_i variance[i] * phi_i(s) * phi_i(t)
ott.assert_almost_equal(
    rankM([0.5], [0.25])[0, 0], 2.0 + 3.0 * 0.5 * 0.25, 1e-12, 1e-12
)
# diagonal covariance ctor behaves like the variance ctor
diagCov = ot.CovarianceMatrix(2)
diagCov[0, 0] = 2.0
diagCov[1, 1] = 3.0
rankMDiag = ot.RankMCovarianceModel(diagCov, basis)
ott.assert_almost_equal(rankMDiag.getVariance(), [2.0, 3.0], 0, 0)
ott.assert_almost_equal(
    rankMDiag([0.5], [0.25])[0, 0], rankM([0.5], [0.25])[0, 0], 1e-12, 1e-12
)
# non-diagonal covariance ctor exercises the full covariance branch
fullCov = ot.CovarianceMatrix(2)
fullCov[0, 0] = 2.0
fullCov[1, 1] = 3.0
fullCov[1, 0] = 0.5
rankMFull = ot.RankMCovarianceModel(fullCov, basis)
ott.assert_almost_equal(
    rankMFull.getCovariance(), fullCov, 1e-14, 1e-14
)
# C(s,t) = sum_ij phi_i(s) cov_ij phi_j(t)
ott.assert_almost_equal(
    rankMFull([0.5], [0.25])[0, 0],
    2.0 + 0.5 * (0.5 + 0.25) + 3.0 * 0.5 * 0.25,
    1e-12,
    1e-12,
)
# discretize on both branches
vertices = ot.Sample([[0.0], [1.0], [2.0]])
ott.assert_almost_equal(
    rankM.discretize(vertices),
    ot.CovarianceMatrix([[2.0, 2.0, 2.0], [2.0, 5.0, 8.0], [2.0, 8.0, 14.0]]),
    1e-12,
    1e-12,
)
assert rankMFull.discretize(vertices).getNbRows() == 3
with ott.assert_raises(Exception):
    rankM.discretize(ot.Sample([[0.0, 0.0]]))
# partialGradient is not implemented
with ott.assert_raises(Exception):
    rankM.partialGradient([0.0], [1.0])
# operator() dimension check
with ott.assert_raises(Exception):
    rankM([0.0, 1.0], [0.0])
with ott.assert_raises(Exception):
    rankM([0.0], [0.0, 1.0])
# invalid ctors
with ott.assert_raises(Exception):
    ot.RankMCovarianceModel([], basis)
with ott.assert_raises(Exception):
    ot.RankMCovarianceModel(ot.CovarianceMatrix(0), basis)
# comparison, repr/str
assert rankM == rankM
assert not (rankM != rankM)
assert rankM != rankMFull
assert rankMDiag == rankM
assert len(repr(rankM)) > 0
assert len(str(rankM)) > 0
assert len(repr(rankMFull)) > 0
assert len(str(rankMFull)) > 0
# cross-check discretize against operator() on a small mesh
mesh = ot.IntervalMesher([3]).build(ot.Interval([-2.0], [2.0]))
verts = mesh.getVertices()
C = rankM.discretize(verts)
for j in range(len(verts)):
    for i in range(j, len(verts)):
        ott.assert_almost_equal(
            C[i, j], rankM(verts[i], verts[j])[0, 0], 1e-12, 1e-12
        )
CFull = rankMFull.discretize(verts)
for j in range(len(verts)):
    for i in range(j, len(verts)):
        ott.assert_almost_equal(
            CFull[i, j], rankMFull(verts[i], verts[j])[0, 0], 1e-12, 1e-12
        )
# save/load
storage = ot.XMLStorageManager("t_CovarianceModel_std_RankM.xml")
study = ot.Study()
study.setStorageManager(storage)
study.add("model", rankMFull)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("t_CovarianceModel_std_RankM.xml"))
loadedStudy.load()
loadedModel = ot.CovarianceModel()
loadedStudy.fillObject("model", loadedModel)
assert loadedModel == rankMFull
os.remove("t_CovarianceModel_std_RankM.xml")

# FractionalBrownianMotionModel extra coverage
fbm_default = ot.FractionalBrownianMotionModel()
assert fbm_default == fbm_default
assert len(repr(fbm_default)) > 0
assert len(str(fbm_default)) > 0
fbm = ot.FractionalBrownianMotionModel([2.0], [3.0], [0.25])
ott.assert_almost_equal(fbm.getExponent(), [0.25])
test_model(fbm)
# 5-argument constructor with dissymmetry and correlation
eta = ot.SquareMatrix([[0.0, 0.1], [-0.1, 0.0]])
rho = ot.CorrelationMatrix([[1.0, 0.5], [0.5, 1.0]])
fbm2 = ot.FractionalBrownianMotionModel([1.0], [1.0, 2.0], [0.3, 0.2], eta, rho)
assert fbm2 == fbm2
assert not fbm2 == fbm
ott.assert_almost_equal(fbm2.getExponent(), [0.3, 0.2])
ott.assert_almost_equal(fbm2.getEta(), eta)
ott.assert_almost_equal(fbm2.getRho(), rho)
assert len(str(fbm2)) > 0
# NOTE: operator() is not symmetric for output dim > 1 when eta is nonzero,
# the correct relation is C(s,t)^T == C(t,s), so the symmetry-based generic
# test_model cannot be used here.
# Check the diagonal against its closed form instead.
ott.assert_almost_equal(
    fbm2([0.5], [1.5])[0, 0],
    0.5 * (abs(0.5) ** 0.6 + abs(1.5) ** 0.6 - abs(1.0) ** 0.6),
    1e-12,
    1e-12,
)
ott.assert_almost_equal(
    fbm2([0.5], [1.5])[1, 1],
    0.5 * 4.0 * (abs(0.5) ** 0.4 + abs(1.5) ** 0.4 - abs(1.0) ** 0.4),
    1e-12,
    1e-12,
)
# the asymmetry term changes sign between (i, j) and (j, i), so that
# C(s,t)^T == C(t,s) (regression: upper triangle stayed 0 or was mirrored)
assert abs(fbm2([0.5], [1.5])[1, 0]) > 0.0
assert abs(fbm2([0.5], [1.5])[0, 1]) > 0.0
ott.assert_almost_equal(
    fbm2([0.5], [1.5])[0, 1], fbm2([1.5], [0.5])[1, 0], 1e-12, 1e-12
)
ott.assert_almost_equal(
    fbm2([0.5], [1.5])[1, 0], fbm2([1.5], [0.5])[0, 1], 1e-12, 1e-12
)
# symmetric case (eta=0): C(s,t) == C(t,s)^T
fbmSym = ot.FractionalBrownianMotionModel(
    [1.0], [1.0, 2.0], [0.3, 0.2], ot.SquareMatrix(2), ot.CorrelationMatrix(2)
)
cst = fbmSym([0.5], [1.5])
cts = fbmSym([1.5], [0.5])
ott.assert_almost_equal(cst[0, 1], cts[1, 0], 1e-12, 1e-12)
ott.assert_almost_equal(cst[1, 0], cst[0, 1], 1e-12, 1e-12)
assert abs(fbm2([-0.5], [1.0])[1, 0]) > 0.0
# multivariate evaluation: sign/zero/abs branches and Hij == 1 branch
_ = fbm2([-0.5], [1.0])
_ = fbm2([0.0], [1.0])
_ = fbm2([0.0], [0.0])
fbm_h1 = ot.FractionalBrownianMotionModel(
    [1.0], [1.0, 1.0], [0.5, 0.5], eta, ot.CorrelationMatrix(2)
)
ott.assert_almost_equal(
    fbm_h1([0.5], [1.5])[0, 0], 0.5 * (abs(0.5) + abs(1.5) - abs(1.0)), 1e-12, 1e-12
)
_ = fbm_h1([0.0], [0.0])
assert fbm_h1 == fbm_h1
assert not fbm_h1 == fbm2
# nugget factor on the diagonal
fbm.setNuggetFactor(0.5)
assert fbm.computeAsScalar([1.0], [1.0]) > fbm.computeAsScalar([1.0], [2.0])
_ = fbm([1.0], [1.0])
fbm.setNuggetFactor(0.0)
# error paths
with ott.assert_raises(Exception):
    ot.FractionalBrownianMotionModel([1.0, 2.0], [1.0], [0.5])
with ott.assert_raises(Exception):
    ot.FractionalBrownianMotionModel([1.0], [1.0], [1.5])
with ott.assert_raises(Exception):
    ot.FractionalBrownianMotionModel([1.0], [1.0], [-0.5])
with ott.assert_raises(Exception):
    ot.FractionalBrownianMotionModel(
        [1.0], [1.0, 2.0], [0.3, 0.4], ot.SquareMatrix(3), ot.CorrelationMatrix(2)
    )
with ott.assert_raises(Exception):
    ot.FractionalBrownianMotionModel(
        [1.0], [1.0, 2.0], [0.3, 0.4], ot.SquareMatrix(2), ot.CorrelationMatrix(3)
    )
with ott.assert_raises(Exception):
    ot.FractionalBrownianMotionModel(
        [1.0],
        [1.0, 1.0],
        [0.9, 0.9],
        ot.SquareMatrix([[0.0, 0.9], [-0.9, 0.0]]),
        ot.CorrelationMatrix(2),
    )
with ott.assert_raises(Exception):
    fbm.computeAsScalar([0.5, 1.0], [1.0])
with ott.assert_raises(Exception):
    fbm.computeAsScalar([0.5, 1.0])
with ott.assert_raises(Exception):
    fbm([0.5, 1.0], [1.0])
with ott.assert_raises(Exception):
    fbm.partialGradient([0.5, 1.0], [1.0])
with ott.assert_raises(Exception):
    fbm2.computeAsScalar([0.5], [1.0])
with ott.assert_raises(Exception):
    fbm.setExponentEtaRho([0.5], ot.SquareMatrix(1), ot.CorrelationMatrix(2))
with ott.assert_raises(Exception):
    fbm.setFullParameter([1.0])
# full parameter roundtrip
fp = fbm2.getFullParameter()
assert len(fbm2.getFullParameterDescription()) == len(fp)
fbm2.setFullParameter(fp)
ott.assert_almost_equal(fbm2.getFullParameter(), fp)
# save/load
storage = ot.XMLStorageManager("t_CovarianceModel_std_FBM.xml")
study = ot.Study()
study.setStorageManager(storage)
study.add("model", ot.CovarianceModel(fbm2))
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("t_CovarianceModel_std_FBM.xml"))
loadedStudy.load()
loadedModel = ot.CovarianceModel()
loadedStudy.fillObject("model", loadedModel)
assert loadedModel == ot.CovarianceModel(fbm2)
os.remove("t_CovarianceModel_std_FBM.xml")
