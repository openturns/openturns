#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


def test_model(myModel, test_grad=True, x1=None, x2=None):

    print('myModel = ',  myModel)

    inputDimension = myModel.getInputDimension()
    dimension = myModel.getOutputDimension()
    active = myModel.getActiveParameter()
    print('active=', active)
    print('parameter=', myModel.getParameter())
    print('parameterDescription=', myModel.getParameterDescription())

    if x1 is None and x2 is None:
        x1 = ot.Point(inputDimension)
        x2 = ot.Point(inputDimension)
        for j in range(inputDimension):
            x1[j] = -1.0 - j
            x2[j] = 3.0 + 2.0 * j

    eps = 1e-5
    print('myModel(', x1, ', ', x2, ')=',  repr(myModel(x1, x2)))

    grad = myModel.partialGradient(x1, x2)
    print('dCov =', repr(grad))

    if (dimension == 1):
        gradfd = ot.Point(inputDimension)
        for j in range(inputDimension):
            x1_d = ot.Point(x1)
            x1_d[j] = x1_d[j] + eps
            gradfd[j] = (myModel.computeAsScalar(x1_d, x2) - myModel.computeAsScalar(x1, x2)) / eps
    else:
        gradfd = ot.Matrix(inputDimension, dimension * dimension)
        covarianceX1X2 = myModel(x1, x2)
        centralValue = ot.Point(covarianceX1X2.getImplementation())
        # Loop over the shifted points
        for i in range(inputDimension):
            currentPoint = ot.Point(x1)
            currentPoint[i] += eps
            localCovariance = myModel(currentPoint, x2)
            currentValue = ot.Point(
                localCovariance.getImplementation())
            for j in range(currentValue.getSize()):
                gradfd[i, j] = (currentValue[j] - centralValue[j]) / eps
    print('dCov (FD)=', repr(gradfd))

    if test_grad:
        pGrad = myModel.parameterGradient(x1, x2)
        precision = ot.PlatformInfo.GetNumericalPrecision()
        ot.PlatformInfo.SetNumericalPrecision(4)
        print('dCov/dP=', pGrad)
        ot.PlatformInfo.SetNumericalPrecision(precision)


def test_scalar_model(myModel, test_grad=True):

    inputDimension = 1
    dimension = 1
    active = myModel.getActiveParameter()
    
    x1 = 2.0
    x2 = -3.0
    ott.assert_almost_equal(myModel.computeAsScalar([x1], [x2]), myModel.computeAsScalar(x1, x2), 1.0e-14, 1.0e-14)

    eps = 1e-5
 
    grad = myModel.partialGradient([x1], [x2])[0, 0]

    x1_g = x1 + eps
    x1_d = x1 - eps
    gradfd = (myModel.computeAsScalar(x1_g, x2) - myModel.computeAsScalar(x1_d, x2)) / (2.0 * eps)
    ott.assert_almost_equal(gradfd, grad, 1e-5, 1e-5)

inputDimension = 2

myDefautModel = ot.SquaredExponential([2.0], [3.0])
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

myModel = ot.SquaredExponential([2.0] * inputDimension, [3.0])
test_model(myModel)


myDefautModel = ot.GeneralizedExponential([2.0], [3.0], 1.5)
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

myModel = ot.GeneralizedExponential([2.0] * inputDimension, [3.0], 1.5)
test_model(myModel)


myDefautModel = ot.AbsoluteExponential([2.0], [3.0])
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

myModel = ot.AbsoluteExponential([2.0] * inputDimension, [3.0])
test_model(myModel)

myDefautModel = ot.MaternModel([2.0], [3.0], 1.5)
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

myModel = ot.MaternModel([2.0] * inputDimension, [3.0], 1.5)
test_model(myModel)


myDefautModel = ot.ExponentiallyDampedCosineModel([2.0], [3.0], 1.5)
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

myModel = ot.ExponentiallyDampedCosineModel(
    [2.0] * inputDimension, [3.0], 1.5)
test_model(myModel)

myDefautModel = ot.SphericalModel([2.0], [3.0], 4.5)
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

myModel = ot.SphericalModel([2.0] * inputDimension, [3.0], 4.5)
test_model(myModel)

myDefautModel = ot.FractionalBrownianMotionModel(2.0, 3.0, 0.25)
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

myModel = ot.SphericalModel([2.0] * inputDimension, [3.0], 4.5)
test_model(myModel)

myDefautModel = ot.DiracCovarianceModel()
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

rho = ot.SymbolicFunction(['tau'], ['exp(-abs(tau))*cos(2*pi_*abs(tau))'])
myDefautModel = ot.StationaryFunctionalCovarianceModel([1.0], [1.0], rho)
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

amplitude = [1.5 + 2.0 * k for k in range(2)]
dimension = 2
spatialCorrelation = ot.CorrelationMatrix(dimension)
for j in range(dimension):
    for i in range(j + 1, dimension):
        spatialCorrelation[i, j] = (
            i + 1.0) / dimension - (j + 1.0) / dimension
myModel = ot.DiracCovarianceModel(
    inputDimension, amplitude, spatialCorrelation)
test_model(myModel, x1=[0.5, 0.0], x2=[0.5, 0.0])

myDefautModel = ot.ProductCovarianceModel()
print('myDefautModel = ',  myDefautModel)
test_model(myDefautModel)

cov1 = ot.AbsoluteExponential([2.0], [3.0])
cov2 = ot.SquaredExponential([2.0], [3.0])
myModel = ot.ProductCovarianceModel([cov1, cov2])
test_model(myModel)

# Collection ==> add covariance models
# Add AbsoluteExponentialModel to the collection
myAbsoluteExponential = ot.AbsoluteExponential([2.0] * inputDimension, [3.0])
mySquaredExponential = ot.SquaredExponential([2.0] * inputDimension, [3.0])
myGeneralizedExponential = ot.GeneralizedExponential([2.0] * inputDimension, [3.0], 1.5)
# Build TensorizedCovarianceModel with scale = [1,..,1]
myModel = ot.TensorizedCovarianceModel(
    [myAbsoluteExponential, mySquaredExponential, myGeneralizedExponential])
test_model(myModel, test_grad=False)
# Define new scale
scale = [2.5, 1.5]
myModel.setScale(scale)
test_model(myModel, test_grad=False)

# Test scalar input models
coll = [ot.AbsoluteExponential(), ot.SquaredExponential(), ot.GeneralizedExponential()]
coll += [ot.MaternModel(), ot.SphericalModel(), ot.ExponentiallyDampedCosineModel()]
for model in coll:
    test_scalar_model(model)
