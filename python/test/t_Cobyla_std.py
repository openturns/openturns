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
levelFunction = ot.NumericalMathFunction(
    ["x1", "x2", "x3", "x4"], ["y1"], ["x1+2*x2-3*x3+4*x4"])
specific = ot.CobylaSpecificParameters()
startingPoint = ot.NumericalPoint(4, 0.0)
algo = ot.Cobyla(specific, ot.OptimizationProblem(levelFunction, 3.0))
algo.setStartingPoint(startingPoint)
print('algo=', algo)
algo.run()
result = algo.getResult()
print('x^=', printNumericalPoint(result.getOptimalPoint(), 4))





# non-linear
levelFunction = ot.NumericalMathFunction(
    ["x1", "x2", "x3", "x4"], ["y1"], ["x1*cos(x1)+2*x2*x3-3*x3+4*x3*x4"])
specific = ot.CobylaSpecificParameters()
startingPoint = ot.NumericalPoint(4, 0.0)
algo = ot.Cobyla(specific, ot.OptimizationProblem(levelFunction, 3.0))
algo.setStartingPoint(startingPoint)
algo.setMaximumIterationsNumber(400)
algo.setMaximumAbsoluteError(1.0e-10)
algo.setMaximumRelativeError(1.0e-10)
algo.setMaximumResidualError(1.0e-10)
algo.setMaximumConstraintError(1.0e-10)
algo.run()
print('algo=', algo)
algo.run()
result = algo.getResult()
print('x^=', printNumericalPoint(result.getOptimalPoint(), 4))




# bounds
linear = ot.NumericalMathFunction(
    ['x1', 'x2', 'x3', 'x4'], ['y1'], ['x1+2*x2-3*x3+4*x4'])

dim = 4
startingPoint = [0.] * dim

bounds = ot.Interval([-3.]*dim,[5.]*dim)

for minimization in [True, False]:

    problem = ot.OptimizationProblem(linear, ot.NumericalMathFunction(), ot.NumericalMathFunction(), bounds)
    problem.setMinimization(minimization)
    specific = ot.CobylaSpecificParameters()
    algo = ot.Cobyla(specific, problem)
    algo.setMaximumIterationsNumber(150)
    algo.setStartingPoint(startingPoint)
    print('algo=', algo)
    algo.run()
    result = algo.getResult()
    print('x^=', printNumericalPoint(result.getOptimalPoint(), 4))


