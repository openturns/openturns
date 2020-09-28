#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()


# Create an instance
myFunc = ot.PointToPointEvaluation(
    ot.SymbolicFunction("x", "sin(x)"), ot.SymbolicFunction("x", "cos(x)"))

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Evaluation on a point
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
print("called ", myFunc.getCallsNumber(), " times")
# Evaluation on a sample
sample = [[1.0] * myFunc.getInputDimension()]*10
print("sample=", sample)
print("myFunc(sample)=", myFunc(sample))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")

# Construction based on a PointToFieldFunction followed by a FieldToPointFunction
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
myFunc = ot.PointToPointEvaluation(projection, lifting)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Evaluation on a point
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")
