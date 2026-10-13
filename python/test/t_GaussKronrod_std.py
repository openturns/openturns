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

# State retrieval without out-arguments, see issue #3169
f0 = ot.SymbolicFunction(["x"], ["x"])
algo0 = ot.GaussKronrod()
ref0 = 0.5
value0 = algo0.integrate(f0, ot.Interval(0.0, 1.0))[0]
ott.assert_almost_equal(value0, ref0, 1e-12, 1e-12)
assert algo0.getIterationNumber() == 2
assert algo0.getEvaluationSubIntervals().getSize() == 2
assert algo0.getLocalErrorEstimates().getSize() == 2
assert algo0.getErrorEstimate() >= 0.0
# the bare call leaves the same state as the out-arguments call
error1 = ot.Point()
ai1 = ot.Point()
bi1 = ot.Point()
fi1 = ot.Sample()
ei1 = ot.Point()
value1 = algo0.integrate(f0, 0.0, 1.0, error1, ai1, bi1, fi1, ei1)[0]
ott.assert_almost_equal(value1, ref0, 1e-12, 1e-12)
ott.assert_almost_equal(algo0.getErrorEstimate(), error1[0])
assert algo0.getIterationNumber() == ai1.getSize()
subIntervals = algo0.getEvaluationSubIntervals()
for i in range(subIntervals.getSize()):
    ott.assert_almost_equal(subIntervals[i, 0], ai1[i])
    ott.assert_almost_equal(subIntervals[i, 1], bi1[i])
ott.assert_almost_equal(list(algo0.getLocalErrorEstimates()), list(ei1))

# A zero error target is rejected instead of misreporting one used interval
with ott.assert_raises(Exception):
    ot.GaussKronrod(100, 0.0, ot.GaussKronrodRule(ot.GaussKronrodRule.G3K7))
with ott.assert_raises(Exception):
    algo0.setMaximumError(0.0)
# Infinite error targets are rejected as well
with ott.assert_raises(Exception):
    ot.GaussKronrod(100, float("inf"), ot.GaussKronrodRule(ot.GaussKronrodRule.G3K7))
with ott.assert_raises(Exception):
    algo0.setMaximumError(float("inf"))
