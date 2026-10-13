#! /usr/bin/env python

import openturns as ot
import math as m
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

f = ot.SymbolicFunction(["t", "y0", "y1"], ["t - y0", "y1 + t^2"])
nt = 100
timeGrid = [(i**2.0) / (nt - 1.0) ** 2.0 for i in range(nt)]
phi = ot.ParametricFunction(f, [0], [0.0])
solver = ot.Fehlberg(phi)
print("ODE solver=", solver)
initialState = [1.0, -1.0]
print("time grid=", ot.Point(timeGrid))
result = solver.solve(initialState, timeGrid)
print("result=", result)
print("last value=", result[nt - 1])
t = timeGrid[nt - 1]
ref = ot.Point(2)
ref[0] = -1.0 + t + 2.0 * m.exp(-t)
ref[1] = -2.0 + -2.0 * t - t * t + m.exp(t)
print("ref. value=", ref)

# Extra coverage: ctors, error branches, save/load (no prints to keep expout stable)

assert isinstance(repr(solver), str)
assert isinstance(str(solver), str)
defaultSolver = ot.Fehlberg()
assert isinstance(repr(defaultSolver), str)
assert isinstance(str(defaultSolver), str)
# all available orders can be built; solve a short grid with loose precision
shortGrid = [0.0, 0.2, 0.5]
for order in [0, 1, 2, 3, 4]:
    orderSolver = ot.Fehlberg(phi, 1e-4, order)
    assert isinstance(repr(orderSolver), str)
    if order > 0:
        shortResult = orderSolver.solve(initialState, shortGrid)
        assert shortResult.getSize() == len(shortGrid)
        assert shortResult.getDimension() == 2
ott.assert_almost_equal(
    ot.Fehlberg(phi, 1e-8, 4).solve(initialState, [0.0, 0.5])[1],
    [-1.0 + 0.5 + 2.0 * m.exp(-0.5), -2.0 - 2.0 * 0.5 - 0.5 * 0.5 + m.exp(0.5)],
    1e-4,
    1e-4,
)
with ott.assert_raises(Exception):
    ot.Fehlberg(phi, 1e-10, 5)
# empty grid returns an empty sample
assert solver.solve(initialState, []).getSize() == 0
with ott.assert_raises(Exception):
    solver.solve([1.0], [0.0, 1.0])
with ott.assert_raises(Exception):
    solver.solve(initialState, [0.0, 1.0, 0.5])
# non-finite transition values must raise
badTransition = ot.ParametricFunction(
    ot.PythonFunction(3, 2, lambda X: [float("inf"), float("inf")]), [0], [0.0]
)
with ott.assert_raises(Exception):
    ot.Fehlberg(badTransition, 1e-6, 4).solve([0.0, 0.0], [0.0, 0.1])
# save/load round-trip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("fehlberg.xml"))
study.add("solver", solver)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("fehlberg.xml"))
loadedStudy.load()
assert loadedStudy.hasObject("solver")

os.remove("fehlberg.xml")
