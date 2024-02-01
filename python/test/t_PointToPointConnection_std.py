#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Construction based on two functions
# Create an instance */
myFunc = ot.PointToPointConnection(
    ot.SymbolicFunction("x", "sin(x)"), ot.SymbolicFunction("x", "cos(x)")
)

print("myFunc=", myFunc)
# Get the input and output description */
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension */
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a point */
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
print("myFunc.gradient(point)=", myFunc.gradient(point))
print("myFunc.hessian(point)=", myFunc.hessian(point))
# Get the number of calls */
print("called ", myFunc.getEvaluationCallsNumber(), " times")

# Construction based on a PointToFieldFunction followed by a FieldToPointFunction
# Create a KarhunenLoeveResult
mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 1.0e-2)
algo.run()
result = algo.getResult()
# Create a PointToFieldFunction
lifting = ot.KarhunenLoeveLifting(result)
# Create a FieldToPointFunction
projection = ot.KarhunenLoeveProjection(result)
# Create an instance */
myFunc = ot.PointToPointConnection(projection, lifting)

print("myFunc=", myFunc)
# Get the input and output description */
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension */
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a point */
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
print("myFunc.gradient(point)=", myFunc.gradient(point).clean(1.0e-8))
print("myFunc.hessian(point)=", myFunc.hessian(point).clean(1.0e-7))
# Get the number of calls */
print("called ", myFunc.getEvaluationCallsNumber(), " times")
