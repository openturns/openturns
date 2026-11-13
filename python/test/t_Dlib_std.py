#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott

# Define the problems based on Rosebrock function
rosenbrock = ot.SymbolicFunction(["x1", "x2"], ["(1-x1)^2+100*(x2-x1^2)^2"])

unboundedProblem = ot.OptimizationProblem(rosenbrock)

notConstrainingBounds = ot.Interval([-5.0, -5.0], [5.0, 5.0])
constrainingBounds = ot.Interval([0.0, -2.0], [5.0, 0.5])

start = [3.0, -1.5]
unboundedPref = [1.0, 1.0]
boundedPref = [0.70856, 0.5]

for bound in [False, True]:
    for bounds in [ot.Interval(), notConstrainingBounds, constrainingBounds]:
        problem = ot.OptimizationProblem(rosenbrock)
        problem.setBounds(bounds)
        for algoName in ["cg", "bfgs", "lbfgs", "newton", "trust_region"]:
            if algoName == "trust_region" and problem.hasBounds():
                continue
            algo = ot.Dlib(problem, algoName)
            algo.setStartingPoint(start)
            algo.setMaximumIterationNumber(10000)
            algo.setMaximumCallsNumber(100000)
            algo.run()
            result = algo.getResult()
            x = result.getOptimalPoint()
            y = result.getOptimalValue()
            print(f"bounds={problem.hasBounds()} algo={algoName} x^={x} y^={y}")
            if bounds == constrainingBounds:
                ott.assert_almost_equal(x, boundedPref, 5e-2)
            else:
                ott.assert_almost_equal(x, unboundedPref, 5e-2)

# progress/stop callbacks and user stop
progress_calls = []


def progress(p):
    progress_calls.append(p)


def no_stop():
    return False


def stop_now():
    return True


algo = ot.Dlib(unboundedProblem, "bfgs")
algo.setStartingPoint(start)
algo.setMaximumIterationNumber(10000)
algo.setMaximumCallsNumber(100000)
algo.setProgressCallback(progress)
algo.setStopCallback(no_stop)
algo.run()
ott.assert_almost_equal(algo.getResult().getOptimalPoint(), unboundedPref, 5e-2)
assert len(progress_calls) > 0, "progress callback was not invoked"
assert all(0.0 <= p <= 100.0 for p in progress_calls), "progress out of range"
assert all(b >= a for a, b in zip(progress_calls, progress_calls[1:])), "progress not monotonic"

stop_algo = ot.Dlib(unboundedProblem, "bfgs")
stop_algo.setStartingPoint(start)
stop_algo.setMaximumCallsNumber(100000)
stop_algo.setStopCallback(stop_now)
stop_algo.run()
assert stop_algo.getResult().getStatus() == ot.OptimizationResult.INTERRUPTION
