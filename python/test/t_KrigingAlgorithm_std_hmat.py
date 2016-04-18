#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from openturns.testing import *
from math import sqrt

TESTPREAMBLE()

# Set hmat
ResourceMap.Set("KrigingAlgorithm-LinearAlgebra", "HMAT")
# Test 1

sampleSize = 6
dimension = 1

f = NumericalMathFunction(['x0'], ['y'], ['x0 * sin(x0)'])

X = NumericalSample(sampleSize, dimension)
X2 = NumericalSample(sampleSize, dimension)
for i in range(sampleSize):
    X[i, 0] = 3.0 + i
    X2[i, 0] = 2.5 + i
X[0, 0] = 1.0
X[1, 0] = 3.0
X2[0, 0] = 2.0
X2[1, 0] = 4.0
Y = f(X)
Y2 = f(X2)

# create algorithm
basis = ConstantBasisFactory(dimension).build()
covarianceModel = SquaredExponential([1e-05], [4.11749])

algo = KrigingAlgorithm(X, Y, basis, covarianceModel)

algo.run()

# perform an evaluation
result = algo.getResult()
print("X=", X)
print("f(X)=", Y)

assert_almost_equal(result.getMetaModel()(X), Y)
assert_almost_equal(result.getResiduals(), [1.32804e-07], 1e-3, 1e-3)
assert_almost_equal(result.getRelativeErrors(), [5.20873e-21])

# Kriging variance is 0 on learning points
var = result.getConditionalCovariance(X)

# assert_almost_equal could not be applied to matrices
# application to NumericalPoint
covariancePoint = NumericalPoint(var.getImplementation())
theoricalVariance = NumericalPoint(sampleSize * sampleSize)
assert_almost_equal(covariancePoint, theoricalVariance, 8.95e-7, 8.95e-7)

# Test 2

# Kriging use case
spatialDimension = 2

# Learning data
levels = [8., 5.]
box = Box(levels)
inputSample = box.generate()
# Scale each direction
inputSample *= 10


model = NumericalMathFunction(['x', 'y'], ['z'], ['cos(0.5*x) + sin(y)'])
outputSample = model(inputSample)

# Validation
sampleSize = 10
inputValidSample = ComposedDistribution(
    2 * [Uniform(0, 10.0)]).getSample(sampleSize)
outputValidSample = model(inputValidSample)

# 2) Definition of exponential model
# The parameters have been calibrated using TNC optimization
# and AbsoluteExponential models
covarianceModel = SquaredExponential([1.98824, 0.924731], [3.15352])

# 3) Basis definition
basisCollection = BasisCollection(
    1, ConstantBasisFactory(spatialDimension).build())

# Kriring algorithm
algo = KrigingAlgorithm(
    inputSample, outputSample, basisCollection, covarianceModel)
algo.run()
result = algo.getResult()
# Get meta model
metaModel = result.getMetaModel()
outData = metaModel(inputValidSample)


# 4) Errors
# Interpolation
assert_almost_equal(outputSample,  metaModel(inputSample), 1e-5, 1e-5)


# 5) Kriging variance is 0 on learning points
var = result.getConditionalCovariance(inputSample)

# assert_almost_equal could not be applied to matrices
# application to NumericalPoint
covariancePoint = NumericalPoint(var.getImplementation())
theoricalVariance = NumericalPoint(covariancePoint.getSize(), 0.0)
assert_almost_equal(covariancePoint, theoricalVariance, 7e-7, 7e-7)

# Estimation
# rtol & a tol fixed to 1e-1
assert_almost_equal(
    outputValidSample,  metaModel(inputValidSample), 1.e-1, 1e-1)
