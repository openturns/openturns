#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import numpy.testing as nptest

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = ot.Point([10.,20.])
point2 = ot.Point([11.,21.])
point3 = ot.Point([12.,22.])
sample1 = ot.Sample([point1,point2,point3])
referencevariance = sample1.computeVariance()
mixedsample = ot.Sample(sample1)
mixedsample.add(point1)
mixedsample.add(point2)
mixedsample.add(point3)
referencemixedvariance = mixedsample.computeVariance()

# Iterative variance, one point at a time
print("Iterative variance, one point at a time")
dimension = 2
itervariancePoint = ot.IterativeVariance(dimension)
itervariancePoint.increment(point1)
itervariancePoint.increment(point2)
itervariancePoint.increment(point3)
print("result=",itervariancePoint)
computedvariance = itervariancePoint.getVariance()
nptest.assert_equal(referencevariance, computedvariance)
iteration = itervariancePoint.getIteration()
nptest.assert_equal(iteration, 3)

# Iterative variance, one single sample
print("Iterative variance, one single sample")
itervarianceSample = ot.IterativeVariance(dimension)
itervarianceSample.increment(sample1)
print("result=",itervarianceSample)
computedvariance = itervarianceSample.getVariance()
nptest.assert_equal(referencevariance, computedvariance)
iteration = itervarianceSample.getIteration()
nptest.assert_equal(iteration, 3)

# Iterative variance, one single sample, then one point at a time
print("Iterative variance, one single sample, then one point at a time")
itervarianceMixed = ot.IterativeVariance(dimension)
itervarianceMixed.increment(sample1)
itervarianceMixed.increment(point1)
itervarianceMixed.increment(point2)
itervarianceMixed.increment(point3)
print("result=",itervarianceMixed)
computedvariance = itervarianceMixed.getVariance()
nptest.assert_equal(referencemixedvariance, computedvariance)
iteration = itervarianceMixed.getIteration()
nptest.assert_equal(iteration, 6)
