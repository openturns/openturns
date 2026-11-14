#!/usr/bin/env python

import openturns as ot


def printResults(result, problemName):
    print(f"*** {problemName} completed:")
    print(f"      -- Optimal point = {result.getOptimalPoint()}")
    print(f"      -- Optimal value = {result.getOptimalValue()}")
    print(f"      -- Iteration number = {result.getIterationNumber()}")
    print(f"      -- Evaluation number = {result.getCallsNumber()}")
    print(f"      -- Absolute error = {result.getAbsoluteError():.6e}")
    print(f"      -- Relative error = {result.getRelativeError():.6e}")
    print(f"      -- Residual error = {result.getResidualError():.6e}")
    print(f"      -- Constraint error = {result.getConstraintError():.6e}")


# Define the problems based on Rastrigin function
rastrigin = ot.SymbolicFunction(
    ["x1", "x2"], ["20 + x1^2 - 10*cos(2*pi_*x1) + x2^2 - 10*cos(2*pi_*x2)"]
)

unboundedProblem = ot.OptimizationProblem(rastrigin)

notConstrainingBounds = ot.Interval([-5.0, -5.0], [3.0, 2.0])
notConstrainingBoundsProblem = ot.OptimizationProblem(
    rastrigin, ot.Function(), ot.Function(), notConstrainingBounds
)

constrainingBounds = ot.Interval([-1.0, -2.0], [5.0, -0.5])
constrainingBoundsProblem = ot.OptimizationProblem(
    rastrigin, ot.Function(), ot.Function(), constrainingBounds
)

boundedPref = [0.0, -1.0]
unboundedPref = [0.0, 0.0]

# GLOBAL ALGORITHM

# Non-contraining bounds Global
notConstrainingBoundsGlobal = ot.Dlib(notConstrainingBoundsProblem, "global")
notConstrainingBoundsGlobal.setStartingPoint([0.0] * 2)
notConstrainingBoundsGlobal.setMaximumCallsNumber(300)
notConstrainingBoundsGlobal.run()
printResults(notConstrainingBoundsGlobal.getResult(), "Non-constraining bounds Global")

# Contraining bounds Global
constrainingBoundsGlobal = ot.Dlib(constrainingBoundsProblem, "global")
constrainingBoundsGlobal.setMaximumCallsNumber(300)
constrainingBoundsGlobal.setStartingPoint([0.0] * 2)
constrainingBoundsGlobal.run()
printResults(constrainingBoundsGlobal.getResult(), "Constraining bounds Global")

assert (
    notConstrainingBoundsGlobal.getResult().getOptimalValue()[0] < 4.0
), "optimum not found"

# progress/stop callbacks and user stop
max_calls = 20
global_progress = []


def progress(p):
    global_progress.append(p)


def no_stop():
    return False


def stop_now():
    return True


global_algo = ot.Dlib(notConstrainingBoundsProblem, "global")
global_algo.setStartingPoint([0.0] * 2)
global_algo.setMaximumCallsNumber(max_calls)
global_algo.setProgressCallback(progress)
global_algo.setStopCallback(no_stop)
global_algo.run()
assert len(global_progress) == max_calls, "wrong progress calls number"
assert all(0.0 <= p <= 100.0 for p in global_progress), "progress out of range"
assert all(b >= a for a, b in zip(global_progress, global_progress[1:])), "progress not monotonic"
assert abs(global_progress[-1] - 100.0) < 1e-12, "wrong final progress"

stop_global = ot.Dlib(notConstrainingBoundsProblem, "global")
stop_global.setStartingPoint([0.0] * 2)
stop_global.setMaximumCallsNumber(max_calls)
stop_global.setStopCallback(stop_now)
stop_global.run()
assert stop_global.getResult().getStatus() == ot.OptimizationResult.INTERRUPTION
assert stop_global.getResult().getCallsNumber() == 0, "wrong calls number"
