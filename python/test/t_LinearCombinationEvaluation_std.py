#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # First, build two functions from R^3->R
    inVar = Description(3)
    inVar[0] = "x1"
    inVar[1] = "x2"
    inVar[2] = "x3"
    outVar = Description(1)
    outVar[0] = "y"
    formula = Description(1)
    formula[0] = "x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)"
    functions = []
    functions.append(SymbolicFunction(inVar, formula))
    formula[0] = "exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)"
    functions.append(SymbolicFunction(inVar, formula))
    # Second, build the weights
    coefficients = Point(2)
    coefficients[0] = 0.3
    coefficients[1] = 2.9
    myEvaluation = LinearCombinationEvaluation(
        functions, coefficients)
    inPoint = Point(3)
    inPoint[0] = 1.2
    inPoint[1] = 2.3
    inPoint[2] = 3.4
    print("myEvaluation=", myEvaluation)
    print("Value at ", inPoint, "=", myEvaluation(inPoint))

except:
    import sys
    print("t_LinearCombinationEvaluation_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
