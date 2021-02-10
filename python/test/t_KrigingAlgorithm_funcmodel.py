#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

inputDimension = 2

levels = [9, 6]
box = ot.Box(levels)
inputSample = box.generate()
inputSample *= 10.0

model = ot.SymbolicFunction(['x', 'y'], ['cos(0.5*x) + sin(y)'])
outputSample = model(inputSample)

# Validation
sampleSize = 10
inputValidSample = ot.ComposedDistribution(
    2 * [ot.Uniform(0, 10.0)]).getSample(sampleSize)
outputValidSample = model(inputValidSample)

# Basis definition
basis = ot.LinearBasisFactory(inputDimension).build()

# Reimplement the squared exponential covariance model
rho = ot.SymbolicFunction(
    ['x', 'y'], ['exp(-0.5* (x * x + y * y))'])
covarianceModel = ot.StationaryFunctionalCovarianceModel([5, 2], [1], rho)
covarianceModel.setParameter([6.72648, 3.49326, 4.90341])

# Kriging algorithm
algo = ot.KrigingAlgorithm(inputSample, outputSample, covarianceModel, basis)
loglikelihood = algo.getReducedLogLikelihoodFunction()([50.0] * inputDimension)
algo.setOptimizeParameters(False)
algo.run()
result = algo.getResult()
metaModel = result.getMetaModel()

# Consistency check: does the reimplementation fit the SquaredExponential class?
squaredExponential = ot.SquaredExponential(inputDimension)
squaredExponential.setParameter([6.72648, 3.49326, 4.90341])
algoSE = ot.KrigingAlgorithm(inputSample, outputSample, squaredExponential, basis)
loglikelihoodSE = algoSE.getReducedLogLikelihoodFunction()([50.0] * inputDimension)
ott.assert_almost_equal(loglikelihood, loglikelihoodSE, 0.0, 0.0)

# High level consistency check: does the prediction fit too?
algoSE.setOptimizeParameters(False)
algoSE.run()
resultSE = algoSE.getResult()
metaModelSE = resultSE.getMetaModel()
ott.assert_almost_equal(metaModel(inputValidSample), metaModelSE(inputValidSample), 0.0, 0.0)

# Validate the metamodel
ott.assert_almost_equal(outputValidSample, metaModel(inputValidSample), 5.0e-3, 5.0e-3)
