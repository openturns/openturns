#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

myFunc = ot.FieldToPointFunction()

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the spatial, input and output dimension
print("myFunc input dimension=", myFunc.getInputMesh().getDimension())
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")
