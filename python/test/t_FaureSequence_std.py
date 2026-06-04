#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()


# First 8 points of Faure sequence in dimension 1
expected = ot.Sample(
    [
        [1.0 / 2.0],
        [1.0 / 4.0],
        [3.0 / 4.0],
        [1.0 / 8.0],
        [5.0 / 8.0],
        [3.0 / 8.0],
        [7.0 / 8.0],
        [1.0 / 16.0],
    ]
)
sequence = ot.FaureSequence(1)
print(sequence)
faureSample = sequence.generate(8)
ott.assert_almost_equal(faureSample, expected)

# First 8 points of Faure sequence in dimension 2
expected = ot.Sample(
    [
        [0.5, 0.5],
        [0.25, 0.75],
        [0.75, 0.25],
        [0.125, 0.625],
        [0.625, 0.125],
        [0.375, 0.375],
        [0.875, 0.875],
        [0.0625, 0.9375],
    ]
)
sequence = ot.FaureSequence(2)
print(sequence)
faureSample = sequence.generate(8)
ott.assert_almost_equal(faureSample, expected)

# First 8 points of Faure sequence in dimension 3
expected = ot.Sample(
    [
        [1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0],
        [2.0 / 3.0, 2.0 / 3.0, 2.0 / 3.0],
        [1.0 / 9.0, 4.0 / 9.0, 7.0 / 9.0],
        [4.0 / 9.0, 7.0 / 9.0, 1.0 / 9.0],
        [7.0 / 9.0, 1.0 / 9.0, 4.0 / 9.0],
        [2.0 / 9.0, 8.0 / 9.0, 5.0 / 9.0],
        [5.0 / 9.0, 2.0 / 9.0, 8.0 / 9.0],
        [8.0 / 9.0, 5.0 / 9.0, 2.0 / 9.0],
    ]
)
sequence = ot.FaureSequence(3)
print(sequence)
faureSample = sequence.generate(8)
ott.assert_almost_equal(faureSample, expected)

# Create a Faure sequence in dimension 15
sequence = ot.FaureSequence(15)
print(sequence)
faureSample = sequence.generate(10)

# Create another Faure' sequence of dimension 2 to estimate Pi in [0 1)^2
dimension = 2
sequence = ot.FaureSequence(dimension)
pointInsideCircle = 0
sampleSize = 3**7  # This is significant!
for i in range(sampleSize):
    faurePoint = sequence.generate()
    if faurePoint.norm() < 1.0:
        pointInsideCircle += 1
probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
probability = m.pi / 4.0
print("sample size=", sampleSize)
print("computed probability =", probabilityEstimate)
print("expected probability =", probability)
rtol = 10.0 / sampleSize
ott.assert_almost_equal(probability, probabilityEstimate, rtol)

# Test against GSobol' test function
# https://github.com/openturns/openturns/issues/2653
a = [0.0, 9.0, 99.0]


def GSobolModel(X):
    X = ot.Point(X)
    d = X.getDimension()
    Y = 1.0
    for i in range(d):
        Y *= (abs(4.0 * X[i] - 2.0) + a[i]) / (1.0 + a[i])
    return ot.Point([Y])


dimension = 3
gSobolFunction = ot.PythonFunction(dimension, 1, GSobolModel)
gSobolFunction.setOutputDescription(["Y"])

# Define the distribution
distributionList = [ot.Uniform(0.0, 1.0) for i in range(dimension)]
distribution = ot.JointDistribution(distributionList)

# Compute Mean
gSobolMean = 1.0
basis = 3  # ie the smallest prime number greater or equal to the dimension
sampleSize = basis**7  # This is significant!
sequence = ot.FaureSequence()
experiment_QMC = ot.LowDiscrepancyExperiment(sequence, distribution, sampleSize, False)
inputSample = experiment_QMC.generate()
outputSample = gSobolFunction(inputSample)
computedMean = outputSample.computeMean()[0]
print("sample size=", sampleSize)
print("computed mean =", computedMean)
print("expected mean =", gSobolMean)
rtol = 10.0 / sampleSize
ott.assert_almost_equal(computedMean, gSobolMean, rtol)
