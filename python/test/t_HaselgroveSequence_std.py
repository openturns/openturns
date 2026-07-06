#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()


# Create a Haselgrove
sequence = ot.HaselgroveSequence(15)
print(repr(sequence))

# Create a numerical sample of the sequence
haselgroveSample = sequence.generate(10)
print(repr(haselgroveSample))

# Create another Haselgrove' sequence of dimension 2 to estimate Pi in [0
# 1)^2
dimension = 2
sequence = ot.HaselgroveSequence(dimension)
pointInsideCircle = 0
sampleSize = 1000
for i in range(sampleSize):
    haselgrovePoint = sequence.generate()
    print(haselgrovePoint.__repr__())
    if haselgrovePoint.norm() < 1.0:
        pointInsideCircle = pointInsideCircle + 1
probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
probability = m.pi / 4.0
relativeError = abs(probability - probabilityEstimate) / probability
print("sample size=", sampleSize)
print("relative error to Pi=%e" % relativeError)

# Test generate(size) consistency with per-point generate
sequence = ot.HaselgroveSequence(5)
sample_bulk = sequence.generate(100)
sequence = ot.HaselgroveSequence(5)
sample_loop = ot.Sample(0, 5)
for i in range(100):
    sample_loop.add(sequence.generate())
ott.assert_almost_equal(sample_bulk, sample_loop)
