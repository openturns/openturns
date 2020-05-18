#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = ot.Point([10.,20.])
point2 = ot.Point([11.,21.])
point3 = ot.Point([12.,22.])
sample1 = ot.Sample([point1,point2,point3])
referencemin = point1
referencemax = point3

# Iterative extrema, one point at a time
print("Iterative extrema, one point at a time")
dimension = 2
iterPoint = ot.IterativeExtrema(dimension)
iterPoint.increment(point1)
iterPoint.increment(point2)
iterPoint.increment(point3)
print("result=",iterPoint)
computedmin = iterPoint.getMin()
computedmax = iterPoint.getMax()
assert(referencemin == computedmin)
assert(referencemax == computedmax)
iteration = iterPoint.getIteration()
assert(iteration == 3)

# Iterative extrema, one single sample
print("Iterative extrema, one single sample")
iterSample = ot.IterativeExtrema(dimension)
iterSample.increment(sample1)
print("result=",iterSample)
computedmin = iterSample.getMin()
computedmax = iterSample.getMax()
assert(referencemin == computedmin)
assert(referencemax == computedmax)
iteration = iterSample.getIteration()
assert(iteration == 3)

# Iterative extrema, one single sample, then one point at a time
print("Iterative extrema, one single sample, then one point at a time")
iterMixed = ot.IterativeExtrema(dimension)
iterMixed.increment(sample1)
iterMixed.increment(point1)
iterMixed.increment(point2)
iterMixed.increment(point3)
print("result=",iterMixed)
computedmin = iterMixed.getMin()
computedmax = iterMixed.getMax()
assert(referencemin == computedmin)
assert(referencemax == computedmax)
iteration = iterMixed.getIteration()
assert(iteration == 6)
