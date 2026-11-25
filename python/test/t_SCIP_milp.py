#!/usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

CONTINUOUS = ot.OptimizationProblemImplementation.CONTINUOUS
INTEGER = ot.OptimizationProblemImplementation.INTEGER
BINARY = ot.OptimizationProblemImplementation.BINARY


def check_milp(name, problem, expectedPoint, expectedValue):
    """Solve a linear problem, where SCIP must prove optimality."""
    algo = otexp.SCIP(problem)
    algo.setStartingPoint([0.0] * problem.getDimension())
    algo.setMaximumCallsNumber(1000)
    algo.setMaximumIterationNumber(1000)
    algo.run()
    result = algo.getResult()
    print(name, result.getStatusMessage())
    assert result.getStatus() == ot.OptimizationResult.SUCCESS, name
    ott.assert_almost_equal(result.getOptimalPoint(), expectedPoint, 1e-6, 1e-6)
    ott.assert_almost_equal(result.getOptimalValue(), [expectedValue], 1e-6, 1e-6)


# Binary knapsack:
#   maximize 15*x0 + 12*x1 + 4*x2 + 2*x3
#   subject to 8*x0 + 5*x1 + 3*x2 + 2*x3 <= 10
# The optimum is [0, 1, 1, 1] with value 18. The linear formulation keeps the
# LP relaxation and SCIP proves optimality.
cost = [-15.0, -12.0, -4.0, -2.0]
bounds = ot.Interval([0.0] * 4, [1.0] * 4)
A = ot.Matrix([[8.0, 5.0, 3.0, 2.0]])
LU = ot.Interval([0.0], [10.0])
problem = otexp.LinearProgrammingProblem(cost, bounds, A, LU)
problem.setVariablesType([BINARY] * 4)
check_milp("knapsack", problem, [0.0, 1.0, 1.0, 1.0], -18.0)

# Continuous linear problem: the integrality constraints are absent, SCIP uses
# its LP solver on the same structured problem.
cost = [1.0, 1.0]
bounds = ot.Interval([0.0] * 2, [10.0] * 2)
A = ot.Matrix([[1.0, 1.0]])
LU = ot.Interval([1.0], [1.0e30])
problem = otexp.LinearProgrammingProblem(cost, bounds, A, LU)
check_milp("continuous", problem, [0.0, 1.0], 1.0)

# Mixed integer linear problem with a maximization and a one-sided inequality:
#   maximize 3*x0 + 2*x1
#   subject to x0 + x1 <= 4.5
#   x0 integer in [0, 3], x1 binary
# The optimum is [3, 1] with value 11.
cost = [-3.0, -2.0]
bounds = ot.Interval([0.0, 0.0], [3.0, 1.0])
A = ot.Matrix([[1.0, 1.0]])
LU = ot.Interval([-1.0e30], [4.5])
problem = otexp.LinearProgrammingProblem(cost, bounds, A, LU)
problem.setVariablesType([INTEGER, BINARY])
check_milp("one-sided", problem, [3.0, 1.0], -11.0)

# Infeasible linear problem: x >= 1 and x <= 0
cost = [1.0]
bounds = ot.Interval([1.0], [2.0])
A = ot.Matrix([[1.0]])
LU = ot.Interval([-1.0e30], [0.0])
problem = otexp.LinearProgrammingProblem(cost, bounds, A, LU)
algo = otexp.SCIP(problem)
algo.setCheckStatus(False)
algo.run()
result = algo.getResult()
print("infeasible", result.getStatusMessage())
assert result.getStatus() == ot.OptimizationResult.FAILURE, "infeasible status"
assert "infeasible" in result.getStatusMessage(), "infeasible message"

# A nonlinear objective cannot be bounded by SCIP without interval arithmetic,
# so optimality cannot be proven. The incumbent is checked instead.
#   minimize (x0 - 1)^2 + (x1 - 1)^2 + (x2 - 1)^2, x binary
# The optimum is [1, 1, 1] with value 0.
objective = ot.SymbolicFunction(
    ["x0", "x1", "x2"], ["(x0 - 1)^2 + (x1 - 1)^2 + (x2 - 1)^2"]
)
problem = ot.OptimizationProblem(objective)
problem.setBounds(ot.Interval([0.0] * 3, [1.0] * 3))
problem.setVariablesType([BINARY] * 3)
algo = otexp.SCIP(problem)
algo.setCheckStatus(False)
algo.setStartingPoint([0.0] * 3)
algo.setMaximumCallsNumber(100000)
algo.setMaximumIterationNumber(1000)
algo.run()
result = algo.getResult()
print("quadratic binary", result.getStatusMessage())
xStar = result.getOptimalPoint()
ott.assert_almost_equal(xStar, [1.0, 1.0, 1.0], 1e-6, 1e-6)
ott.assert_almost_equal(result.getOptimalValue(), [0.0], 1e-6, 1e-6)
assert result.getCallsNumber() > 1, "missing evaluations"

# Linear objective with a nonlinear constraint:
#   minimize x0 - x1
#   subject to x0^2 + x1^2 <= 1
# The optimum is [-1/sqrt(2), 1/sqrt(2)] with value -sqrt(2). The starting
# point is the optimum itself, which checks the warm start injection.
objective = ot.SymbolicFunction(["x0", "x1"], ["x0 - x1"])
inequality = ot.SymbolicFunction(["x0", "x1"], ["1 - x0^2 - x1^2"])
problem = ot.OptimizationProblem(objective)
problem.setBounds(ot.Interval([-2.0] * 2, [2.0] * 2))
problem.setInequalityConstraint(inequality)
xOptimal = ot.Point([-1.0 / 2.0**0.5, 1.0 / 2.0**0.5])
algo = otexp.SCIP(problem)
algo.setCheckStatus(False)
algo.setStartingPoint(xOptimal)
algo.setMaximumCallsNumber(100000)
algo.setMaximumIterationNumber(1000)
algo.run()
result = algo.getResult()
print("nonlinear inequality", result.getStatusMessage())
xStar = result.getOptimalPoint()
ott.assert_almost_equal(xStar, xOptimal, 1e-3, 1e-3)
assert inequality(xStar)[0] >= -1e-6, "unfeasible point"
ott.assert_almost_equal(result.getOptimalValue(), [-(2.0**0.5)], 1e-3, 1e-3)

# Nonlinear equality constraint:
#   minimize x0^2 + x1^2
#   subject to x0 + x1 = 1
# The optimum is [1/2, 1/2] with value 1/2.
objective = ot.SymbolicFunction(["x0", "x1"], ["x0^2 + x1^2"])
equality = ot.SymbolicFunction(["x0", "x1"], ["x0 + x1 - 1"])
problem = ot.OptimizationProblem(objective)
problem.setBounds(ot.Interval([-2.0] * 2, [2.0] * 2))
problem.setEqualityConstraint(equality)
algo = otexp.SCIP(problem)
algo.setCheckStatus(False)
algo.setStartingPoint([0.5, 0.5])
algo.setMaximumCallsNumber(20000)
algo.setMaximumIterationNumber(1000)
algo.setMaximumConstraintError(1e-8)
algo.run()
result = algo.getResult()
print("nonlinear equality", result.getStatusMessage())
xStar = result.getOptimalPoint()
ott.assert_almost_equal(xStar, [0.5, 0.5], 1e-3, 1e-3)
assert abs(equality(xStar)[0]) <= 1e-5, "unfeasible point"
ott.assert_almost_equal(result.getOptimalValue(), [0.5], 1e-3, 1e-3)

# The call budget is enforced cooperatively on the black-box evaluations
algo = otexp.SCIP(problem)
algo.setCheckStatus(False)
algo.setStartingPoint([0.0, 1.0])
algo.setMaximumCallsNumber(5)
algo.setMaximumIterationNumber(1000)
algo.run()
result = algo.getResult()
print("maximum calls", result.getStatusMessage())
assert result.getStatus() == ot.OptimizationResult.MAXIMUMCALLS, "call limit status"
assert "call limit" in result.getStatusMessage(), "call limit message"

# The node budget maps to the native SCIP node limit
algo = otexp.SCIP(problem)
algo.setCheckStatus(False)
algo.setStartingPoint([0.0, 1.0])
algo.setMaximumCallsNumber(1000000)
algo.setMaximumIterationNumber(1)
algo.setMaximumConstraintError(1e-8)
algo.run()
result = algo.getResult()
print("maximum nodes", result.getStatusMessage())
assert result.getStatus() == ot.OptimizationResult.MAXIMUMCALLS, "node limit status"
assert "node limit" in result.getStatusMessage(), "node limit message"
assert result.getIterationNumber() >= 1, "missing nodes"
