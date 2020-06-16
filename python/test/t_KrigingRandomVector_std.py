#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.PlatformInfo.SetNumericalPrecision(3)
# Kriging use case
inputDimension = 2

# Learning data
levels = [8., 5.]
box = ot.Box(levels)
inputSample = box.generate()
# Scale each direction
inputSample *= 10


model = ot.SymbolicFunction(['x', 'y'], ['cos(0.5*x) + sin(y)'])
outputSample = model(inputSample)

# Validation data
sampleSize = 10
inputValidSample = ot.ComposedDistribution(
    2 * [ot.Uniform(0, 10.0)]).getSample(sampleSize)
outputValidSample = model(inputValidSample)

# 2) Definition of exponential model
# The parameters have been calibrated using TNC optimization
# and AbsoluteExponential models
covarianceModel = ot.SquaredExponential([7.63, 2.11], [7.38])

# 3) Basis definition
basis = ot.ConstantBasisFactory(inputDimension).build()

# Kriging algorithm
algo = ot.KrigingAlgorithm(inputSample, outputSample,
                        covarianceModel, basis)
algo.setOptimizeParameters(False) # do not optimize hyper-parameters
algo.run()
result = algo.getResult()
# Get meta model
metaModel = result.getMetaModel()
outData = metaModel(inputValidSample)


# 4) Errors
# Interpolation
ott.assert_almost_equal(outputSample,  metaModel(inputSample), 3.0e-5, 3.0e-5)


# 5) Kriging variance is 0 on learning points
var = result.getConditionalCovariance(inputSample)

# assert_almost_equal could not be applied to matrices
# application to Point
covariancePoint = ot.Point(var.getImplementation())
trueVariance = ot.Point(covariancePoint.getSize(), 0.0)
ott.assert_almost_equal(covariancePoint, trueVariance, 1e-6, 1e-6)

# Random vector evaluation
rvector = ot.KrigingRandomVector(result, inputValidSample[0])

# Realization of the random vector
realization = rvector.getRealization()
print("Realization of the KRV=", realization)

# Get a sample of size 10
realizations = rvector.getSample(10)
print("Sample of realizations of the KRV=", realizations)
