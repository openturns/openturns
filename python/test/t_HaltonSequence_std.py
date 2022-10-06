#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()

# Create a Halton
sequence = ot.HaltonSequence(15)
print(repr(sequence))

# Create a numerical sample of the sequence
haltonSample = sequence.generate(10)
print(repr(haltonSample))

# Create another Halton' sequence of dimension 2 to estimate Pi in [0 1)^2
dimension = 2
sequence = ot.HaltonSequence(dimension)
pointInsideCircle = 0
sampleSize = 1000
for i in range(sampleSize):
    haltonPoint = sequence.generate()
    print(haltonPoint.__repr__())
    if haltonPoint.norm() < 1.0:
        pointInsideCircle = pointInsideCircle + 1
probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
probability = m.pi / 4.0
relativeError = abs(probability - probabilityEstimate) / probability
print('sample size=', sampleSize)
print('relative error to Pi=%e' % relativeError)
