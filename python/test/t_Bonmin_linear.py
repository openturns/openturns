#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Binary knapsack:
#   maximize 15*x0 + 12*x1 + 4*x2 + 2*x3
#   subject to 8*x0 + 5*x1 + 3*x2 + 2*x3 <= 10
#   x0, x1, x2, x3 in {0, 1}
# The optimum is [0, 1, 1, 1] with objective value 18.
# It is described with a structured LinearProgrammingProblem, whose constraints only
# live in the matrix form L <= A.x <= U.
cost = [-15.0, -12.0, -4.0, -2.0]
bounds = ot.Interval([0.0] * 4, [1.0] * 4)
A = ot.Matrix([[8.0, 5.0, 3.0, 2.0]])
LU = ot.Interval([0.0], [10.0])
problem = otexp.LinearProgrammingProblem(cost, bounds, A, LU)
problem.setVariablesType([ot.OptimizationProblemImplementation.BINARY] * 4)

algo = ot.Bonmin(problem)
algo.setStartingPoint([0.0] * 4)
algo.setMaximumCallsNumber(1000)
algo.setMaximumIterationNumber(1000)
algo.run()
result = algo.getResult()
print(result)
assert result.getStatus() == ot.OptimizationResult.SUCCESS
assert result.getOptimalPoint() == [0.0, 1.0, 1.0, 1.0]
ott.assert_almost_equal(result.getOptimalValue(), [-18.0])
