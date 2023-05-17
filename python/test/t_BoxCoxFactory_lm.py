#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

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
shift = [1.0e-10]
myBoxCox, result = factory.buildWithLM(inputSample, outputSample, shift)

# estimated lambda =  1.99098,  beta = [9.90054,2.95995]
beta = [9.90054, 2.95995]
rtol = 1e-2
atol = 5e-3
ott.assert_almost_equal(myBoxCox.getLambda(), lamb, rtol, atol)
ott.assert_almost_equal(result.getCoefficients(), beta, rtol, atol)
