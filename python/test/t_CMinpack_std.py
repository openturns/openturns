#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import math as m
import sys

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)
ot.Log.Show(ot.Log.ALL)


def progress(percent):
    sys.stderr.write('-- progress=' + str(percent) + '%\n')


def stop():
    sys.stderr.write('-- stop?\n')
    return False


n = 3
m = 10

x = [[0.5 + i] for i in range(m)]


model = ot.SymbolicFunction(['a', 'b', 'c', 'x'], ['a + b * exp(c * x)'])
p_ref = [2.8, 1.2, 0.5]  # a, b, c
modelx = ot.ParametricFunction(model, [0, 1, 2], p_ref)
y = modelx(x)


def residualFunction_py(p):
    modelx = ot.ParametricFunction(model, [0, 1, 2], p)
    return [modelx(x[i])[0] - y[i, 0] for i in range(m)]


residualFunction = ot.PythonFunction(n, m, residualFunction_py)

bounds = ot.Interval([0, 0, 0], [2.5, 8.0, 19])


for bound in [True, False]:
    problem = ot.LeastSquaresProblem(residualFunction)
    if bound:
        problem.setBounds(bounds)
    startingPoint = [1.0] * n
    algo = ot.CMinpack(problem)
    algo.setStartingPoint(startingPoint)
    # algo.setProgressCallback(progress)
    # algo.setStopCallback(stop)
    algo.run()
    result = algo.getResult()
    # print(result.getInputSample())
    # print(result.getOutputSample())
    x_star = result.getOptimalPoint()
    print(result)
    if bound:
        assert x_star in bounds, "optimal point not in bounds"
    else:
        ott.assert_almost_equal(x_star, p_ref)
