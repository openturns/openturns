#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from openturns.testing import *

TESTPREAMBLE()
# The objective is the validation of KrigingAlgorithm & KrigingEvaluation with ExponentialModel
# For that purpose, we use a 3d model which have been calibrated in comparison with 1d-exponential models
# This last one returns the same covariance matrix if amplitude=1, scale
# are similars and nugget factor value fixed to 0
ResourceMap.SetAsNumericalScalar(
    "CovarianceModelImplementation-DefaultNuggetFactor", 0)


# 1) Definition of use case: spatial data, numerical functions

# Kriging use case
spatialDimension = 1
sampleSize = 201

xMax = 10.0
xMin = 0.0
dX = xMax - xMin
dx = dX / (sampleSize - 1.0)

# Learning data
inputSample = NumericalSample(sampleSize, spatialDimension)
# Validation data
inputValidSample = NumericalSample(sampleSize, spatialDimension)
for k in range(sampleSize):
    inputSample[k, 0] = k * dx
    inputValidSample[k, 0] = 0.5 * (2.0 * k + 1.0) * dx

f = NumericalMathFunction(['x0'], ['y'], ['x0 * sin(x0)'])
g = NumericalMathFunction(['x0'], ['y'], ['3 + cos(x0*x0)'])
h = NumericalMathFunction(['x0'], ['y'], ['cos(0.5*x0*x0) + sin(x0)'])
model = NumericalMathFunctionCollection()
model.add(f)
model.add(g)
model.add(h)
model = NumericalMathFunction(model)

outputSample = model(inputSample)
outputValidSample = model(inputValidSample)

# 2) Definition of exponential model
# The parameters have been calibrated using TNC optimization
# and AbsoluteExponential models
amplitude = [1.0, 1.0, 1.0]
scale = [2.0]
expCovModel = ExponentialModel(spatialDimension, amplitude, scale)

# 3) Basis definition
basisCollection = BasisCollection()
basisCollection.add(ConstantBasisFactory(spatialDimension).build())
basisCollection.add(LinearBasisFactory(spatialDimension).build())
basisCollection.add(Basis())

# Kriging algorithm
algo = KrigingAlgorithm(
    inputSample, outputSample, basisCollection, expCovModel)
algo.run()
result = algo.getResult()
print("KrigingResult=", repr(result))
# Get meta model
metaModel = result.getMetaModel()
outData = metaModel(inputValidSample)

# 4) Errors
# Interpolation
assert_almost_equal(outputSample,  metaModel(inputSample))
# Estimation
# rtol & a tol fixed to 1e-1
assert_almost_equal(
    outputValidSample,  metaModel(inputValidSample), 1.e-1, 1e-1)

# 5) Kriging variance
# It should be 0 on learning points
var = result.getConditionalCovariance(inputSample)

# assert_almost_equal could not be applied to matrices
# application to NumericalPoint
covariancePoint = NumericalPoint(var.getImplementation())
covSize = expCovModel.getDimension() * sampleSize
theoricalVariance = NumericalPoint(covSize * covSize)
assert_almost_equal(covariancePoint, theoricalVariance, 1e-10, 1e-10)

# Gradient validation
metaModelDFGradient = result.getMetaModel()
metaModelDFGradient.setGradient(CenteredFiniteDifferenceGradient(ResourceMap.GetAsNumericalScalar(
    "CenteredFiniteDifferenceGradient-DefaultEpsilon"), metaModelDFGradient.getEvaluation()))
# Set precision for gradient
PlatformInfo.SetNumericalPrecision(3)
for validationPoint in inputValidSample:
    print ("df(X0)=", model.gradient(validationPoint), ", d^f(X0) =", metaModel.gradient(
        validationPoint), ",  d^f(X0) FD=", metaModelDFGradient.gradient(validationPoint))
