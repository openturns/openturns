#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
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
analysis = ot.LinearModelAnalysis(result)
print(analysis)
# Compute confidence level (95%) for coefficients estimate
alpha = 0.95
# interval confidence bounds
interval = analysis.getCoefficientsConfidenceInterval(alpha)
print("confidence intervals with level=%1.2f : %s" % (alpha, interval))

print("")
print("")
print("")
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
analysis = ot.LinearModelAnalysis(result)
print(analysis)
# Compute confidence level (95%) for coefficients estimate
alpha = 0.95
# interval confidence bounds
interval = analysis.getCoefficientsConfidenceInterval(alpha)
print("confidence intervals with level=%1.2f : %s" % (alpha, interval))

# https://github.com/openturns/openturns/issues/1729
ot.RandomGenerator.SetSeed(1789)
sample_size = 14605 # 14604 : OK, 14605 : Fail
a0 = 0.6
a1 = 2.0
x = ot.Normal(0.5, 0.2).getSample(sample_size)
epsilon = ot.Exponential(0.1).getSample(sample_size)
y = ot.Sample([[a0]]*sample_size) + a1 * x + epsilon
algo = ot.LinearModelAlgorithm(x, y)
result = algo.getResult()
analysis = ot.LinearModelAnalysis(result)
results = str(analysis)
