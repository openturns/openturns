#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
from math import sin

ot.TESTPREAMBLE()


# lm build
print(
    "Fit y ~ 3 - 2 x + 0.05 * sin(x) model using 20 points (sin(x) ~ noise)")
size = 20
oneSample = ot.Sample(size, 1)
twoSample = ot.Sample(size, 1)
for i in range(size):
    oneSample[i, 0] = 7.0 * sin(-3.5 + (6.5 * i) / (size - 1.0)) + 2.0
    twoSample[i, 0] = -2.0 * oneSample[
        i, 0] + 3.0 + 0.05 * sin(oneSample[i, 0])

test = ot.LinearModelAlgorithm(oneSample, twoSample)
result = ot.LinearModelResult(test.getResult())
print ("trend coefficients = ", result.getCoefficients())

print("Fit y ~ 1 + 0.1 x + 10 x^2 model using 100 points")
ot.RandomGenerator.SetSeed(0)
size = 100
# Define a linespace from 0 to 10 with size points
# We use a Box expermient ==> remove 0 & 1 points
experiment = ot.Box([size - 2])
X = experiment.generate()
# X is defined in [0,1]
X *= [10]
# Stack X2
X2 = ot.Sample(X)
for i in range(size):
    X2[i, 0] = X[i, 0] * X2[i, 0]
X.stack(X2)
# Define y
Y = ot.Sample(size, 1)
for i in range(size):
    Y[i, 0] = 1.0 + 0.1 * X[i, 0] + 10.0 * \
        X[i, 0] * X[i, 0] + 0.1 * ot.DistFunc.rNormal()
test = ot.LinearModelAlgorithm(X, Y)
result = test.getResult()
print ("trend coefficients = ", result.getCoefficients())

# Test various attributes
cook = result.getCookDistances()
cook_reference = [0.0233296,0.0360369,0.00178903,0.0502183,0.0966701,0.00562596]
ott.assert_almost_equal(cook[0:6], cook_reference, 1e-5, 0.0)

leverages = result.getLeverages()
leverages_reference = [0.0864939,0.0797831,0.0735447,0.0677578,0.0624023,0.0574582]
ott.assert_almost_equal(leverages[0:6], leverages_reference, 1e-6, 0.0)

# with invalid sample description
X.setDescription(['X0', 'price (euros)'])
result = ot.LinearModelAlgorithm(X, Y).getResult()
print(result)

# dof=0
input_sample = ot.Sample([[1], [2]])
output_sample = ot.Sample([[2], [6]])
basis = ot.LinearBasisFactory(1).build()
algo = ot.LinearModelAlgorithm(input_sample, basis, output_sample)
algo.run()
result = algo.getResult()
print(result.getCoefficients(), result.getDegreesOfFreedom(), result.getResiduals())
