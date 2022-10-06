#! /usr/bin/env python

import openturns as ot
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
print('sample size=', sampleSize)
print('relative error to Pi=%e' % relativeError)
