#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m

ot.TESTPREAMBLE()

f = ot.NumericalMathFunction(['t', 'y0', 'y1'], ['dy0', 'dy1'], ['t - y0', 'y1 + t^2'])
phi = ot.TemporalFunction(f)
solver = ot.RungeKutta(phi)
print('ODE solver=', solver)
initialState = [1.0, -1.0]
nt = 100
timeGrid = list(map(lambda i:(i**2.0) / (nt - 1.0)**2.0, range(nt)))
print('time grid=', ot.NumericalPoint(timeGrid))
result = solver.solve(initialState, timeGrid)
print('result=', result)
print('last value=', result[nt - 1])
t = timeGrid[nt - 1]
ref = ot.NumericalPoint(2)
ref[0] = -1.0 + t + 2.0 * m.exp(-t)
ref[1] = -2.0 + -2.0 * t - t * t + m.exp(t)
print('ref. value=', ref)
