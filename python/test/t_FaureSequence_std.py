#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()


# Create a Faure
sequence = ot.FaureSequence(15)
print(sequence)

# Create a numerical sample of the sequence
faureSample = sequence.generate(10)
print(repr(faureSample))

# Create another Faure' sequence of dimension 2 to estimate Pi in [0 1)^2
dimension = 2
sequence = ot.FaureSequence(dimension)
pointInsideCircle = 0
sampleSize = 1000
for i in range(sampleSize):
    faurePoint = sequence.generate()
    print(faurePoint.__repr__())
    if faurePoint.norm() < 1.0:
        pointInsideCircle = pointInsideCircle + 1
probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
probability = m.pi / 4.0
relativeError = abs(probability - probabilityEstimate) / probability
print("sample size=", sampleSize)
print("relative error to Pi=%e" % relativeError)
