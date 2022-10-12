#! /usr/bin/env python
import openturns as ot
import math
import openturns.testing as ott

ot.TESTPREAMBLE()

f = ot.SymbolicFunction(["x"], ["sin(x)"])
a = -2.5
b = 4.5
# Integrate sin(t) between a & b --> cos(b) - sin(b)
ref = math.cos(a) - math.cos(b)

all_methods = [
    ot.FejerAlgorithm.FEJERTYPE1,
    ot.FejerAlgorithm.FEJERTYPE2,
    ot.FejerAlgorithm.CLENSHAWCURTIS,
]
# 1D checking
for method in all_methods:
    algo = ot.FejerAlgorithm([100], method)
    value, adaptedNodes = algo.integrateWithNodes(f, ot.Interval(a, b))
    ott.assert_almost_equal(value[0], ref, 1e-10, 1e-10)

g = ot.SymbolicFunction(["x", "y"], ["cos(pi_ * x / 2) * sin(pi_ * y)"])
ref = 8 / (math.pi * math.pi)
interval = ot.Interval([-1, 0], [1, 1])
for method in all_methods:
    algo = ot.FejerAlgorithm([64, 64], method)
    value, adaptedNodes = algo.integrateWithNodes(g, interval)
    ott.assert_almost_equal(value[0], ref, 1e-10, 1e-10)

# Now we use the same calculus using variables changes
h = ot.SymbolicFunction(
    ["x", "y"], ["cos(pi_ * x / 2) * sin(pi_ * y / 2 + pi_/2 ) / 2"]
)
interval = ot.Interval([-1, -1], [1, 1])
for method in all_methods:
    algo = ot.FejerAlgorithm([64, 64], method)
    value, adaptedNodes = algo.integrateWithNodes(h, interval)
    ott.assert_almost_equal(value[0], ref, 1e-10, 1e-10)
