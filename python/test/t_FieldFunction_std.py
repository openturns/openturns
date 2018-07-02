#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

# Create an intance
myFunc = ot.FieldFunction()

print('myFunc=', myFunc)
# Get the input and output description
print('myFunc input description=', myFunc.getInputDescription())
print('myFunc output description=', myFunc.getOutputDescription())
# Get the input and output dimension, based on description
print('myFunc input dimension=', myFunc.getInputDimension())
print('myFunc output dimension=', myFunc.getOutputDimension())
print('myFunc input dimension=', myFunc.getInputMesh().getDimension())
# Get the number of calls
print('called ', myFunc.getCallsNumber(), ' times')
