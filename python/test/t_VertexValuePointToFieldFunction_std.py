#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Create an intance
g = ot.SymbolicFunction(["t", "x"], ["x + t^2", "5*x + 3*t"])
grid = ot.RegularGrid(0.0, 0.2, 6)
f = ot.VertexValuePointToFieldFunction(g, grid)

print("f=", f)
# Get the input and output description
print("f input description=",
      f.getInputDescription())
print("f output description=",
      f.getOutputDescription())
# Get the input and output dimension, based on description
print("f input dimension=",
      f.getInputDimension())
print("f output dimension=",
      f.getOutputDimension())
x = [4.0]
print("input=", x)
print("output time series=", f(x))
# Get the number of calls
print("called ", f.getCallsNumber(), " times")

x = [[3.0], [4.0], [5.0]]
print("input=", x)
print("output time series=", f(x))
# Get the number of calls
print("called ", f.getCallsNumber(), " times")
f1 = f.getMarginal(1)
print("f1=", f1)
