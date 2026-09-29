#! /usr/bin/env python

import math
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

Continuous = ot.OptimizationProblemImplementation.CONTINUOUS
Integer = ot.OptimizationProblemImplementation.INTEGER
Binary = ot.OptimizationProblemImplementation.BINARY

# Components of the linked SCIP library: an LP solver is always available, an NLP
# solver (Ipopt, Worhp, ...) is optional and drives the quality of the nonlinear search
lpSolver = otexp.SCIP.GetLPsolver()
nlpSolvers = otexp.SCIP.GetNLPsolvers()
print("lpSolver=", lpSolver, "nlpSolvers=", nlpSolvers)
assert lpSolver, "no LP solver in the SCIP library"

# Linear problem (same as HiGHS std)
bounds = ot.Interval([0.0, 1.0], [4.0, ot.SpecFunc.MaxScalar])
cost = [1.1, 1.0]
A = ot.Matrix([[0.0, 1.0], [1.0, 2.0], [3.0, 2.0]])
cb = ot.Interval([-1e9, 5.0, 6.0], [7.0, 15.0, 1e9])
problem = otexp.LinearProblem(cost, bounds, A, cb)
print(problem)
sol = {
    Continuous: [0.5, 2.25],
    Integer: [0.0, 3.0],
}
for vtype in [Continuous, Integer]:
    problem.setVariablesType([vtype] * 2)
    algo = otexp.SCIP(problem)
    print(algo)
    algo.run()
    result = algo.getResult()
    print(result)
    ott.assert_almost_equal(result.getOptimalPoint(), sol[vtype])

# Mixed-integer nonlinear problem (same as Bonmin std). The first constraint bounds
# x1 and x2 to the unit disk centered on (1/2, 1/2), so the huge upper bounds used in
# Bonmin are replaced by the implied ones, otherwise the black-box relaxation of the
# problem is unbounded and the solver has nothing to work with
objectiveFunction = ot.SymbolicFunction(
    ["x0", "x1", "x2", "x3"], ["-x0 -x1 -x2"]
)
bounds = ot.Interval([0] * 4, [1, 1, 1, 5])
h = ot.SymbolicFunction(
    ["x0", "x1", "x2", "x3"],
    ["-(x1-1/2)^2 - (x2-1/2)^2 + 1/4", "-x0 + x1", "-x0 - x2 - x3 + 2"],
)
variablesType = [Binary, Continuous, Continuous, Integer]
problem = ot.OptimizationProblem(objectiveFunction)
problem.setBounds(bounds)
problem.setVariablesType(variablesType)
problem.setInequalityConstraint(h)
algo = otexp.SCIP(problem)
algo.setStartingPoint([0.0] * 4)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getOptimalPoint(), [1, 1, 0.5, 0], 1, 1e-2)

# Binary linear problem (same as Bonmin MIT15)
objectiveFun = ot.SymbolicFunction(
    ["x", "y", "z", "t"], ["-(15*x + 12*y + 4*z + 2*t)"]
)
constraintFun = ot.SymbolicFunction(
    ["x", "y", "z", "t"], ["-(8*x + 5*y + 3*z + 2*t -10)"]
)
problem = ot.OptimizationProblem(objectiveFun)
problem.setInequalityConstraint(constraintFun)
problem.setBounds(ot.Interval([0.0] * 4, [1.0] * 4))
problem.setVariablesType([Binary] * 4)
algo = otexp.SCIP(problem)
algo.setStartingPoint([0.0] * 4)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getOptimalPoint(), [0, 1, 1, 1], 1, 5e-4)

# Mixed-integer problem with black-box constraint: the objective is linear and the only
# nonlinear part is the PythonFunction constraint, so the problem is solved to optimality


def outsideDisk(x):
    return [0.5 - ((x[1] - 1.0) ** 2 + (x[2] - 1.0) ** 2)]


objectiveFun = ot.SymbolicFunction(["x0", "x1", "x2"], ["-x0 -x1 -x2"])
constraintFun = ot.PythonFunction(3, 1, outsideDisk)
problem = ot.OptimizationProblem(objectiveFun)
problem.setBounds(ot.Interval([0.0] * 3, [3.0] * 3))
problem.setVariablesType([Integer, Continuous, Continuous])
problem.setInequalityConstraint(constraintFun)
algo = otexp.SCIP(problem)
algo.setStartingPoint([0.0] * 3)
algo.run()
result = algo.getResult()
print(result)
ott.assert_almost_equal(result.getOptimalPoint(), [3, 1.5, 1.5], 3, 1e-4)
ott.assert_almost_equal(result.getOptimalValue(), [-6.0], 7, 1e-4)

# Mixed-integer problem with black-box objective (same as Bonmin swiler2014). Its
# objective is multimodal, thus the global minimum cannot be reached whatever the build,
# and only the consistency of the returned solution is checked


def swiler2014(x):
    x1, x2, x3 = x
    a = math.sin(2 * math.pi * x3 - math.pi)
    b = 7 * math.sin(2 * math.pi * x2 - math.pi) ** 2
    fac = [0, 12.0, 0.5, 8.0, 3.5][int(x1)]
    return [a + b + fac * a]


objectiveFun = ot.PythonFunction(3, 1, swiler2014)
bounds = ot.Interval([0.0] * 3, [1.0, 1.0, 4.0])
variablesType = [Integer, Continuous, Continuous]
problem = ot.OptimizationProblem(objectiveFun)
problem.setBounds(bounds)
problem.setVariablesType(variablesType)
algo = otexp.SCIP(problem)
startingPoint = [0.0] * 3
algo.setStartingPoint(startingPoint)
algo.setMaximumIterationNumber(1000)
algo.setCheckStatus(False)  # budget-limited run may stop on node limit
algo.run()
result = algo.getResult()
print(result)
assert result.getStatus() in [ot.OptimizationResult.SUCCESS, ot.OptimizationResult.MAXIMUMCALLS]
point = result.getOptimalPoint()
# The solution lies in the domain and matches the variables types
for i in range(3):
    assert bounds.getLowerBound()[i] <= point[i] <= bounds.getUpperBound()[i]
    if variablesType[i] != Continuous:
        ott.assert_almost_equal(point[i], round(point[i]), 7, 1e-6)
# The reported value is the objective evaluated at the reported point
ott.assert_almost_equal(result.getOptimalValue(), objectiveFun(point), 7, 1e-6)
# The search improved upon the starting point
assert result.getOptimalValue()[0] <= objectiveFun(startingPoint)[0] + 1e-8

# The NLP solver used for the nonlinear parts can be selected, or the NLP relaxation
# can be disabled, whatever the components of the linked SCIP library


def outsideDisk(x):
    return [0.5 - ((x[1] - 1.0) ** 2 + (x[2] - 1.0) ** 2)]


for options in [{}, {"SCIP-nlp/disable": True}] + [
    {"SCIP-nlp/solver": name} for name in nlpSolvers
]:
    print("options=", options)
    objectiveFun = ot.SymbolicFunction(["x0", "x1", "x2"], ["-x0 -x1 -x2"])
    constraintFun = ot.PythonFunction(3, 1, outsideDisk)
    problem = ot.OptimizationProblem(objectiveFun)
    problem.setBounds(ot.Interval([0.0] * 3, [3.0] * 3))
    problem.setVariablesType([Integer, Continuous, Continuous])
    problem.setInequalityConstraint(constraintFun)
    algo = otexp.SCIP(problem)
    algo.setStartingPoint([0.0] * 3)
    for key, value in options.items():
        if isinstance(value, bool):
            ot.ResourceMap.AddAsBool(key, value)
        else:
            ot.ResourceMap.AddAsString(key, value)
    algo.run()
    for key in options:
        ot.ResourceMap.RemoveKey(key)
    result = algo.getResult()
    print(result)
    # Whatever the NLP solver, the solution remains feasible up to the solver tolerance
    point = result.getOptimalPoint()
    assert outsideDisk(point)[0] >= -1e-3
    assert result.getOptimalValue()[0] >= -6.0 - 1e-4
