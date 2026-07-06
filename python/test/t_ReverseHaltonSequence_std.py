#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()


# Create a ReverseHalton
sequence = ot.ReverseHaltonSequence(15)
print(sequence)

# Create a numerical sample of the sequence
reverseHaltonSample = sequence.generate(10)
print(repr(reverseHaltonSample))

# Create another ReverseHalton' sequence of dimension 2 to estimate Pi in
# [0 1)^2
dimension = 2
sequence = ot.ReverseHaltonSequence(dimension)
pointInsideCircle = 0
sampleSize = 1000
for i in range(sampleSize):
    reverseHaltonPoint = sequence.generate()
    print(reverseHaltonPoint.__repr__())
    if reverseHaltonPoint.norm() < 1.0:
        pointInsideCircle = pointInsideCircle + 1
probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
probability = m.pi / 4.0
relativeError = abs(probability - probabilityEstimate) / probability
print("sample size=", sampleSize)
print("relative error to Pi=%e" % relativeError)

# Test generate(size) consistency with per-point generate
sequence = ot.ReverseHaltonSequence(5)
sample_bulk = sequence.generate(100)
sequence = ot.ReverseHaltonSequence(5)
sample_loop = ot.Sample(0, 5)
for i in range(100):
    sample_loop.add(sequence.generate())
ott.assert_almost_equal(sample_bulk, sample_loop)
