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
