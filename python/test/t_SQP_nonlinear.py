#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()


def printNumericalPoint(point, digits):
    oss = "["
    eps = pow(0.1, digits)
    for i in range(point.getDimension()):
        if i == 0:
            sep = ""
        else:
            sep = ","
        if fabs(point[i]) < eps:
            oss += sep + "%.6f" % point[i]
        else:
            oss += sep + "%.6f" % point[i]
        sep = ","
    oss += "]"
    return oss

# TESTPREAMBLE()

try:
    try:
        # Test function operator ()
        levelFunction = NumericalMathFunction(
            ["x1", "x2", "x3", "x4"], ["y1"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
        # Add a finite difference gradient to the function,
        # needs it
        myGradient = NonCenteredFiniteDifferenceGradient(
            1e-7, levelFunction.getEvaluation())
        # Substitute the gradient
        levelFunction.setGradient(
            NonCenteredFiniteDifferenceGradient(myGradient))
        specific = SQPSpecificParameters()
        startingPoint = NumericalPoint(4, 0.0)
        myAlgorithm = SQP(specific, levelFunction)
        myAlgorithm.setStartingPoint(startingPoint)
        myAlgorithm.setLevelValue(-0.5)
        print("myAlgorithm=", myAlgorithm)
        myAlgorithm.run()
        print("result = ", printNumericalPoint(
            myAlgorithm.getResult().getMinimizer(), 4))
    except:
        raise
except:
    import sys
    print("t_SQP_nonlinear.py", sys.exc_info()[0], sys.exc_info()[1])
