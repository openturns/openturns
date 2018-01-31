#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:

    try:
        # Function
        sampleSize = 20
        levelFunction = SymbolicFunction(
            ["x1", "x2", "x3", "x4"], ["x1+2*x2-3*x3+4*x4"])
        myOperator = LessOrEqual()
        threshold = 2.0
        mySample = Sample(0, levelFunction.getInputDimension())
        random = 0.1

        for index in range(sampleSize):
            point = Point(levelFunction.getInputDimension())
            norm = 0.0
            for coordinate in range(levelFunction.getInputDimension()):
                point[coordinate] = sqrt(-2.0 * log(random))
                random = fmod(random + sqrt(2.0), 1.0)
                point[coordinate] *= cos(2.0 * atan(1.0) * random)
                norm += point[coordinate] * point[coordinate]

            for coordinate in range(levelFunction.getInputDimension()):
                point[coordinate] /= sqrt(norm)

            mySample.add(point)

        myNearestPointChecker = NearestPointChecker(
            levelFunction, myOperator, threshold, mySample)
        for index in range(sampleSize):
            print(repr(mySample[index]))
        myNearestPointChecker.run()
        print("myNearestPointChecker = ", myNearestPointChecker)
    except:
        raise

except:
    import sys
    print("t_NearestPointChecker_std.py", sys.exc_info()[0], sys.exc_info()[1])
