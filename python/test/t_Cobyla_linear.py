#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *


def printNumericalPoint(point, digits):
    oss = "["
    eps = pow(0.1, digits)
    format = "%." + str(digits) + "f"
    for i in range(point.getDimension()):
        if i == 0:
            sep = ""
        else:
            sep = ","
        if fabs(point[i]) < eps:
            oss += sep + format % fabs(point[i])
        else:
            oss += sep + format % point[i]
        sep = ","
    oss += "]"
    return oss

TESTPREAMBLE()

try:
    levelFunction = NumericalMathFunction(
        ["x1", "x2", "x3", "x4"], ["y1"], ["x1+2*x2-3*x3+4*x4"])
    specific = CobylaSpecificParameters()
    startingPoint = NumericalPoint(4, 0.0)
    myAlgorithm = CobylaObsolete(specific, levelFunction)
    myAlgorithm.setStartingPoint(startingPoint)
    myAlgorithm.setLevelValue(3.0)
    myAlgorithm.run()

    print("result = ", printNumericalPoint(
        myAlgorithm.getResult().getMinimizer(), 4))

except:
    import sys
    print("t_Cobyla_linear.py", sys.exc_info()[0], sys.exc_info()[1])
