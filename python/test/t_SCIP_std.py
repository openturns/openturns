#!/usr/bin/env python

import math
import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

progress_values = []


def progress(percent):
    progress_values.append(percent)


def stop():
    return False


# Same MINLP as t_Bonmin_std.py:
#   min -x0 - x1 - x2
#   s.t.
#       -(x1 - 1/2)^2 - (x2 - 1/2)^2 + 1/4 >= 0
#       -x0 + x1 >= 0
#       -x0 - x2 - x3 + 2 >= 0
#       x0 in {0, 1}, x3 in [[0, 5]] integer, the other variables continuous
objectiveFunction = ot.SymbolicFunction(["x0", "x1", "x2", "x3"], ["-x0 -x1 -x2"])
# The first constraint bounds x1 and x2 to the unit disk centered on (1/2, 1/2):
# use the implied bounds, otherwise the black-box relaxation is unbounded and
# the solver has nothing to work with.
bounds = ot.Interval([0] * 4, [1, 1, 1, 5])
h = ot.SymbolicFunction(
    ["x0", "x1", "x2", "x3"],
    ["-(x1-1/2)^2 - (x2-1/2)^2 + 1/4", "-x0 + x1", "-x0 - x2 - x3 + 2"],
)
variablesType = [
    ot.OptimizationProblemImplementation.BINARY,
    ot.OptimizationProblemImplementation.CONTINUOUS,
    ot.OptimizationProblemImplementation.CONTINUOUS,
    ot.OptimizationProblemImplementation.INTEGER,
]
problem = ot.OptimizationProblem(objectiveFunction)
problem.setBounds(bounds)
problem.setVariablesType(variablesType)
problem.setInequalityConstraint(h)

# The optimal solution, used to check the returned point
xOptimal = ot.Point([1.0, 1.0, 0.5, 0.0])

# A black-box term cannot be bounded by SCIP, so optimality cannot be proven:
# inspect the result instead of raising when the node limit is reached.
algo = otexp.SCIP(problem)
algo.setCheckStatus(False)
algo.setStartingPoint(xOptimal)
algo.setMaximumCallsNumber(10000)
algo.setMaximumIterationNumber(1000)
algo.setMaximumConstraintError(1e-6)
algo.setProgressCallback(progress)
algo.setStopCallback(stop)
algo.run()
result = algo.getResult()
print(result)
assert result.getStatus() in [
    ot.OptimizationResult.SUCCESS,
    ot.OptimizationResult.MAXIMUMCALLS,
]
assert result.getStatus() != ot.OptimizationResult.TIMEOUT
assert result.getStatus() != ot.OptimizationResult.INTERRUPTION
xStar = result.getOptimalPoint()
print("x*=", xStar)
assert xStar.getDimension() == 4, "wrong dimension"
# The starting point is the optimum, SCIP must keep it
ott.assert_almost_equal(xStar, xOptimal, 1e-1, 1e-1)
yStar = result.getOptimalValue()[0]
print("f(x*)=", yStar)
assert yStar <= -2.5 + 1e-2, "wrong optimal value"
assert result.getCallsNumber() > 0, "missing evaluations"
assert len(progress_values) > 0, "missing progress updates"

# Rewards: objective, constraints and bounds are satisfied
ott.assert_almost_equal(objectiveFunction(xStar)[0], yStar, 1e-6, 1e-6)
assert (h(xStar)[0] >= -1e-5) and (h(xStar)[1] >= -1e-5) and (h(xStar)[2] >= -1e-5)

# Components of the linked SCIP library: an LP solver is always available, an
# NLP solver (Ipopt, Worhp, ...) is optional
lpSolver = otexp.SCIP.GetLPsolver()
nlpSolvers = otexp.SCIP.GetNLPsolvers()
print("lpSolver=", lpSolver, "nlpSolvers=", nlpSolvers)
assert lpSolver, "no LP solver in the SCIP library"


def outsideDisk(x):
    return [0.5 - ((x[1] - 1.0) ** 2 + (x[2] - 1.0) ** 2)]


# Mixed-integer problem with black-box constraint: the objective is linear and
# the only nonlinear part is the PythonFunction constraint
objectiveFun = ot.SymbolicFunction(["x0", "x1", "x2"], ["-x0 -x1 -x2"])
problem = ot.OptimizationProblem(objectiveFun)
problem.setBounds(ot.Interval([0.0] * 3, [3.0] * 3))
problem.setVariablesType(
    [
        ot.OptimizationProblemImplementation.INTEGER,
        ot.OptimizationProblemImplementation.CONTINUOUS,
        ot.OptimizationProblemImplementation.CONTINUOUS,
    ]
)
problem.setInequalityConstraint(ot.PythonFunction(3, 1, outsideDisk))
algo = otexp.SCIP(problem)
# The black-box constraint cannot be relaxed: start from the optimal point so
# that SCIP has a feasible incumbent, and accept the node limit.
algo.setStartingPoint([3.0, 1.5, 1.5])
algo.setMaximumIterationNumber(1000)
algo.setCheckStatus(False)
algo.run()
result = algo.getResult()
print(result)
assert result.getStatus() in [
    ot.OptimizationResult.SUCCESS,
    ot.OptimizationResult.MAXIMUMCALLS,
]
ott.assert_almost_equal(result.getOptimalPoint(), [3, 1.5, 1.5], 3, 1e-4)
ott.assert_almost_equal(result.getOptimalValue(), [-6.0], 7, 1e-4)

# Mixed-integer problem with a multimodal black-box objective: the global
# minimum cannot be reached, only the consistency of the solution is checked


def swiler2014(x):
    x1, x2, x3 = x
    a = math.sin(2 * math.pi * x3 - math.pi)
    b = 7 * math.sin(2 * math.pi * x2 - math.pi) ** 2
    fac = [0, 12.0, 0.5, 8.0, 3.5][int(x1)]
    return [a + b + fac * a]


objectiveFun = ot.PythonFunction(3, 1, swiler2014)
bounds = ot.Interval([0.0] * 3, [1.0, 1.0, 4.0])
variablesType = [
    ot.OptimizationProblemImplementation.INTEGER,
    ot.OptimizationProblemImplementation.CONTINUOUS,
    ot.OptimizationProblemImplementation.CONTINUOUS,
]
problem = ot.OptimizationProblem(objectiveFun)
problem.setBounds(bounds)
problem.setVariablesType(variablesType)
algo = otexp.SCIP(problem)
startingPoint = [0.0] * 3
algo.setStartingPoint(startingPoint)
algo.setMaximumIterationNumber(1000)
algo.setCheckStatus(False)
algo.run()
result = algo.getResult()
print(result)
assert result.getStatus() in [
    ot.OptimizationResult.SUCCESS,
    ot.OptimizationResult.MAXIMUMCALLS,
]
point = result.getOptimalPoint()
for i in range(3):
    assert bounds.getLowerBound()[i] <= point[i] <= bounds.getUpperBound()[i]
    if variablesType[i] != ot.OptimizationProblemImplementation.CONTINUOUS:
        ott.assert_almost_equal(point[i], round(point[i]), 7, 1e-6)
ott.assert_almost_equal(result.getOptimalValue(), objectiveFun(point), 7, 1e-6)
assert result.getOptimalValue()[0] <= objectiveFun(startingPoint)[0] + 1e-8

# The NLP solver used for the nonlinear parts can be selected, or the NLP
# relaxation can be disabled
for options in [{}, {"SCIP-nlp/disable": True}] + [
    {"SCIP-nlp/solver": name} for name in nlpSolvers
]:
    print("options=", options)
    objectiveFun = ot.SymbolicFunction(["x0", "x1", "x2"], ["-x0 -x1 -x2"])
    problem = ot.OptimizationProblem(objectiveFun)
    problem.setBounds(ot.Interval([0.0] * 3, [3.0] * 3))
    problem.setVariablesType(
        [
            ot.OptimizationProblemImplementation.INTEGER,
            ot.OptimizationProblemImplementation.CONTINUOUS,
            ot.OptimizationProblemImplementation.CONTINUOUS,
        ]
    )
    problem.setInequalityConstraint(ot.PythonFunction(3, 1, outsideDisk))
    algo = otexp.SCIP(problem)
    algo.setStartingPoint([1.0, 1.0, 1.0])
    algo.setMaximumIterationNumber(1000)
    algo.setCheckStatus(False)
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
    point = result.getOptimalPoint()
    assert outsideDisk(point)[0] >= -1e-3
    assert result.getOptimalValue()[0] >= -6.0 - 1e-4

# repr/str and ResourceMap forwarding
print(algo)
ot.ResourceMap.AddAsScalar("SCIP-limits/gap", 1e-6)
ot.ResourceMap.AddAsUnsignedInteger("SCIP-limits/solutions", 100)
ot.ResourceMap.AddAsBool("SCIP-misc/catchctrlc", False)
print(algo)
assert "limits/gap" in repr(algo)
for key in ["SCIP-limits/gap", "SCIP-limits/solutions", "SCIP-misc/catchctrlc"]:
    ot.ResourceMap.RemoveKey(key)

# Invalid SCIP parameter
ot.ResourceMap.AddAsScalar("SCIP-no/such/parameter", 1.0)
try:
    with ott.assert_raises(Exception):
        algo.run()
finally:
    ot.ResourceMap.RemoveKey("SCIP-no/such/parameter")

# checkProblem branches: multi-objective and least squares problems are rejected
multi = ot.OptimizationProblem(ot.SymbolicFunction(["x"], ["x", "2*x"]))
with ott.assert_raises(Exception):
    otexp.SCIP(multi)
lsProblem = ot.LeastSquaresProblem(ot.SymbolicFunction(["x"], ["x^2"]))
with ott.assert_raises(Exception):
    otexp.SCIP(lsProblem)

# Invalid starting point dimension
bad = otexp.SCIP(problem)
with ott.assert_raises(Exception):
    bad.setStartingPoint([0.0])
    bad.run()

# Default algorithm without a problem
empty = otexp.SCIP()
with ott.assert_raises(Exception):
    empty.run()

# Save/load round-trip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("scip.xml"))
study.add("algo", algo)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("scip.xml"))
loadedStudy.load()
assert loadedStudy.hasObject("algo")
os.remove("scip.xml")
