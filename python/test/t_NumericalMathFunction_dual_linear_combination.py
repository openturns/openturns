#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # First, build two functions from R^3->R
    functions = list()
    functions.append(NumericalMathFunction(["x1", "x2", "x3"], [
                     "y"], ["x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)"]))
    functions.append(NumericalMathFunction(
        ["x1", "x2", "x3"], ["y"], ["exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)"]))
    # Second, build the weights
    coefficients = NumericalSample(0, 3)
    coefficients.add([1.5, 2.5, -0.5])
    coefficients.add([-3.5, 0.5, -1.5])
    # Third, build the function
    myFunction = NumericalMathFunction(functions, coefficients)
    inPoint = NumericalPoint([1.2, 2.3, 3.4])
    print("myFunction=", myFunction)
    print("Value at ", inPoint, "=", myFunction(inPoint))
    print("Gradient at ", inPoint, "=", myFunction.gradient(inPoint))
    PlatformInfo.SetNumericalPrecision(5)
    print("Hessian at ", inPoint, "=", myFunction.hessian(inPoint))
    for i in range(myFunction.getOutputDimension()):
        print("Marginal ", i, "=", myFunction.getMarginal(i))
    print("Marginal (0,1)=", myFunction.getMarginal([0, 1]))
    print("Marginal (0,2)=", myFunction.getMarginal([0, 2]))
    print("Marginal (1,2)=", myFunction.getMarginal([1, 2]))

except:
    import sys
    print("t_NumericalMathFunction_dual_linear_combination.py",
          sys.exc_info()[0], sys.exc_info()[1])
