#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Construction based on two functions
# Create an instance
grid = ot.RegularGrid(0.0, 0.1, 11)
myFunc = ot.FieldToFieldConnection(
    ot.ValueFunction(ot.SymbolicFunction("x", "sin(x)"), grid), ot.ValueFunction(ot.SymbolicFunction("x", "cos(x)"), grid))

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a point
field = ot.Field(grid, grid.getVertices() * [2.0])
print("field=", field)
print("myFunc(field)=", myFunc(field))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")

# Construction based on a FieldToPointFunction followed by a PointToFieldFunction
# Create a KarhunenLoeveResult
mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
# Create a PointToFieldFunction
lifting = ot.KarhunenLoeveLifting(result)
# Create a FieldToPointFunction
projection = ot.KarhunenLoeveProjection(result)
# Create an instance
myFunc = ot.FieldToFieldConnection(lifting, projection)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a point
field = result.getModesAsProcessSample().computeMean()
print("field=", field)
print("myFunc(field)=", myFunc(field))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")

