#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# First, build two functions from R^3->R
inVar = ['x1', 'x2', 'x3']
formula = ['x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)']
functions = []
functions.append(ot.SymbolicFunction(inVar, formula))
formula = ['exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)']
functions.append(ot.SymbolicFunction(inVar, formula))
# Second, build the weights
coefficients = [0.3, 2.9]
# Third, build the function
myFunction = ot.LinearCombinationFunction(functions, coefficients)
inPoint = ot.Point([1.2, 2.3, 3.4])
print('myFunction=', myFunction)
print('Value at ', inPoint, '=', myFunction(inPoint))
print('Gradient at ', inPoint, '=', myFunction.gradient(inPoint))
print('Hessian at ', inPoint, '=', myFunction.hessian(inPoint))
