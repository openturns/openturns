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


# Define the problems based on Rastrigin function
rastrigin = ot.SymbolicFunction(
    ['x1', 'x2'], ['20 + x1^2 - 10*cos(2*pi_*x1) + x2^2 - 10*cos(2*pi_*x2)'])

unboundedProblem = ot.OptimizationProblem(rastrigin)

notConstrainingBounds = ot.Interval([-5.0, -5.0], [3.0, 2.0])
notConstrainingBoundsProblem = ot.OptimizationProblem(
    rastrigin, ot.Function(), ot.Function(), notConstrainingBounds)

constrainingBounds = ot.Interval([-1.0, -2.0], [5.0, -0.5])
constrainingBoundsProblem = ot.OptimizationProblem(
    rastrigin, ot.Function(), ot.Function(), constrainingBounds)

boundedPref = [0.0, -1.0]
unboundedPref = [0.0, 0.0]

## GLOBAL ALGORITHM ##

# Non-contraining bounds Global
notConstrainingBoundsGlobal = ot.Dlib(notConstrainingBoundsProblem, "Global")
notConstrainingBoundsGlobal.setStartingPoint([0.0]*2)
notConstrainingBoundsGlobal.setMaximumEvaluationNumber(300)
notConstrainingBoundsGlobal.run()
printResults(notConstrainingBoundsGlobal.getResult(),
             "Non-constraining bounds Global")

# Contraining bounds Global
constrainingBoundsGlobal = ot.Dlib(constrainingBoundsProblem, "Global")
constrainingBoundsGlobal.setMaximumEvaluationNumber(300)
constrainingBoundsGlobal.setStartingPoint([0.0]*2)
constrainingBoundsGlobal.run()
printResults(constrainingBoundsGlobal.getResult(),
             "Constraining bounds Global")

ott.assert_almost_equal(notConstrainingBoundsGlobal.getResult(
).getOptimalPoint(), unboundedPref, 1, 1e-4)
ott.assert_almost_equal(
    constrainingBoundsGlobal.getResult().getOptimalPoint(), boundedPref, 1, 1e-4)
