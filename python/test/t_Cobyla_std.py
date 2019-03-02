#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m

ot.TESTPREAMBLE()


def printPoint(point, digits):
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
levelFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["x1+2*x2-3*x3+4*x4"])
startingPoint = ot.Point(4, 0.0)
algo = ot.Cobyla(ot.NearestPointProblem(levelFunction, 3.0))
algo.setStartingPoint(startingPoint)
print('algo=', algo)
algo.run()
result = algo.getResult()
print('x^=', printPoint(result.getOptimalPoint(), 4))
print('f(x^)=', printPoint(result.getOptimalValue(), 4))
print('lambda^=', printPoint(result.getLagrangeMultipliers(), 4))

# non-linear
levelFunction = ot.SymbolicFunction(
    ["x1", "x2", "x3", "x4"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
startingPoint = ot.Point(4, 0.0)
algo = ot.Cobyla(ot.NearestPointProblem(levelFunction, 3.0))
algo.setStartingPoint(startingPoint)
algo.setMaximumEvaluationNumber(400)
algo.setMaximumAbsoluteError(1.0e-10)
algo.setMaximumRelativeError(1.0e-10)
algo.setMaximumResidualError(1.0e-10)
algo.setMaximumConstraintError(1.0e-10)
algo.run()
result = algo.getResult()
print('x^=', printPoint(result.getOptimalPoint(), 4))
print('f(x^)=', printPoint(result.getOptimalValue(), 4))
print('lambda^=', printPoint(result.getLagrangeMultipliers(), 4))

# bounds
linear = ot.SymbolicFunction(
    ['x1', 'x2', 'x3', 'x4'], ['x1+2*x2-3*x3+4*x4'])

dim = 4
startingPoint = [0.] * dim

bounds = ot.Interval([-3.] * dim, [5.] * dim)

for minimization in [True, False]:

    problem = ot.OptimizationProblem(
        linear, ot.Function(), ot.Function(), bounds)
    problem.setMinimization(minimization)
    algo = ot.Cobyla(problem)
    algo.setMaximumEvaluationNumber(150)
    algo.setStartingPoint(startingPoint)
    print('algo=', algo)
    algo.run()
    result = algo.getResult()
    print('x^=', printPoint(result.getOptimalPoint(), 4))
    print('f(x^)=', printPoint(result.getOptimalValue(), 4))
    print('lambda^=', printPoint(result.getLagrangeMultipliers(), 4))
