#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(4)

# linear
levelFunction = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["x1+2*x2-3*x3+4*x4"])
algo = ot.Cobyla(ot.NearestPointProblem(levelFunction, 3.0))
algo.setStartingPoint([0.0] * 4)
print("algo=", algo)
algo.run()
result = algo.getResult()
print("x^=", result.getOptimalPoint())
print("f(x^)=", result.getOptimalValue())
print("lambda^=", result.computeLagrangeMultipliers())

# non-linear
levelFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"]
)
algo = ot.Cobyla(ot.NearestPointProblem(levelFunction, 3.0))
algo.setStartingPoint([0.0] * 4)
algo.setMaximumCallsNumber(400)
algo.setMaximumAbsoluteError(1.0e-10)
algo.setMaximumRelativeError(1.0e-10)
algo.setMaximumResidualError(1.0e-10)
algo.setMaximumConstraintError(1.0e-10)
algo.run()
result = algo.getResult()
print("x^=", result.getOptimalPoint())
print("f(x^)=", result.getOptimalValue())
print("lambda^=", result.computeLagrangeMultipliers())

# bounds
linear = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["x1+2*x2-3*x3+4*x4"])

dim = 4

bounds = ot.Interval([-3.0] * dim, [5.0] * dim)

for minimization in [True, False]:
    problem = ot.OptimizationProblem(linear, ot.Function(), ot.Function(), bounds)
    problem.setMinimization(minimization)
    algo = ot.Cobyla(problem)
    algo.setMaximumCallsNumber(150)
    algo.setStartingPoint([0.0] * dim)
    print("algo=", algo)
    algo.run()
    result = algo.getResult()
    print("x^=", result.getOptimalPoint())
    print("f(x^)=", result.getOptimalValue())
    print("lambda^=", result.computeLagrangeMultipliers())

# empty problem
algo = ot.Cobyla()
try:
    algo.run()
except Exception:
    print("OK")
