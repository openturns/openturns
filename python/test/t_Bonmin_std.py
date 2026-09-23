#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
import sys


def progress(percent):
    sys.stderr.write("-- progress=" + str(percent) + "%\n")


def stop():
    sys.stderr.write("-- stop?\n")
    return False


# List available algorithms
print(ot.Bonmin.GetAlgorithmNames())

# Definition of objective function
objectiveFunction = ot.SymbolicFunction(["x0", "x1", "x2", "x3"], ["-x0 -x1 -x2"])

# Definition of variables bounds
bounds = ot.Interval([0] * 4, [1, 1e6, 1e6, 5])

# Definition of constraints
# Constraints in OpenTURNS are defined as g(x) = 0 and h(x) >= 0
#    No equality constraint -> nothing to do
#    Inequality constraints:
h = ot.SymbolicFunction(
    ["x0", "x1", "x2", "x3"],
    ["-(x1-1/2)^2 - (x2-1/2)^2 + 1/4", "-x0 + x1", "-x0 - x2 - x3 + 2"],
)

# Definition of variables types
variablesType = [
    ot.OptimizationProblemImplementation.BINARY,
    ot.OptimizationProblemImplementation.CONTINUOUS,
    ot.OptimizationProblemImplementation.CONTINUOUS,
    ot.OptimizationProblemImplementation.INTEGER,
]

# Setting up Bonmin problem
problem = ot.OptimizationProblem(objectiveFunction)
problem.setBounds(bounds)
problem.setVariablesType(variablesType)
problem.setInequalityConstraint(h)

algo = ot.Bonmin(problem)
algo.setStartingPoint([0.0] * 4)
algo.setMaximumCallsNumber(10000)
algo.setMaximumIterationNumber(1000)
# algo.setProgressCallback(progress)
# algo.setStopCallback(stop)

for name in ot.Bonmin.GetAlgorithmNames():
    print(f"-- {name} algorithm...")
    algo.setAlgorithmName(name)
    algo.run()
    result = algo.getResult()
    x_star = result.getOptimalPoint()
    print(f"x*={x_star}")
    y_star = result.getOptimalValue()
    neval = result.getCallsNumber()
    print(f"f(x*)={y_star} neval={neval}")
    ott.assert_almost_equal(result.getOptimalPoint(), [1, 1, 0.5, 0], 1, 1e-2)

# extra coverage: accessors, repr/str, clone, error branches
assert algo.getAlgorithmName() in list(ot.Bonmin.GetAlgorithmNames())
_ = repr(algo)
_ = str(algo)
algo2 = ot.Bonmin("B-BB")
assert algo2.getAlgorithmName() == "B-BB"
with ott.assert_raises(Exception):
    algo2.setAlgorithmName("unknown-solver")
with ott.assert_raises(Exception):
    ot.Bonmin("unknown-solver")
# checkProblem branches: ctor with multi-objective problem must raise
multi = ot.OptimizationProblem(ot.SymbolicFunction(["x"], ["x", "2*x"]))
with ott.assert_raises(Exception):
    ot.Bonmin(multi)
lsProblem = ot.LeastSquaresProblem(ot.SymbolicFunction(["x"], ["x^2"]))
with ott.assert_raises(Exception):
    ot.Bonmin(lsProblem)
# invalid starting point dimension triggers error
bad = ot.Bonmin(problem)
with ott.assert_raises(Exception):
    bad.setStartingPoint([0.0])
    bad.run()
