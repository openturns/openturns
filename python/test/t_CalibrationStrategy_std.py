#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:

    lowerBound = 0.3
    upperBound = 0.5
    ii = Interval(lowerBound, upperBound)
    myAlgo = CalibrationStrategy(Interval(lowerBound, upperBound))

    for x in (0.1, 0.3, 0.5, 0.7):
        updateFactor = myAlgo.computeUpdateFactor(x)
        print("updateFactor=", updateFactor)

except:
    import sys
    print("t_CalibrationStrategy_std.py", sys.exc_info()[0], sys.exc_info()[1])
