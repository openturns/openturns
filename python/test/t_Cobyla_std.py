#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

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

# nearest-point problem converging exactly onto the relaxed boundary:
# the solver enforces |h| <= maximumConstraintError while the replay judges
# the raw residual strictly, which must not discard a successful run
for levelValue, expected in [(0.0, 1.0), (0.5, 1.5)]:
    f = ot.SymbolicFunction(["u"], ["u - 1.0"])
    solver = ot.Cobyla(ot.NearestPointProblem(f, levelValue))
    solver.setMaximumConstraintError(1e-8)
    solver.setMaximumAbsoluteError(1e-8)
    solver.setStartingPoint([0.0])
    solver.run()
    result = solver.getResult()
    ott.assert_almost_equal(result.getOptimalPoint(), [expected], 1e-4, 0.0)
