#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Create a KarhunenLoeveResult
mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
lifting = ot.KarhunenLoeveLifting(result)
# Construction based on a Function followed by a PointToFieldFunction
dimension = lifting.getInputDimension()
function = ot.SymbolicFunction(
    ot.Description.BuildDefault(dimension, "x"),
    ot.Description.BuildDefault(dimension, "x"),
)
# Create an instance
myFunc = ot.PointToFieldConnection(lifting, function)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a point
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")

# Construction based on a PointToFieldFunction followed by a
# FieldToPointFunction
fieldFunction = ot.ValueFunction(ot.SymbolicFunction("x", "x"), mesh)
# Create an instance
myFunc = ot.PointToFieldConnection(fieldFunction, lifting)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a point
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")
