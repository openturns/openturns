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
        ["x1", "x2", "x3", "x4"], ["y1"], ["x1+2*x2-3*x3+4*x4"])
    specific = TNCSpecificParameters()
    startingPoint = NumericalPoint(4, 0.0)
    bounds = Interval(NumericalPoint(4, -3.0), NumericalPoint(4, 5.0))
    myAlgorithm = TNC()
    myAlgorithm.setStartingPoint(startingPoint)

    problem = OptimizationProblem()
    problem.setBounds(bounds)
    problem.setObjective(levelFunction)
    problem.setMinimization(True)

    myAlgorithm.setProblem(problem)
    myAlgorithm.run()
    print("minimizer = ", printNumericalPoint(myAlgorithm.getResult().getOptimalPoint(), 4),
          " value=%g" % myAlgorithm.getResult().getOptimalValue()[0])

    problem.setMinimization(False)
    myAlgorithm.setProblem(problem)
    myAlgorithm.run()
    print("maximizer = ", printNumericalPoint(myAlgorithm.getResult().getOptimalPoint(), 4),
          " value=%g" % myAlgorithm.getResult().getOptimalValue()[0])

except:
    import sys
    print("t_TNC_linear.py", sys.exc_info()[0], sys.exc_info()[1])
