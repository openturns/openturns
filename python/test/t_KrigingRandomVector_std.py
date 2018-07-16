#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from openturns.testing import *

TESTPREAMBLE()

PlatformInfo.SetNumericalPrecision(3)
# Kriging use case
inputDimension = 2

# Learning data
levels = [8., 5.]
box = Box(levels)
inputSample = box.generate()
# Scale each direction
inputSample *= 10


model = SymbolicFunction(['x', 'y'], ['cos(0.5*x) + sin(y)'])
outputSample = model(inputSample)

# Validation data
sampleSize = 10
inputValidSample = ComposedDistribution(
    2 * [Uniform(0, 10.0)]).getSample(sampleSize)
outputValidSample = model(inputValidSample)

# 2) Definition of exponential model
# The parameters have been calibrated using TNC optimization
# and AbsoluteExponential models
covarianceModel = SquaredExponential([1.988, 0.924], [3.153])

# 3) Basis definition
basisCollection = BasisCollection(
    1, ConstantBasisFactory(inputDimension).build())

# Kriring algorithm
algo = KrigingAlgorithm(inputSample, outputSample,
                        covarianceModel, basisCollection)
algo.run()
result = algo.getResult()
# Get meta model
metaModel = result.getMetaModel()
outData = metaModel(inputValidSample)


# 4) Errors
# Interpolation
assert_almost_equal(outputSample,  metaModel(inputSample), 3.0e-5, 3.0e-5)


# 5) Kriging variance is 0 on learning points
var = result.getConditionalCovariance(inputSample)

# assert_almost_equal could not be applied to matrices
# application to Point
covariancePoint = Point(var.getImplementation())
theoricalVariance = Point(covariancePoint.getSize(), 0.0)
assert_almost_equal(covariancePoint, theoricalVariance, 1e-6, 1e-6)

# Random vector evaluation
rvector = KrigingRandomVector(result, inputValidSample[0])

# Realization of the random vector
realization = rvector.getRealization()
print("Realization of the KRV=", realization)

# Get a sample of size 10
realizations = rvector.getSample(10)
print("Sample of realizations of the KRV=", realizations)
