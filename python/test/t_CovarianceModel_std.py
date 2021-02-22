#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
from math import sqrt

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

    mesh = ot.IntervalMesher([9]*inputDimension).build(
        ot.Interval([-10]*inputDimension, [10]*inputDimension))

    C = myModel.discretize(mesh)
    if dimension == 1:
        # Check that discretize & computeAsScalar provide the 
        # same values
        vertices = mesh.getVertices()
        for j in range(len(vertices)):
            for i in range(j, len(vertices)):
                ott.assert_almost_equal(C[i,j], myModel.computeAsScalar(vertices[i], vertices[j]), 1e-14, 1e-14)
    else:
        # Check that discretize & operator() provide the
        # same values
        vertices = mesh.getVertices()
        localMatrix = ot.SquareMatrix(dimension)
        for j in range(len(vertices)):
            for i in range(j, len(vertices)):
                for localJ in range(dimension):
                    for localI in range(dimension):
                        localMatrix[localI, localJ] = C[i * dimension + localI,j * dimension + localJ]
                ott.assert_almost_equal(localMatrix, myModel(vertices[i], vertices[j]), 1e-14, 1e-14)

    if test_partial_grad:
        grad = myModel.partialGradient(x1, x2)

        if (dimension == 1):
            gradfd = ot.Matrix(inputDimension, 1)
            for j in range(inputDimension):
                x1_g = ot.Point(x1)
                x1_d = ot.Point(x1)
                x1_g[j] = x1_d[j] + eps
                x1_d[j] = x1_d[j] - eps
                gradfd[j, 0] = (myModel.computeAsScalar(x1_g, x2) - myModel.computeAsScalar(x1_d, x2)) / (2 * eps)
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

        ott.assert_almost_equal(grad, gradfd, 1e-5, 1e-5, "in " + myModel.getClassName() + " grad")        

def test_scalar_model(myModel, x1=None, x2=None):
    if x1 is None and x2 is None:
        x1 = 2.0
        x2 = -3.0
    # Check that computeAsScalar(Scalar) == computeAsScalar(Point)
    ott.assert_almost_equal(myModel.computeAsScalar([x1], [x2]), myModel.computeAsScalar(x1, x2), 1.0e-14, 1.0e-14)

    # Gradient testing
    eps = 1e-5
 
    grad = myModel.partialGradient([x1], [x2])[0, 0]

    x1_g = x1 + eps
    x1_d = x1 - eps
    gradfd = (myModel.computeAsScalar(x1_g, x2) - myModel.computeAsScalar(x1_d, x2)) / (2.0 * eps)
    ott.assert_almost_equal(gradfd, grad, 1e-5, 1e-5)

inputDimension = 2

# 1) SquaredExponential
myModel = ot.SquaredExponential([2.0], [3.0])
ott.assert_almost_equal(myModel.getScale(), [2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
test_model(myModel)

myModel = ot.SquaredExponential([2.0] * inputDimension, [3.0])
ott.assert_almost_equal(myModel.getScale(), [2, 2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
test_model(myModel)


# 2) GeneralizedExponential
myModel = ot.GeneralizedExponential([2.0], [3.0], 1.5)
ott.assert_almost_equal(myModel.getScale(), [2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getP(), 1.5, 1e-15, 1e-15)
test_model(myModel)

myModel = ot.GeneralizedExponential([2.0] * inputDimension, [3.0], 1.5)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getP(), 1.5, 1e-15, 1e-15)
test_model(myModel)

# 3) AbsoluteExponential
myModel = ot.AbsoluteExponential([2.0], [3.0])
ott.assert_almost_equal(myModel.getScale(), [2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
test_model(myModel)

myModel = ot.AbsoluteExponential([2.0] * inputDimension, [3.0])
ott.assert_almost_equal(myModel.getScale(), [2, 2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
test_model(myModel)

# 4) MaternModel
myModel = ot.MaternModel([2.0], [3.0], 1.5)
ott.assert_almost_equal(myModel.getScale(), [2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getNu(), 1.5, 1e-15, 1e-15)
test_model(myModel)

myModel = ot.MaternModel([2.0] * inputDimension, [3.0], 1.5)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getNu(), 1.5, 1e-15, 1e-15)
test_model(myModel)

# 5) ExponentiallyDampedCosineModel
myModel = ot.ExponentiallyDampedCosineModel([2.0], [3.0], 1)
ott.assert_almost_equal(myModel.getScale(), [2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getFrequency(), 1, 1e-15, 1e-15)
test_model(myModel)
myModel.setFrequency(3)
ott.assert_almost_equal(myModel.getFrequency(), 3, 1e-15, 1e-15)

myModel = ot.ExponentiallyDampedCosineModel([2.0] * inputDimension, [3.0], 1)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getFrequency(), 1, 1e-15, 1e-15)
test_model(myModel)

# 6) SphericalModel
myModel = ot.SphericalModel([2.0], [3.0], 4.5)
ott.assert_almost_equal(myModel.getScale(), [2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getRadius(), 4.5, 1e-15, 1e-15)
test_model(myModel)

myModel = ot.SphericalModel([2.0] * inputDimension, [3.0], 4.5)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getRadius(), 4.5, 1e-15, 1e-15)
test_model(myModel)
myModel.setRadius(1.5)
ott.assert_almost_equal(myModel.getRadius(), 1.5, 1e-15, 1e-15)

# 7) FractionalBrownianMotionModel
myModel = ot.FractionalBrownianMotionModel(2.0, 3.0, 0.25)
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
        spatialCorrelation[i, j] = (
            i + 1.0) / dimension - (j + 1.0) / dimension
myModel = ot.DiracCovarianceModel(
    inputDimension, amplitude, spatialCorrelation)
ott.assert_almost_equal(myModel.getScale(), [1, 1], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), amplitude, 1e-15, 1e-15)
test_model(myModel, test_partial_grad=False, x1=[0.5, 0.0], x2=[0.5, 0.0])

# 9) StationaryFunctionalCovarianceModel
rho = ot.SymbolicFunction(['tau'], ['exp(-abs(tau))*cos(2*pi_*abs(tau))'])
myModel = ot.StationaryFunctionalCovarianceModel([1.0], [1.0], rho)
ott.assert_almost_equal(myModel.getScale(), [1], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [1], 1e-15, 1e-15)
test_model(myModel)

# 10) ProductCovarianceModel
myModel = ot.ProductCovarianceModel()
test_model(myModel)

cov1 = ot.AbsoluteExponential([2.0], [3.0])
cov2 = ot.SquaredExponential([2.0], [3.0])
myModel = ot.ProductCovarianceModel([cov1, cov2])
test_model(myModel)
ott.assert_almost_equal(myModel.getScale(), [2, 2], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [9], 1e-15, 1e-15)
point = [0.50, -6]
x = [point[0]]
y = [point[1]]
ott.assert_almost_equal(myModel.computeAsScalar(point), cov1.computeAsScalar(x) * cov2.computeAsScalar(y), 1.0e-15, 1.0e-15)

# 11) TensorizedCovarianceModel

# Collection ==> add covariance models
myAbsoluteExponential = ot.AbsoluteExponential([2.0] * inputDimension, [3.0])
mySquaredExponential = ot.SquaredExponential([2.0] * inputDimension, [3.0])
myGeneralizedExponential = ot.GeneralizedExponential([2.0] * inputDimension, [3.0], 1.5)
# Build TensorizedCovarianceModel with scale = [1,..,1]
# Tensorized ignore scales
myModel = ot.TensorizedCovarianceModel(
    [myAbsoluteExponential, mySquaredExponential, myGeneralizedExponential])
ott.assert_almost_equal(myModel.getScale(), [1,1], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3, 3, 3], 1e-15, 1e-15)
test_model(myModel)

# Define new scale
scale = [2.5, 1.5]
myModel.setScale(scale)
ott.assert_almost_equal(myModel.getScale(), [2.5, 1.5], 1e-15, 1e-15)
ott.assert_almost_equal(myModel.getAmplitude(), [3, 3, 3], 1e-15, 1e-15)
test_model(myModel)

# 12) Testing 1d in/out dimension & stationary

# Test scalar input models
coll = [ot.AbsoluteExponential(), ot.SquaredExponential(), ot.GeneralizedExponential()]
coll += [ot.MaternModel(), ot.SphericalModel(), ot.ExponentiallyDampedCosineModel()]
for model in coll:
    test_scalar_model(model)

# 13) Isotropic covariance model
 
scale = 3.5
amplitude = 1.5
myOneDimensionalKernel = ot.SquaredExponential([scale], [amplitude])
myIsotropicKernel = ot.IsotropicCovarianceModel(myOneDimensionalKernel, inputDimension)

# Test consistency of isotropic model with underlying 1D kernel
ott.assert_almost_equal(myIsotropicKernel.getAmplitude()[0], amplitude, 1e-12, 0.0)
ott.assert_almost_equal(myIsotropicKernel.getScale()[0], scale, 1e-12, 0.0)
ott.assert_almost_equal(myIsotropicKernel.getKernel().getAmplitude()[0], amplitude, 1e-12, 0.0)
ott.assert_almost_equal(myIsotropicKernel.getKernel().getScale()[0], scale, 1e-12, 0.0)

# Standard tests applied
test_model(myIsotropicKernel)

# Test consistency of isotropic kernel's discretization
inputVector = ot.Point([0.3, 1.7])
inputVectorNorm = ot.Point([inputVector.norm()])
ott.assert_almost_equal(myOneDimensionalKernel(inputVectorNorm)[0,0], 1.992315565746, 1e-12, 0.0)
ott.assert_almost_equal(myIsotropicKernel(inputVector)[0,0], 1.992315565746, 1e-12, 0.0)
inputSample = ot.Sample([ot.Point(2), inputVector])
inputSampleNorm = ot.Sample([ot.Point(1), inputVectorNorm])
oneDimensionalCovMatrix = myOneDimensionalKernel.discretize(inputSampleNorm)
isotropicCovMatrix = myIsotropicKernel.discretize(inputSample)
ott.assert_almost_equal(oneDimensionalCovMatrix[0,0], 2.250000000002, 1e-12, 0.0)
ott.assert_almost_equal(oneDimensionalCovMatrix[1,1], 2.250000000002, 1e-12, 0.0)
ott.assert_almost_equal(isotropicCovMatrix[0,0], 2.250000000002, 1e-12, 0.0)
ott.assert_almost_equal(isotropicCovMatrix[1,1], 2.250000000002, 1e-12, 0.0)
ott.assert_almost_equal(oneDimensionalCovMatrix[0,1], 1.992315565746, 1e-12, 0.0)
ott.assert_almost_equal(isotropicCovMatrix[0,1], 1.992315565746, 1e-12, 0.0)

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
if (not checkDiag):
    raise Exception("isDiagonal differ between spatial covariance & covariance model")
rho = spatialCovariance[1, 0] / sqrt(spatialCovariance[0, 0] * spatialCovariance[1, 1])
ott.assert_almost_equal(myModel.getOutputCorrelation()[0,1], rho, 1e-15, 1e-15, "in ExponentialModel correlation")
