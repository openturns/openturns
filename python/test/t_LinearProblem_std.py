#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

objective = ot.SymbolicFunction(["x0", "x1"], ["1.1*x0 + x1"])
ineq = ot.SymbolicFunction(["x0", "x1"], ["x0 + 2*x1 - 5", "3*x0 + 2*x1 - 6"])
eq = ot.SymbolicFunction(["x0", "x1"], ["7*x0 + 9*x1 - 50"])
problem = ot.OptimizationProblem(objective)
problem.setInequalityConstraint(ineq)
problem.setEqualityConstraint(eq)
bounds = ot.Interval([0.0, 1.0], [4.0, 1e30])
problem.setBounds(bounds)
problem.setVariablesType(
    [
        ot.OptimizationProblemImplementation.CONTINUOUS,
        ot.OptimizationProblemImplementation.INTEGER,
    ]
)
location = [0.0] * 2
linearProblem = otexp.LinearProblem.Linearize(problem, location)
print(linearProblem)
cost = linearProblem.getLinearCost()
A = linearProblem.getLinearConstraintCoefficients()
LU = linearProblem.getLinearConstraintBounds()
assert cost == [1.1, 1.0]
ott.assert_almost_equal(A, ot.Matrix([[1, 2], [3, 2], [7, 9]]))
assert LU.getLowerBound() == [5.0, 6.0, 50.0]
assert LU.getUpperBound()[2] == 50.0
assert problem.getVariablesType()[1] == ot.OptimizationProblemImplementation.INTEGER
assert problem.getBounds().getLowerBound() == [0.0, 1.0]

# constant term, as in QuadraticProblem: the optimal point is unchanged,
# the optimal value is shifted
assert linearProblem.getConstant() == 0.0
linearProblem.setConstant(1.0)
assert linearProblem.getConstant() == 1.0
# the objective includes the constant term
ott.assert_almost_equal(linearProblem.getObjective()([1.0, 2.0]), [1.1 + 2.0 + 1.0])
# the constant term can also be given to the constructor
problemConstant = otexp.LinearProblem(
    [1.1, 1.0], ot.Interval([0.0, 1.0], [4.0, 1e30]), A, LU, 2.0
)
assert problemConstant.getConstant() == 2.0
ott.assert_almost_equal(problemConstant.getObjective()([1.0, 2.0]), [1.1 + 2.0 + 2.0])
# setting a new cost preserves the constant term
linearProblem.setLinearCost([1.0, 1.0])
assert linearProblem.getConstant() == 1.0
ott.assert_almost_equal(linearProblem.getObjective()([1.0, 2.0]), [1.0 + 2.0 + 1.0])

# Linearize captures the offset on nonlinear objectives: at the location
# the linearized objective matches the original function
nonlinearObjective = ot.SymbolicFunction(["x0", "x1"], ["x0^2 + x1"])
nonlinearProblem = ot.OptimizationProblem(nonlinearObjective)
nonlinearProblem.setInequalityConstraint(
    ot.SymbolicFunction(["x0", "x1"], ["x0 + x1 - 1"])
)
nonlinearProblem.setBounds(ot.Interval([0.0, 0.0], [4.0, 4.0]))
nonlinearLocation = [1.0, 2.0]
nonlinearLinear = otexp.LinearProblem.Linearize(nonlinearProblem, nonlinearLocation)
assert nonlinearLinear.getLinearCost() == [2.0, 1.0]
# f(x0) - g(x0).x0 = 3 - 4
ott.assert_almost_equal(nonlinearLinear.getConstant(), -1.0)
ott.assert_almost_equal(
    nonlinearLinear.getObjective()(nonlinearLocation),
    nonlinearObjective(nonlinearLocation),
)
