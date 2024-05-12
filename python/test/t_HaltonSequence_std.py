#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

# Create 6 points from a Halton sequence in dimension 1
expected = ot.Sample(
    [
        [1.0 / 2.0],
        [1.0 / 4.0],
        [3.0 / 4.0],
        [1.0 / 8.0],
        [5.0 / 8.0],
        [3.0 / 8.0],
    ]
)
sequence = ot.HaltonSequence(1)
print(sequence)
haltonSample = sequence.generate(6)
ott.assert_almost_equal(haltonSample, expected)

# Create 6 points from a Halton sequence in dimension 2
expected = ot.Sample(
    [
        [1.0 / 2.0, 1.0 / 3.0],
        [1.0 / 4.0, 2.0 / 3.0],
        [3.0 / 4.0, 1.0 / 9.0],
        [1.0 / 8.0, 4.0 / 9.0],
        [5.0 / 8.0, 7.0 / 9.0],
        [3.0 / 8.0, 2.0 / 9.0],
    ]
)
sequence = ot.HaltonSequence(2)
print(sequence)
haltonSample = sequence.generate(6)
ott.assert_almost_equal(haltonSample, expected)

# Create 12 points from a Halton sequence in dimension 4
expected = ot.Sample(
    [
        [0.5, 0.333333, 0.2, 0.142857],
        [0.250000, 0.666667, 0.400000, 0.285714],
        [0.750000, 0.111111, 0.600000, 0.428571],
        [0.125000, 0.444444, 0.800000, 0.571429],
        [0.625000, 0.777778, 0.040000, 0.714286],
        [0.375000, 0.222222, 0.240000, 0.857143],
        [0.875000, 0.555556, 0.440000, 0.020408],
        [0.062500, 0.888889, 0.640000, 0.163265],
        [0.562500, 0.037037, 0.840000, 0.306122],
        [0.312500, 0.370370, 0.080000, 0.448980],
        [0.812500, 0.703704, 0.280000, 0.591837],
        [0.187500, 0.148148, 0.480000, 0.734694],
    ]
)
sequence = ot.HaltonSequence(4)
print(sequence)
haltonSample = sequence.generate(12)
ott.assert_almost_equal(haltonSample, expected)

# Create 10 points from a Halton in dimension 15
sequence = ot.HaltonSequence(15)
print(sequence)
haltonSample = sequence.generate(10)

# Create another Halton' sequence of dimension 2 to estimate Pi in [0 1)^2
dimension = 2
sequence = ot.HaltonSequence(dimension)
pointInsideCircle = 0
sampleSize = (2 * 3) ** 4  # This is significant!
for i in range(sampleSize):
    haltonPoint = sequence.generate()
    if haltonPoint.norm() < 1.0:
        pointInsideCircle += 1
probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
probability = m.pi / 4.0
print("sample size=", sampleSize)
print("computed probability =", probabilityEstimate)
print("expected probability =", probability)
rtol = 10.0 / sampleSize
ott.assert_almost_equal(probability, probabilityEstimate, rtol)
