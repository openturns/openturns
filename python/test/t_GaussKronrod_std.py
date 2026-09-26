#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math

ot.TESTPREAMBLE()

# First, a smooth function
f = ot.SymbolicFunction("x", "sin(x)")
a = -2.5
b = 4.5
# Default parameters
algo = ot.GaussKronrod()
rules = ot.GaussKronrod.GetRules()
for i in range(len(rules)):
    algo.setRule(rules[i])
    print("Algo=", algo)
    # High-level interface
    error = ot.Point()
    value = algo.integrate(f, ot.Interval(a, b), error)[0]
    ref = math.cos(a) - math.cos(b)
    ott.assert_almost_equal(value, ref, 1e-5, algo.getMaximumError())

# Second, a piecewise smooth function
f = ot.SymbolicFunction("x", "abs(sin(x))")
a = -2.5
b = 4.5
algo = ot.GaussKronrod()
rules = ot.GaussKronrod.GetRules()
rules[0] = ot.GaussKronrod.GetRuleFromName("G3K7")

for i in range(len(rules)):
    algo.setRule(rules[i])
    print("Algo=", algo)
    error = ot.Point()
    value = algo.integrate(f, ot.Interval(a, b), error)[0]
    ref = 4.0 + math.cos(b) - math.cos(a)
    ott.assert_almost_equal(value, ref, 1e-5, algo.getMaximumError())
