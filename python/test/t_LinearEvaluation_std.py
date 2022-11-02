#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

inputDimension = 3
outputDimension = 2
# Center
center = ot.Point(inputDimension)
center[0] = -1
center[1] = 0.5
center[2] = 1
# Constant term
constant = ot.Point(outputDimension)
constant[0] = -1.0
constant[1] = 2.0
# Linear term
linear = ot.Matrix(inputDimension, outputDimension)
linear[0, 0] = 1.0
linear[1, 0] = 2.0
linear[2, 0] = 3.0
linear[0, 1] = 4.0
linear[1, 1] = 5.0
linear[2, 1] = 6.0

# myFunction = linear * (X- center) + constant
myFunction = ot.LinearEvaluation(center, constant, linear)
myFunction.setName("linearFunction")
inPoint = ot.Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outPoint = myFunction(inPoint)
print("myFunction=", repr(myFunction))
print(myFunction.getName(), "( ", repr(inPoint), " ) = ", repr(outPoint))
