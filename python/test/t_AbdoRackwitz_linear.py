#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *


def printNumericalPoint(point, digits):
    oss = "["
    eps = pow(0.1, digits)
    for i in range(point.getDimension()):
        if i == 0:
            sep = ""
        else:
            sep = ","
        if fabs(point[i]) < eps:
            oss += sep + "%.6f" % fabs(point[i])
        else:
            oss += sep + "%.6f" % point[i]
        sep = ","
    oss += "]"
    return oss

TESTPREAMBLE()

levelFunction = NumericalMathFunction(
    ["x1", "x2", "x3", "x4"], ["y1"], ["x1+2*x2-3*x3+4*x4"])
# Add a finite difference gradient to the function, as Abdo Rackwitz algorithm
# needs it
myGradient = NonCenteredFiniteDifferenceGradient(
    1e-7, levelFunction.getEvaluation())
print("myGradient = ", repr(myGradient))
# Substitute the gradient
levelFunction.setGradient(
    NonCenteredFiniteDifferenceGradient(myGradient))
specific = AbdoRackwitzSpecificParameters()
startingPoint = NumericalPoint(4, 0.0)
myAlgorithm = AbdoRackwitz(specific, levelFunction)
myAlgorithm.setStartingPoint(startingPoint)
myAlgorithm.setLevelValue(3.0)
myAlgorithm.run()
print("result = ", printNumericalPoint(
    myAlgorithm.getResult().getMinimizer(), 4))
