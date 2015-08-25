#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()


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

# TESTPREAMBLE()

try:
    levelFunction = NumericalMathFunction(
        ["x1", "x2", "x3", "x4"], ["y1"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
    specific = CobylaSpecificParameters()
    startingPoint = NumericalPoint(4, 0.0)
    myAlgorithm = Cobyla(specific, levelFunction)
    myAlgorithm.setStartingPoint(startingPoint)
    myAlgorithm.setLevelValue(3.0)
    myAlgorithm.setMaximumIterationsNumber(400)
    myAlgorithm.setMaximumAbsoluteError(1.0e-10)
    myAlgorithm.setMaximumRelativeError(1.0e-10)
    myAlgorithm.setMaximumResidualError(1.0e-10)
    myAlgorithm.setMaximumConstraintError(1.0e-10)
    myAlgorithm.run()

    print("result = ", printNumericalPoint(
        myAlgorithm.getResult().getOptimalPoint(), 4))
    # print "evaluation history X=", myAlgorithm.getResult().getInputSample()
    # print "evaluation history Y=", myAlgorithm.getResult().getOutputSample()
    # print "absolute error history=", myAlgorithm.getResult().getAbsoluteErrorHistory()
    # print "relative error history=", myAlgorithm.getResult().getRelativeErrorHistory()
    # print "residual error history=", myAlgorithm.getResult().getResidualErrorHistory()
    # print "constraint error history=",
    # myAlgorithm.getResult().getConstraintErrorHistory()
except:
    import sys
    print("t_Cobyla_nonlinear.py", sys.exc_info()[0], sys.exc_info()[1])
