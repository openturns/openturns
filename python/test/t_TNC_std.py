#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m


def printNumericalPoint(point, digits):
    oss = "["
    eps = pow(0.1, digits)
    format = "%." + str(digits) + "f"
    for i in range(point.getDimension()):
        if i == 0:
            sep = ""
        else:
            sep = ","
        if m.fabs(point[i]) < eps:
            oss += sep + format % m.fabs(point[i])
        else:
            oss += sep + format % point[i]
        sep = ","
    oss += "]"
    return oss

# linear
levelFunction = ot.Function(
    ["x1", "x2", "x3", "x4"], ["y1"], ["x1+2*x2-3*x3+4*x4"])
startingPoint = ot.NumericalPoint(4, 0.0)
bounds = ot.Interval(ot.NumericalPoint(4, -3.0), ot.NumericalPoint(4, 5.0))
algo = ot.TNC()
algo.setStartingPoint(startingPoint)

problem = ot.OptimizationProblem(levelFunction)
problem.setBounds(bounds)
problem.setMinimization(True)

algo.setProblem(problem)
print('algo=', algo)
algo.run()
result = algo.getResult()
print('result=', printNumericalPoint(result.getOptimalPoint(), 4))
print('multipliers=', printNumericalPoint(result.getLagrangeMultipliers(), 4))

problem.setMinimization(False)
algo.setProblem(problem)
print('algo=', algo)
algo.run()
result = algo.getResult()
print('result=', printNumericalPoint(result.getOptimalPoint(), 4))
print('multipliers=', printNumericalPoint(result.getLagrangeMultipliers(), 4))

# non-linear
levelFunction = ot.Function(
    ["x1", "x2", "x3", "x4"], ["y1"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
startingPoint = ot.NumericalPoint(4, -0.01)

startingPointNearMinimizationCorner = ot.NumericalPoint(4)
startingPointNearMinimizationCorner[0] = 3.0
startingPointNearMinimizationCorner[1] = -2.5
startingPointNearMinimizationCorner[2] = 4.5
startingPointNearMinimizationCorner[3] = -2.5
startingPointNearMaximizationCorner = ot.NumericalPoint(4)
startingPointNearMaximizationCorner[0] = -2.5
startingPointNearMaximizationCorner[1] = 4.5
startingPointNearMaximizationCorner[2] = 4.5
startingPointNearMaximizationCorner[3] = 4.5

bounds = ot.Interval(ot.NumericalPoint(4, -3.0), ot.NumericalPoint(4, 5.0))

algo = ot.TNC()
problem = ot.OptimizationProblem(levelFunction)
problem.setBounds(bounds)

problem.setMinimization(True)
algo.setProblem(problem)
algo.setStartingPoint(startingPointNearMinimizationCorner)
print('algo=', algo)
algo.run()
result = algo.getResult()
print('result=', printNumericalPoint(result.getOptimalPoint(), 4))
print('multipliers=', printNumericalPoint(result.getLagrangeMultipliers(), 4))

problem.setMinimization(False)
algo.setProblem(problem)
print('algo=', algo)
algo.setStartingPoint(startingPointNearMaximizationCorner)
algo.run()
result = algo.getResult()
print('result=', printNumericalPoint(result.getOptimalPoint(), 4))
print('multipliers=', printNumericalPoint(result.getLagrangeMultipliers(), 4))
