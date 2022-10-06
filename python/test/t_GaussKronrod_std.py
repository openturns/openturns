#! /usr/bin/env python

import openturns as ot
import math

ot.TESTPREAMBLE()

# First, a smooth function
f = ot.SymbolicFunction("x", "sin(x)")
a = -2.5
b = 4.5
# Default parameters
algo = ot.GaussKronrod()
rules = [ot.GaussKronrodRule(ot.GaussKronrodRule.G3K7), ot.GaussKronrodRule(
    ot.GaussKronrodRule.G7K15), ot.GaussKronrodRule(ot.GaussKronrodRule.G11K23), ot.GaussKronrodRule(ot.GaussKronrodRule.G15K31), ot.GaussKronrodRule(ot.GaussKronrodRule.G25K51)]
for i in range(len(rules)):
    algo.setRule(rules[i])
    print("Algo=", algo)
    # High-level interface
    error = ot.Point()
    value = algo.integrate(f, ot.Interval(a, b), error)[0]
    ref = math.cos(a) - math.cos(b)
    print("value=%.6f" % value, ", ref=%.6f" % ref, ", true error below bound? ", abs(ref - value)
          < algo.getMaximumError(), ", estimated error below bound? ", error[0] < algo.getMaximumError())
    # Low-level interface
    # ai = Point()
    # bi = Point()
    # fi = Sample()
    # ei = Point()
    # value2 = algo.integrate(f, a, b, error, ai, bi, fi, ei)[0]
    # ai.add(b)
    # g = f.draw(a, b, 512)
    # lower = Cloud(ai, Point(ai.getDimension()))
    # lower.setColor("magenta")
    # g.add(lower)

# Second, a piecewise smooth function
f = ot.SymbolicFunction("x", "abs(sin(x))")
a = -2.5
b = 4.5
algo = ot.GaussKronrod()
rules = [ot.GaussKronrodRule(ot.GaussKronrodRule.G3K7), ot.GaussKronrodRule(
    ot.GaussKronrodRule.G7K15), ot.GaussKronrodRule(ot.GaussKronrodRule.G11K23), ot.GaussKronrodRule(ot.GaussKronrodRule.G15K31), ot.GaussKronrodRule(ot.GaussKronrodRule.G25K51)]
for i in range(len(rules)):
    algo.setRule(rules[i])
    print("Algo=", algo)
    error = ot.Point()
    value = algo.integrate(f, ot.Interval(a, b), error)[0]
    ref = 4.0 + math.cos(b) - math.cos(a)
    print("value=%.6f" % value, ", ref=%.6f" % ref, ", true error below bound? ", abs(ref - value)
          < algo.getMaximumError(), ", estimated error below bound? ", error[0] < algo.getMaximumError())
    # Low-level interface
    # ai = Point()
    # bi = Point()
    # fi = Sample()
    # ei = Point()
    # value2 = algo.integrate(f, a, b, error, ai, bi, fi, ei)[0]
    # print "ai=", ai
    # print "bi=", bi
    # print "fi=", fi
    # print "ei=", ei
    # ai.add(b)
    # g = f.draw(a, b, 512)
    # lower = Cloud(ai, Point(ai.getDimension()))
    # lower.setColor("magenta")
    # g.add(lower)
