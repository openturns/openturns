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
            algo.setMaximumEvaluationNumber(100000)
            algo.run()
            result = algo.getResult()
            x = result.getOptimalPoint()
            y = result.getOptimalValue()
            print(f"bounds={problem.hasBounds()} algo={algoName} x^={x} y^={y}")
            if bounds == constrainingBounds:
                ott.assert_almost_equal(x, boundedPref, 5e-2)
            else:
                ott.assert_almost_equal(x, unboundedPref, 5e-2)
