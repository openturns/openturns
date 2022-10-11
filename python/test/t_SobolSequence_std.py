#! /usr/bin/env python

import openturns as ot
import math as m

ot.TESTPREAMBLE()


# Create a Sobol' sequence of maximum dimension
sequence = ot.SobolSequence(ot.SobolSequence.MaximumDimension)
print(sequence)

# Create a numerical sample of the sequence
sobolSample = sequence.generate(10)
print(repr(sobolSample))

# Create another Sobol' sequence of dimension 2 to estimate Pi in [0 1)^2
dimension = 2
sequence = ot.SobolSequence(dimension)
pointInsideCircle = 0
sampleSize = 1000
for i in range(sampleSize):
    sobolPoint = sequence.generate()
    print(sobolPoint.__repr__())
    if sobolPoint.norm() < 1.0:
        pointInsideCircle = pointInsideCircle + 1
probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
probability = m.pi / 4.0
relativeError = abs(probability - probabilityEstimate) / probability
print("sample size=", sampleSize)
print("relative error to Pi=%e" % relativeError)
