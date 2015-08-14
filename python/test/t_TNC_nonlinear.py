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

try:
    levelFunction = NumericalMathFunction(
        ["x1", "x2", "x3", "x4"], ["y1"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
    specific = TNCSpecificParameters()
    startingPoint = NumericalPoint(4, -0.01)

    startingPointNearMinimizationCorner = NumericalPoint(4)
    startingPointNearMinimizationCorner[0] = 3.0
    startingPointNearMinimizationCorner[0] = -2.5
    startingPointNearMinimizationCorner[0] = 4.5
    startingPointNearMinimizationCorner[0] = -2.5
    startingPointNearMaximizationCorner = NumericalPoint(4)
    startingPointNearMaximizationCorner[0] = -2.5
    startingPointNearMaximizationCorner[1] = 4.5
    startingPointNearMaximizationCorner[2] = 4.5
    startingPointNearMaximizationCorner[3] = 4.5

    bounds = Interval(NumericalPoint(4, -3.0), NumericalPoint(4, 5.0))
    myAlgorithm = TNC(
        specific, levelFunction, bounds, startingPointNearMinimizationCorner,
        BoundConstrainedAlgorithmImplementationResult.MINIMIZATION)
    myAlgorithm.run()
    print("minimizer = ", printNumericalPoint(myAlgorithm.getResult().getOptimizer(), 4),
          " value=%.5e" % myAlgorithm.getResult().getOptimalValue())

    myAlgorithm = TNC(
        specific, levelFunction, bounds, startingPointNearMaximizationCorner,
        BoundConstrainedAlgorithmImplementationResult.MAXIMIZATION)
    myAlgorithm.run()
    print("maximizer = ", printNumericalPoint(myAlgorithm.getResult().getOptimizer(), 4),
          " value=%.5e" % myAlgorithm.getResult().getOptimalValue())

except:
    import sys
    print("t_TNC_nonlinear.py", sys.exc_info()[0], sys.exc_info()[1])
