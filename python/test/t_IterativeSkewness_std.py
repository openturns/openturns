#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import assert_almost_equal
from math import sqrt

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = ot.Point([10.,20.])
point2 = ot.Point([11.,21.])
point3 = ot.Point([15.,25.])
sample1 = ot.Sample([point1,point2,point3])
referenceskewness = sample1.computeSkewness()
mixedsample = ot.Sample(sample1)
mixedsample.add(point1)
mixedsample.add(point2)
mixedsample.add(point3)
referencemixedskewness = mixedsample.computeSkewness()

# Iterative skewness, one point at a time
print("Iterative skewness, one point at a time")
dimension = 2
iterskewnessPoint = ot.IterativeSkewness(dimension)
iterskewnessPoint.increment(point1)
iterskewnessPoint.increment(point2)
iterskewnessPoint.increment(point3)
computedskewness = iterskewnessPoint.getSkewness()
print("result=",computedskewness)
size = sample1.getSize()
factor = sqrt(size) * sqrt(size - 1.0) / (size - 2.0)
computedskewness *= factor
for i in range(2):
  assert_almost_equal(referenceskewness[i], computedskewness[i])
iteration = iterskewnessPoint.getIteration()
nptest.assert_equal(iteration, 3)

# Iterative skewness, one single sample
print("Iterative skewness, one single sample")
iterskewnessSample = ot.IterativeSkewness(dimension)
iterskewnessSample.increment(sample1)
computedskewness = iterskewnessSample.getSkewness()
print("result=",computedskewness)
computedskewness *= factor
for i in range(2):
  assert_almost_equal(referenceskewness[i], computedskewness[i])
iteration = iterskewnessSample.getIteration()
nptest.assert_equal(iteration, 3)

# Iterative skewness, one single sample, then one point at a time
print("Iterative skewness, one single sample, then one point at a time")
iterskewnessMixed = ot.IterativeSkewness(dimension)
iterskewnessMixed.increment(sample1)
iterskewnessMixed.increment(point1)
iterskewnessMixed.increment(point2)
iterskewnessMixed.increment(point3)
computedskewness = iterskewnessMixed.getSkewness()
print("result=",computedskewness)
size = sample1.getSize() * 2
factor = sqrt(size) * sqrt(size - 1.0) / (size - 2.0)
computedskewness *= factor
for i in range(2):
  assert_almost_equal(referencemixedskewness[i], computedskewness[i])
iteration = iterskewnessMixed.getIteration()
nptest.assert_equal(iteration, 6)
