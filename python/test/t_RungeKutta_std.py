#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m

ot.TESTPREAMBLE()

f = ot.SymbolicFunction(['t', 'y0', 'y1'], ['t - y0', 'y1 + t^2'])
nt = 100
timeGrid = [(i**2.0) / (nt - 1.0)**2.0 for i in range(nt)]
phi = ot.ParametricFunction(f, [0], [0.0])
solver = ot.RungeKutta(phi)
print('ODE solver=', solver)
initialState = [1.0, -1.0]
print('time grid=', ot.Point(timeGrid))
result = solver.solve(initialState, timeGrid)
print('result=', result)
print('last value=', result[nt - 1])
t = timeGrid[nt - 1]
ref = ot.Point(2)
ref[0] = -1.0 + t + 2.0 * m.exp(-t)
ref[1] = -2.0 + -2.0 * t - t * t + m.exp(t)
print('ref. value=', ref)
grid = ot.RegularGrid(0.0, 0.01, nt)
result = solver.solve(initialState, grid)
print('result=', result)
print('last value=', result[nt - 1])
t = grid.getValue(nt - 1)
ref[0] = -1.0 + t + 2.0 * m.exp(-t)
ref[1] = -2.0 + -2.0 * t - t * t + m.exp(t)
print('ref. value=', ref)
