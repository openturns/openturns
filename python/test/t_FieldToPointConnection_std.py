#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# Create a KarhunenLoeveResult
mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
projection = ot.KarhunenLoeveProjection(result)
# Construction based on a FieldFunction followed by a FieldToPointFunction
fieldFunction = ot.ValueFunction(ot.SymbolicFunction("x", "x"), mesh)
# Create an instance
myFunc = ot.FieldToPointConnection(projection, fieldFunction)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a field
field = result.getModesAsProcessSample().computeMean()
print("field=", field)
print("myFunc(field)=", myFunc(field.getValues()))
print("called ", myFunc.getCallsNumber(), " times")
# Connection on a sample
sample = result.getModesAsProcessSample()
print("sample=", sample)
Y = myFunc(sample)
print("myFunc(sample)=", len(Y), Y.getDimension())
print("called ", myFunc.getCallsNumber(), " times")

# Construction based on a FieldToPointFunction followed by a Function
dimension = projection.getOutputDimension()
function = ot.SymbolicFunction(ot.Description.BuildDefault(
    dimension, "x"), ot.Description.BuildDefault(dimension, "x"))
# Create an instance
myFunc = ot.FieldToPointConnection(function, projection)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a field
field = result.getModesAsProcessSample().computeMean()
print("field=", field)
print("myFunc(field)=", myFunc(field.getValues()))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")
