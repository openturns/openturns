#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.RandomGenerator.SetSeed(0)

size = 200

# input sample
inputSample = ot.Uniform(-1.0, 1.0).getSample(size)
outputSample = ot.Sample(inputSample)

# Evaluation of y = ax + b (a: scale, b: translate)

# scale
scale = [3.0]
outputSample *= scale

# translate sample
translate = [10]
outputSample += translate

# Finally inverse transform using an arbitrary lambda
lamb = [2.0]
boxCoxFunction = ot.InverseBoxCoxEvaluation(lamb)

# transform y using BoxCox function
outputSample = boxCoxFunction(outputSample)

# Add small noise
epsilon = ot.Normal(0, 1.0e-3).getSample(size)
outputSample += epsilon

# Now we build the factory
factory = ot.BoxCoxFactory()

# Creation of the BoxCoxTransform
basis = ot.LinearBasisFactory(1).build()
covarianceModel = ot.DiracCovarianceModel()
shift = [1.0e-10]
myBoxCox, result = factory.buildWithGPF(
    inputSample, outputSample, covarianceModel, basis, shift
)

ott.assert_almost_equal(myBoxCox.getLambda()[0], 1.99098, 1.0e-4, 1.0e-4)
ott.assert_almost_equal(result.getTrendCoefficients(), [9.90054, 2.95995], 1.0e-4, 1.0e-4)
# getAmplitude() returns the standard deviation, i.e. the square root of the
# variance 2.12358e-05 printed by the covariance model.
ott.assert_almost_equal(result.getCovarianceModel().getAmplitude(), [0.00460823], 1.0e-6, 1.0e-6)
