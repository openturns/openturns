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
    levelFunction = NumericalMathFunction(
        ["x1", "x2", "x3", "x4"], ["y1"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
    # Activate the cache as we will use an analytical method
    levelFunction.enableCache()
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
    myAlgorithm.setLevelValue(-0.5)
    print("myalgorithm=", repr(myAlgorithm))
    myAlgorithm.run()
    print("result = ", printNumericalPoint(
        myAlgorithm.getResult().getMinimizer(), 4))
    print("evaluation calls number=", levelFunction.getEvaluationCallsNumber())
    print("gradient   calls number=", levelFunction.getGradientCallsNumber())
    print("hessian    calls number=", levelFunction.getHessianCallsNumber())
except:
    import sys
    print("t_AbdoRackwitz_nonlinear.py", sys.exc_info()[0], sys.exc_info()[1])
