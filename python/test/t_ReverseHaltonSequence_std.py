#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:

    # Create a ReverseHalton
    sequence = ReverseHaltonSequence(15)
    print(sequence)

    # Create a numerical sample of the sequence
    reverseHaltonSample = sequence.generate(10)
    print(repr(reverseHaltonSample))

    # Create another ReverseHalton' sequence of dimension 2 to estimate Pi in
    # [0 1)^2
    dimension = 2
    sequence = ReverseHaltonSequence(dimension)
    pointInsideCircle = 0
    sampleSize = 1000
    for i in range(sampleSize):
        reverseHaltonPoint = sequence.generate()
        print(reverseHaltonPoint.__repr__())
        if reverseHaltonPoint.norm() < 1.0:
            pointInsideCircle = pointInsideCircle + 1
    probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
    probability = pi / 4.0
    relativeError = fabs(probability - probabilityEstimate) / probability
    print('sample size=', sampleSize)
    print('relative error to Pi=%e' % relativeError)

except:
    import sys
    print("t_ReverseHaltonSequence_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
