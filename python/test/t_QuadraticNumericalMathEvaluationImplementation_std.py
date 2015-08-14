#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    inputDimension = 3
    outputDimension = 2
    # Center
    center = NumericalPoint(inputDimension)
    center[0] = -1
    center[1] = 0.5
    center[2] = 1
    # Constant term
    constant = NumericalPoint(outputDimension)
    constant[0] = -1.0
    constant[1] = 2.0
    # Linear term
    linear = Matrix(inputDimension, outputDimension)
    linear[0, 0] = 1.0
    linear[1, 0] = 2.0
    linear[2, 0] = 3.0
    linear[0, 1] = 4.0
    linear[1, 1] = 5.0
    linear[2, 1] = 6.0
    # Quadratic term
    quadratic = SymmetricTensor(inputDimension, outputDimension)
    quadratic[0, 0, 0] = 7.0
    quadratic[0, 0, 1] = -7.0
    quadratic[0, 1, 0] = 8.0
    quadratic[0, 1, 1] = -8.0
    quadratic[0, 2, 0] = 9.0
    quadratic[0, 2, 1] = -9.0
    quadratic[1, 0, 0] = 8.0
    quadratic[1, 0, 1] = -8.0
    quadratic[1, 1, 0] = 10.0
    quadratic[1, 1, 1] = -10.0
    quadratic[1, 2, 0] = 11.0
    quadratic[1, 2, 1] = -11.0
    quadratic[2, 0, 0] = 9.0
    quadratic[2, 0, 1] = -9.0
    quadratic[2, 1, 0] = 11.0
    quadratic[2, 1, 1] = -11.0
    quadratic[2, 2, 0] = 12.0
    quadratic[2, 2, 1] = -12.0

    myFunction = QuadraticNumericalMathEvaluationImplementation(
        center, constant, linear, quadratic)
    myFunction.setName("quadraticFunction")
    inPoint = NumericalPoint(inputDimension)
    inPoint[0] = 7.0
    inPoint[1] = 8.0
    inPoint[2] = 9.0
    outPoint = myFunction(inPoint)
    print("myFunction=", repr(myFunction))
    print(myFunction.getName(), "( ", repr(inPoint), " ) = ", repr(outPoint))

except:
    import sys
    print("t_QuadraticNumericalMathEvaluationImplementation_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
