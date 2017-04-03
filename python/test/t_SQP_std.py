#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(4)

# linear
levelFunction = ot.Function(
    ["x1", "x2", "x3", "x4"], ["y1"], ["x1+2*x2-3*x3+4*x4"])
# Add a finite difference gradient to the function
myGradient = ot.NonCenteredFiniteDifferenceGradient(
    1e-7, levelFunction.getEvaluation())
print("myGradient = ", repr(myGradient))
# Substitute the gradient
levelFunction.setGradient(
    ot.NonCenteredFiniteDifferenceGradient(myGradient))
startingPoint = ot.NumericalPoint(4, 0.0)
algo = ot.SQP(ot.OptimizationProblem(levelFunction, 3.0))
algo.setStartingPoint(startingPoint)
print('algo=', algo)
algo.run()
result = algo.getResult()
print('result=', result)


# non-linear
levelFunction = ot.Function(
    ["x1", "x2", "x3", "x4"], ["y1"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
# Add a finite difference gradient to the function,
# needs it
myGradient = ot.NonCenteredFiniteDifferenceGradient(
    1e-7, levelFunction.getEvaluation())
# Substitute the gradient
levelFunction.setGradient(
    ot.NonCenteredFiniteDifferenceGradient(myGradient))
startingPoint = ot.NumericalPoint(4, 0.0)
algo = ot.SQP(ot.OptimizationProblem(levelFunction, -0.5))
algo.setStartingPoint(startingPoint)
print('algo=', algo)
algo.run()
result = algo.getResult()
print('result=', result)
