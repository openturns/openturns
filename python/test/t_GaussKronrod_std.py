#! /usr/bin/env python

from __future__ import print_function
from openturns import *
import math

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # First, a smooth function
    f = SymbolicFunction("x", "sin(x)")
    a = -2.5
    b = 4.5
    # Default parameters
    algo = GaussKronrod()
    rules = [GaussKronrodRule(GaussKronrodRule.G3K7), GaussKronrodRule(
        GaussKronrodRule.G7K15), GaussKronrodRule(GaussKronrodRule.G11K23), GaussKronrodRule(GaussKronrodRule.G15K31), GaussKronrodRule(GaussKronrodRule.G25K51)]
    for i in range(len(rules)):
        algo.setRule(rules[i])
        print("Algo=", algo)
        # High-level interface
        error = Point()
        value = algo.integrate(f, Interval(a, b), error)[0]
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
        # g.draw("Smooth_function_adaptation_" + str(i))

    # Second, a piecewise smooth function
    f = SymbolicFunction("x", "abs(sin(x))")
    a = -2.5
    b = 4.5
    algo = GaussKronrod()
    rules = [GaussKronrodRule(GaussKronrodRule.G3K7), GaussKronrodRule(
        GaussKronrodRule.G7K15), GaussKronrodRule(GaussKronrodRule.G11K23), GaussKronrodRule(GaussKronrodRule.G15K31), GaussKronrodRule(GaussKronrodRule.G25K51)]
    for i in range(len(rules)):
        algo.setRule(rules[i])
        print("Algo=", algo)
        error = Point()
        value = algo.integrate(f, Interval(a, b), error)[0]
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
        # g.draw("Nonsmooth_function_adaptation_" + str(i))
except:
    import sys
    print("t_GaussKronrod_std.py", sys.exc_info()[0], sys.exc_info()[1])
