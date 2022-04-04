#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)
ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = [10., 20.]
point2 = [11., 21.]
point3 = [12., 22.]
sample1 = ot.Sample([point1, point2, point3])
referencemin = point1
referencemax = point3

# Iterative extrema, one point at a time
dimension = 2
iterPoint = ot.IterativeExtrema(dimension)
iterPoint.increment(point1)
iterPoint.increment(point2)
iterPoint.increment(point3)
computedmin = iterPoint.getMin()
computedmax = iterPoint.getMax()
ott.assert_almost_equal(referencemin, computedmin)
ott.assert_almost_equal(referencemax, computedmax)
iteration = iterPoint.getIterationNumber()
ott.assert_almost_equal(iteration, 3)

# Iterative extrema, one single sample
iterSample = ot.IterativeExtrema(dimension)
iterSample.increment(sample1)
computedmin = iterSample.getMin()
computedmax = iterSample.getMax()
ott.assert_almost_equal(referencemin, computedmin)
ott.assert_almost_equal(referencemax, computedmax)
iteration = iterSample.getIterationNumber()
ott.assert_almost_equal(iteration, 3)

# Iterative extrema, one single sample, then one point at a time
iterMixed = ot.IterativeExtrema(dimension)
iterMixed.increment(sample1)
iterMixed.increment(point1)
iterMixed.increment(point2)
iterMixed.increment(point3)
computedmin = iterMixed.getMin()
computedmax = iterMixed.getMax()
ott.assert_almost_equal(referencemin,  computedmin)
ott.assert_almost_equal(referencemax, computedmax)
iteration = iterMixed.getIterationNumber()
ott.assert_almost_equal(iteration, 6)
