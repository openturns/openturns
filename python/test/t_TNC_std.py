#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# linear
levelFunction = ot.SymbolicFunction(["x1", "x2", "x3", "x4"], ["x1+2*x2-3*x3+4*x4"])
bounds = ot.Interval([-3.0] * 4, [5.0] * 4)
algo = ot.TNC()
algo.setStartingPoint([0.0] * 4)

problem = ot.OptimizationProblem(levelFunction)
problem.setBounds(bounds)
problem.setMinimization(True)

algo.setProblem(problem)
print("algo=", algo)
algo.run()
result = algo.getResult()
print("result=", result.getOptimalPoint())
print("multipliers=", result.computeLagrangeMultipliers())

problem.setMinimization(False)
algo.setProblem(problem)
print("algo=", algo)
algo.run()
result = algo.getResult()
print("result=", result.getOptimalPoint())
print("multipliers=", result.computeLagrangeMultipliers())

# non-linear
levelFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"]
)

bounds = ot.Interval(ot.Point(4, -3.0), ot.Point(4, 5.0))

algo = ot.TNC()
problem = ot.OptimizationProblem(levelFunction)
problem.setBounds(bounds)

problem.setMinimization(True)
algo.setProblem(problem)
algo.setStartingPoint([3.0, -2.5, 4.5, -2.5])
print("algo=", algo)
algo.run()
result = algo.getResult()
print("result=", result.getOptimalPoint())
print("multipliers=", result.computeLagrangeMultipliers())

problem.setMinimization(False)
algo.setProblem(problem)
print("algo=", algo)
algo.setStartingPoint([-2.5, 4.5, 4.5, 4.5])
algo.run()
result = algo.getResult()
print("result=", result.getOptimalPoint())
print("multipliers=", result.computeLagrangeMultipliers())

# user stop is reported as INTERRUPTION without throwing, even when it fires
# before any feasible point is retained
stopProblem = ot.OptimizationProblem(ot.SymbolicFunction(["x"], ["(x-1.0)^2"]))
stopProblem.setBounds(ot.Interval([0.0], [2.0]))


def stopNow():
    return True


stopSolver = ot.TNC(stopProblem)
stopSolver.setStartingPoint([0.5])
# enabled checkStatus also guards the final INTERRUPTION exclusion: an
# unwanted throw would fail this test instead of only warning
stopSolver.setCheckStatus(True)
stopSolver.setStopCallback(stopNow)
stopSolver.run()
assert stopSolver.getResult().getStatus() == ot.OptimizationResult.INTERRUPTION
firstStop = ot.TNC(stopProblem)
firstStop.setStartingPoint([5.0])
firstStop.setCheckStatus(False)
firstStop.setStopCallback(stopNow)
firstStop.run()
assert firstStop.getResult().getStatus() == ot.OptimizationResult.INTERRUPTION
