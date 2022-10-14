#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Create an intance
myFunc = ot.PointToFieldFunction()

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")
