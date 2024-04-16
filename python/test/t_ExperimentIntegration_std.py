#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math
from openturns.usecases import ishigami_function

ot.TESTPREAMBLE()

# Test integrate
im = ishigami_function.IshigamiModel()
sampleSize = 100000
experiment = ot.MonteCarloExperiment(im.distributionX, sampleSize)
integration = ot.ExperimentIntegration(experiment)
approximatedOutputMean = integration.integrate(im.model)
rtol = 0.0
atol = 1.0 / math.sqrt(sampleSize)
assert approximatedOutputMean.getDimension() == 1
ott.assert_almost_equal(approximatedOutputMean[0], im.expectation, rtol, atol)

# Test computeL2Norm
im = ishigami_function.IshigamiModel()
centeredIshigamiFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3"], ["sin(x1) + 7 * (sin(x2)) ^ 2 + 0.1 * x3^4 * sin(x1) - 3.5"]
)
functionNorm = integration.computeL2Norm(centeredIshigamiFunction)
exactFunctionNorm = math.sqrt(im.variance)
rtol = 0.0
atol = 10.0 / math.sqrt(sampleSize)
ott.assert_almost_equal(functionNorm[0], exactFunctionNorm, rtol, atol)

# Test compute error
im = ishigami_function.IshigamiModel()
partOfIshigamiFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3"], ["7 * (sin(x2)) ^ 2 + 0.1 * x3^4 * sin(x1)"]
)
functionError = integration.computeL2Norm(im.model - partOfIshigamiFunction)
exactError = math.sqrt(0.5)
rtol = 0.0
atol = 10.0 / math.sqrt(sampleSize)
ott.assert_almost_equal(functionError[0], exactError, rtol, atol)

# Test integrate with multi-variate output
functionCollection = [im.model, im.model, im.model]
multivariateIshigami = ot.AggregatedFunction(functionCollection)
experiment = ot.MonteCarloExperiment(im.distributionX, sampleSize)
integration = ot.ExperimentIntegration(experiment)
approximatedOutputMean = integration.integrate(multivariateIshigami)
rtol = 0.0
atol = 10.0 / math.sqrt(sampleSize)
assert approximatedOutputMean.getDimension() == 3
exactIntegral = [im.expectation] * 3
ott.assert_almost_equal(approximatedOutputMean, exactIntegral, rtol, atol)
