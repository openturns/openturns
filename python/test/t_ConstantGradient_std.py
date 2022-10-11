#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


inputDimension = 3
outputDimension = 2
# Constant term
constant = ot.Matrix(inputDimension, outputDimension)
constant[0, 0] = 1.0
constant[1, 0] = 2.0
constant[2, 0] = 5.0
constant[0, 1] = 7.0
constant[1, 1] = 9.0
constant[2, 1] = 3.0

myGradient = ot.ConstantGradient(constant)
myGradient.setName("constantGradient")
inPoint = ot.Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outMatrix = myGradient.gradient(inPoint)
print("myGradient=", repr(myGradient))
print(myGradient.getName(), "( ", repr(inPoint), " ) = ", repr(outMatrix))
