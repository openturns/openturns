#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import print_function
import openturns as ot
import openturns.testing as ott


def printResults(result, problemName):
    print("*** {} completed:".format(problemName))
    print("      -- Optimal point = ", result.getOptimalPoint())
    print("      -- Optimal value = ", result.getOptimalValue())
    print("      -- Iteration number = ", result.getIterationNumber())
    print("      -- Evaluation number = ", result.getEvaluationNumber())
    print("      -- Absolute error = {:.6e}".format(result.getAbsoluteError()))
    print("      -- Relative error = {:.6e}".format(result.getRelativeError()))
    print("      -- Residual error = {:.6e}".format(result.getResidualError()))
    print(
        "      -- Constraint error = {:.6e}".format(result.getConstraintError()))


n = 3
m = 20
x = [[0.5 + 0.1*i] for i in range(m)]

model = ot.SymbolicFunction(['a', 'b', 'c', 'x'], ['a + b * exp(-c *x^2)'])
p_ref = [2.8, 1.2, 0.5]  # Reference a, b, c
modelx = ot.ParametricFunction(model, [0, 1, 2], p_ref)
y = modelx(x)
ynoise = ot.Sample([ot.Normal(1.0, 0.05).getRealization()[0] * modelx(x)[i]
                    for i in range(m)])  # Generate sample with noise

# Define residual functions


def residualFunction(params):
    modelx = ot.ParametricFunction(model, [0, 1, 2], params)
    return [modelx(x[i])[0] - y[i, 0] for i in range(m)]


def residualFunctionNoise(params):
    modelx = ot.ParametricFunction(model, [0, 1, 2], params)
    return [modelx(x[i])[0] - ynoise[i, 0] for i in range(m)]


# Definition of residual as ot.PythonFunction and optimization problem
residual = ot.PythonFunction(n, m, residualFunction)
residualNoise = ot.PythonFunction(n, m, residualFunctionNoise)

lsqProblem = ot.LeastSquaresProblem(residual)
lsqNoiseProblem = ot.LeastSquaresProblem(residualNoise)

startingPoint = [0.0, 0.0, 0.0]

# LSQ SOLVER
# Definition of Dlib solver, setting starting point
lsqAlgo = ot.Dlib(lsqProblem, "LSQ")
lsqAlgo.setStartingPoint(startingPoint)
lsqAlgo.run()

# Retrieving results
lsqResult = lsqAlgo.getResult()
printResults(lsqResult, "LSQ (without noise)")

# Same with noise
lsqNoiseAlgo = ot.Dlib(lsqNoiseProblem, "LSQ")
lsqNoiseAlgo.setStartingPoint(startingPoint)
lsqNoiseAlgo.run()
lsqNoiseResult = lsqNoiseAlgo.getResult()
printResults(lsqNoiseResult, "LSQ (with noise)")


# LSQLM SOLVER
# Definition of Dlib solver, setting starting point
lsqlmAlgo = ot.Dlib(lsqProblem, "LSQLM")
lsqlmAlgo.setStartingPoint(startingPoint)
lsqlmAlgo.run()

# Retrieving results
lsqlmResult = lsqlmAlgo.getResult()
printResults(lsqlmResult, "LSQLM (without noise)")


# Same with noise
lsqlmNoiseAlgo = ot.Dlib(lsqNoiseProblem, "LSQLM")
lsqlmNoiseAlgo.setStartingPoint(startingPoint)
lsqlmNoiseAlgo.run()
lsqlmNoiseResult = lsqlmNoiseAlgo.getResult()
printResults(lsqlmNoiseResult, "LSQLM (with noise)")


# Check results
ott.assert_almost_equal(lsqResult.getOptimalPoint(), p_ref, 5e-2)
ott.assert_almost_equal(lsqNoiseResult.getOptimalPoint(), p_ref, 5e-2)
ott.assert_almost_equal(lsqlmResult.getOptimalPoint(), p_ref, 5e-2)
ott.assert_almost_equal(lsqlmNoiseResult.getOptimalPoint(), p_ref, 5e-2)
