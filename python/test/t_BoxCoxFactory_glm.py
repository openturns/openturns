#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

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
translate = [3.1]
outputSample += translate

# Finally inverse transform using an arbitrary lambda
lamb = [1.8]
boxCoxFunction = ot.InverseBoxCoxEvaluation(lamb)

# transform y using BoxCox function
outputSample = boxCoxFunction(outputSample)

# Add small noise
epsilon = ot.Normal(0, 1.0e-2).getSample(size)
outputSample += epsilon

# Now we build the factory
factory = ot.BoxCoxFactory()

# Creation of the BoxCoxTransform
result = ot.GeneralLinearModelResult()
basis = ot.LinearBasisFactory(1).build()
covarianceModel = ot.DiracCovarianceModel()
shift = [1.0e-1]
myBoxCox = factory.build(inputSample, outputSample,
                         covarianceModel, basis, shift, result)

print("myBoxCox (GLM) =", myBoxCox)
print("GLM result     =", result)
