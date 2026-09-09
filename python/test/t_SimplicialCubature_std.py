#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

# basic 3D simplex
v1 = [0.0, 0.0, 0.0]
v2 = [1.0, 1.0, 1.0]
v3 = [0.0, 1.0, 1.0]
v4 = [0.0, 0.0, 1.0]
S = [v1, v2, v3, v4]
simplicies = [[0, 1, 2, 3]]
mesh = ot.Mesh(S, simplicies)
f = ot.SymbolicFunction(["x1", "x2", "x3"], ["exp(x1 + x2 + x3)"])
algo = ot.SimplicialCubature()
value = algo.integrate(f, mesh)[0]
ott.assert_almost_equal(value, (m.exp(1.0) - 1.0) ** 3 / 6)

# Test with interval interface
f = ot.SymbolicFunction(["x", "y", "z"], ["sin(x) * cos(y) * exp(z)"])
valueRef = -m.sin(1.0) * (m.cos(1.0) - 1.0) * (m.e - 1.0)
value = algo.integrate(f, ot.Interval([0.0] * 3, [1.0] * 3))
ott.assert_almost_equal(value[0], valueRef)

# Multi-output integrand over the simplex
f = ot.SymbolicFunction(["x1", "x2", "x3"], ["x1", "x2"])
value = algo.integrate(f, mesh)
ott.assert_almost_equal(value, [1.0 / 24.0, 1.0 / 12.0])

# Adaptive refinement on a peaked integrand over [0, 1]^3
f = ot.SymbolicFunction(["x1", "x2", "x3"], ["exp(-10.0 * (x1 + x2 + x3))"])
valueRef = ((1.0 - m.exp(-10.0)) / 10.0) ** 3
algo.setRule(3)
algo.setMaximumRelativeError(1e-5)
value = algo.integrate(f, ot.Interval([0.0] * 3, [1.0] * 3))
ott.assert_almost_equal(value[0], valueRef, 1e-6, 1e-12)

# Rule 4 accuracy over the simplex
f = ot.SymbolicFunction(["x1", "x2", "x3"], ["exp(x1 + x2 + x3)"])
algo.setRule(4)
algo.setMaximumRelativeError(1e-5)
value = algo.integrate(f, mesh)[0]
ott.assert_almost_equal(value, (m.exp(1.0) - 1.0) ** 3 / 6, 1e-6, 1e-12)

# Test empty mesh guard
algo.setRule(1)
algo.setMaximumRelativeError(1e-5)
empty_mesh = ot.Mesh([[0.0, 0.0, 0.0]], [])
value_empty = algo.integrate(f, empty_mesh)
ott.assert_almost_equal(value_empty, [0.0])

# Test input dimension mismatch
with ott.assert_raises(TypeError):
    algo.integrate(ot.SymbolicFunction(["x", "y"], ["1.0"]), mesh)

# Test accessor methods
algo.setRule(2)
assert algo.getRule() == 2
algo.setMaximumAbsoluteError(1.0e-8)
ott.assert_almost_equal(algo.getMaximumAbsoluteError(), 1.0e-8)
algo.setMaximumRelativeError(1.0e-6)
ott.assert_almost_equal(algo.getMaximumRelativeError(), 1.0e-6)
algo.setMaximumCallsNumber(10000)
ott.assert_almost_equal(algo.getMaximumCallsNumber(), 10000)

# Test invalid rule
with ott.assert_raises(TypeError):
    algo.setRule(0)
with ott.assert_raises(TypeError):
    algo.setRule(5)

# Test repr and str
algo.setRule(1)
print("repr=", repr(algo))
print("str=", str(algo))

# Test the EvaluationBlockSize ResourceMap key
ot.ResourceMap.SetAsUnsignedInteger("SimplicialCubature-EvaluationBlockSize", 2)
f = ot.SymbolicFunction(["x1", "x2", "x3"], ["exp(x1 + x2 + x3)"])
algo.setRule(1)
algo.setMaximumAbsoluteError(0.0)
algo.setMaximumRelativeError(1.0e-5)
algo.setMaximumCallsNumber(50000)
value = algo.integrate(f, mesh)[0]
ott.assert_almost_equal(value, (m.exp(1.0) - 1.0) ** 3 / 6, 1e-4, 1e-4)
ot.ResourceMap.SetAsUnsignedInteger("SimplicialCubature-EvaluationBlockSize", 2048)

print("OK")
