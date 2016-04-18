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


# bounds
linear = ot.NumericalMathFunction(
    ['x1', 'x2', 'x3', 'x4'], ['y1'], ['x1+2*x2-3*x3+4*x4'])

dim = 4
startingPoint = [0.] * dim

bounds = ot.Interval([-3.] * dim, [5.] * dim)

for algo in [ot.SLSQP(), ot.LBFGS(), ot.NelderMead()]:
    for minimization in [True, False]:
        for inequality in [True, False]:
            for equality in [True, False]:
                problem = ot.OptimizationProblem(
                    linear, ot.NumericalMathFunction(), ot.NumericalMathFunction(), bounds)
                problem.setMinimization(minimization)
                if inequality:
                    # x3 <= x1
                    problem.setInequalityConstraint(
                        ot.NumericalMathFunction(['x1', 'x2', 'x3', 'x4'], ['ineq'], ['x1-x3']))
                if equality:
                    # x4 = 2
                    problem.setEqualityConstraint(
                        ot.NumericalMathFunction(['x1', 'x2', 'x3', 'x4'], ['eq'], ['x4-2']))
                try:
                    algo.setProblem(problem)
                    algo.setStartingPoint(startingPoint)
                    print('algo=', algo)
                    algo.run()
                    result = algo.getResult()
                    print('x^=', printNumericalPoint(
                        result.getOptimalPoint(), 4))
                except:
                    print('-- Not supported: algo=', algo.getClassName(),
                          'inequality=', inequality, 'equality=', equality)
