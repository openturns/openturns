#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
ot.TESTPREAMBLE()

levelFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["x1+2*x2-3*x3+4*x4"])
# Add a finite difference gradient to the function, as Abdo Rackwitz algorithm
# needs it
myGradient = ot.NonCenteredFiniteDifferenceGradient(
    1e-7, levelFunction.getEvaluation())
print("myGradient = ", repr(myGradient))
# Substitute the gradient
levelFunction.setGradient(
    ot.NonCenteredFiniteDifferenceGradient(myGradient))
startingPoint = [0.0] * 4
algo = ot.AbdoRackwitz(ot.NearestPointProblem(levelFunction, 3.0))
algo.setStartingPoint(startingPoint)
algo.run()
print("result = ", algo.getResult())

levelFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
# Add a finite difference gradient to the function, as Abdo Rackwitz algorithm
# needs it
myGradient = ot.NonCenteredFiniteDifferenceGradient(
    1e-7, levelFunction.getEvaluation())
print("myGradient = ", repr(myGradient))
# Substitute the gradient
levelFunction.setGradient(
    ot.NonCenteredFiniteDifferenceGradient(myGradient))
startingPoint = [0.0] * 4
algo = ot.AbdoRackwitz(ot.NearestPointProblem(levelFunction, -0.5))
algo.setStartingPoint(startingPoint)
print("myalgorithm=", repr(algo))
algo.run()
print("result = ",  algo.getResult())
print("evaluation calls number=", levelFunction.getEvaluationCallsNumber())
print("gradient   calls number=", levelFunction.getGradientCallsNumber())
print("hessian    calls number=", levelFunction.getHessianCallsNumber())
