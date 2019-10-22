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


# Define the problems based on Rosebrock function
rosenbrock = ot.SymbolicFunction(['x1', 'x2'], ['(1-x1)^2+100*(x2-x1^2)^2'])

unboundedProblem = ot.OptimizationProblem(rosenbrock)

notConstrainingBounds = ot.Interval([-5.0, -5.0], [5.0, 5.0])
notConstrainingBoundsProblem = ot.OptimizationProblem(
    rosenbrock, ot.Function(), ot.Function(), notConstrainingBounds)

constrainingBounds = ot.Interval([0.0, -2.0], [5.0, 0.5])
constrainingBoundsProblem = ot.OptimizationProblem(
    rosenbrock, ot.Function(), ot.Function(), constrainingBounds)

start = [3.0, -1.5]
unboundedPref = [1.0, 1.0]
boundedPref = [0.70856, 0.5]

## CONJUGATE GRADIENT ALGORITHM ##

# Unbounded CG
unboundedCg = ot.Dlib(unboundedProblem, 'CG')
unboundedCg.setStartingPoint(start)
unboundedCg.setMaximumIterationNumber(10000)
unboundedCg.setMaximumEvaluationNumber(100000)
unboundedCg.run()
printResults(unboundedCg.getResult(), 'Unbounded CG')

# Non-contraining bounds CG
notConstrainingBoundsCg = ot.Dlib(notConstrainingBoundsProblem, "CG")
notConstrainingBoundsCg.setStartingPoint(start)
notConstrainingBoundsCg.setMaximumIterationNumber(10000)
notConstrainingBoundsCg.setMaximumEvaluationNumber(100000)
notConstrainingBoundsCg.run()
printResults(notConstrainingBoundsCg.getResult(), "Non-constraining bounds CG")

# Contraining bounds CG
constrainingBoundsCg = ot.Dlib(constrainingBoundsProblem, "CG")
constrainingBoundsCg.setStartingPoint(start)
constrainingBoundsCg.setMaximumIterationNumber(10000)
constrainingBoundsCg.setMaximumEvaluationNumber(100000)
constrainingBoundsCg.run()
printResults(constrainingBoundsCg.getResult(), "Constraining bounds CG")


## BFGS ALGORITHM ##

# Unbounded BFGS
unboundedBfgs = ot.Dlib(unboundedProblem, "BFGS")
unboundedBfgs.setStartingPoint(start)
unboundedBfgs.setMaximumIterationNumber(10000)
unboundedBfgs.setMaximumEvaluationNumber(100000)
unboundedBfgs.run()
printResults(unboundedBfgs.getResult(), "Unbounded BFGS")

# Non-contraining bounds BFGS
notConstrainingBoundsBfgs = ot.Dlib(notConstrainingBoundsProblem, "BFGS")
notConstrainingBoundsBfgs.setStartingPoint(start)
notConstrainingBoundsBfgs.setMaximumIterationNumber(10000)
notConstrainingBoundsBfgs.setMaximumEvaluationNumber(100000)
notConstrainingBoundsBfgs.run()
printResults(notConstrainingBoundsBfgs.getResult(),
             "Non-constraining bounds BFGS")

# Contraining bounds BFGS
constrainingBoundsBfgs = ot.Dlib(constrainingBoundsProblem, "BFGS")
constrainingBoundsBfgs.setStartingPoint(start)
constrainingBoundsBfgs.setMaximumIterationNumber(10000)
constrainingBoundsBfgs.setMaximumEvaluationNumber(100000)
constrainingBoundsBfgs.run()
printResults(constrainingBoundsBfgs.getResult(), "Constraining bounds BFGS")


## LBFGS ALGORITHM ##

# Unbounded LBFGS
unboundedLbfgs = ot.Dlib(unboundedProblem, "LBFGS")
unboundedLbfgs.setStartingPoint(start)
unboundedLbfgs.setMaximumIterationNumber(10000)
unboundedLbfgs.setMaximumEvaluationNumber(100000)
unboundedLbfgs.setMaxSize(15)
unboundedLbfgs.run()
printResults(unboundedLbfgs.getResult(), "Unbounded LBFGS")

# Non-contraining bounds LBFGS
notConstrainingBoundsLbfgs = ot.Dlib(notConstrainingBoundsProblem, "LBFGS")
notConstrainingBoundsLbfgs.setStartingPoint(start)
notConstrainingBoundsLbfgs.setMaximumIterationNumber(10000)
notConstrainingBoundsLbfgs.setMaximumEvaluationNumber(100000)
notConstrainingBoundsLbfgs.setMaxSize(15)
notConstrainingBoundsLbfgs.run()
printResults(notConstrainingBoundsLbfgs.getResult(),
             "Non-constraining bounds LBFGS")

# Contraining bounds LBFGS
constrainingBoundsLbfgs = ot.Dlib(constrainingBoundsProblem, "LBFGS")
constrainingBoundsLbfgs.setStartingPoint(start)
constrainingBoundsLbfgs.setMaximumIterationNumber(10000)
constrainingBoundsLbfgs.setMaximumEvaluationNumber(100000)
constrainingBoundsLbfgs.setMaxSize(15)
constrainingBoundsLbfgs.run()


## NEWTON ALGORITHM ##

# Unbounded BFGS
unboundedNewton = ot.Dlib(unboundedProblem, "Newton")
unboundedNewton.setStartingPoint(start)
unboundedNewton.setMaximumIterationNumber(10000)
unboundedNewton.setMaximumEvaluationNumber(100000)
unboundedNewton.run()
printResults(unboundedNewton.getResult(), "Unbounded Newton")

# Non-contraining bounds Newton
notConstrainingBoundsNewton = ot.Dlib(notConstrainingBoundsProblem, "Newton")
notConstrainingBoundsNewton.setStartingPoint(start)
notConstrainingBoundsNewton.setMaximumIterationNumber(10000)
notConstrainingBoundsNewton.setMaximumEvaluationNumber(100000)
notConstrainingBoundsNewton.run()
printResults(notConstrainingBoundsNewton.getResult(),
             "Non-constraining bounds Newton")

# Contraining bounds Newton
constrainingBoundsNewton = ot.Dlib(constrainingBoundsProblem, "Newton")
constrainingBoundsNewton.setStartingPoint(start)
constrainingBoundsNewton.setMaximumIterationNumber(10000)
constrainingBoundsNewton.setMaximumEvaluationNumber(100000)
constrainingBoundsNewton.run()
printResults(constrainingBoundsNewton.getResult(),
             "Constraining bounds Newton")


## TRUST REGION ALGORITHM ##
unboundedTrustRegion = ot.Dlib(unboundedProblem, "TrustRegion")
unboundedTrustRegion.setStartingPoint(start)
unboundedTrustRegion.setMaximumIterationNumber(10000)
unboundedTrustRegion.setMaximumEvaluationNumber(100000)
unboundedTrustRegion.run()
printResults(unboundedTrustRegion.getResult(), "Unbounded TrustRegion")


ott.assert_almost_equal(
    unboundedCg.getResult().getOptimalPoint(), unboundedPref, 5e-2)
ott.assert_almost_equal(
    notConstrainingBoundsCg.getResult().getOptimalPoint(), unboundedPref, 5e-2)
ott.assert_almost_equal(
    constrainingBoundsCg.getResult().getOptimalPoint(), boundedPref, 5e-2)
ott.assert_almost_equal(
    unboundedBfgs.getResult().getOptimalPoint(), unboundedPref, 5e-2)
ott.assert_almost_equal(
    notConstrainingBoundsBfgs.getResult().getOptimalPoint(), unboundedPref, 5e-2)
ott.assert_almost_equal(
    constrainingBoundsBfgs.getResult().getOptimalPoint(), boundedPref, 5e-2)
ott.assert_almost_equal(
    unboundedLbfgs.getResult().getOptimalPoint(), unboundedPref, 5e-2)
ott.assert_almost_equal(
    notConstrainingBoundsLbfgs.getResult().getOptimalPoint(), unboundedPref, 5e-2)
ott.assert_almost_equal(
    constrainingBoundsLbfgs.getResult().getOptimalPoint(), boundedPref, 5e-2)
ott.assert_almost_equal(
    unboundedNewton.getResult().getOptimalPoint(), unboundedPref, 5e-2)
ott.assert_almost_equal(
    notConstrainingBoundsNewton.getResult().getOptimalPoint(), unboundedPref, 5e-2)
ott.assert_almost_equal(
    constrainingBoundsNewton.getResult().getOptimalPoint(), boundedPref, 5e-2)
ott.assert_almost_equal(
    unboundedTrustRegion.getResult().getOptimalPoint(), unboundedPref, 5e-2)
