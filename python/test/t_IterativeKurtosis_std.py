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
point3 = ot.Point([12.,22.])
point4 = ot.Point([15.,25.])
sample1 = ot.Sample([point1,point2,point3,point4])
referenceskewness = sample1.computeSkewness()
referencekurtosis = sample1.computeKurtosis()
mixedsample = ot.Sample(sample1)
mixedsample.add(point1)
mixedsample.add(point2)
mixedsample.add(point3)
mixedsample.add(point4)
referencemixedskewness = mixedsample.computeSkewness()
referencemixedkurtosis = mixedsample.computeKurtosis()

# Iterative kurtosis, one point at a time
print("Iterative kurtosis, one point at a time")
dimension = 2
iterkurtosisPoint = ot.IterativeKurtosis(dimension)
iterkurtosisPoint.increment(point1)
iterkurtosisPoint.increment(point2)
iterkurtosisPoint.increment(point3)
iterkurtosisPoint.increment(point4)
computedskewness = iterkurtosisPoint.getSkewness()
size = sample1.getSize()
factor = sqrt(size) * sqrt(size - 1.0) / (size - 2.0)
computedskewness *= factor
for i in range(2):
  assert_almost_equal(referenceskewness[i], computedskewness[i])

computedkurtosis = iterkurtosisPoint.getKurtosis()
print("result=",computedkurtosis)
#for i in range(2):
  #assert_almost_equal(referencekurtosis[i], computedkurtosis[i])

iteration = iterkurtosisPoint.getIteration()
assert(iteration == 4)

# Iterative kurtosis, one single sample
print("Iterative kurtosis, one single sample")
iterkurtosisSample = ot.IterativeKurtosis(dimension)
iterkurtosisSample.increment(sample1)
computedskewness = iterkurtosisSample.getSkewness()
computedskewness *= factor
for i in range(2):
  assert_almost_equal(referenceskewness[i], computedskewness[i])

computedkurtosis = iterkurtosisSample.getKurtosis()
print("result=",computedkurtosis)
#for i in range(2):
  #assert_almost_equal(referencekurtosis[i], computedkurtosis[i])

iteration = iterkurtosisSample.getIteration()
assert(iteration == 4)

# Iterative kurtosis, one single sample, then one point at a time
print("Iterative kurtosis, one single sample, then one point at a time")
iterkurtosisMixed = ot.IterativeKurtosis(dimension)
iterkurtosisMixed.increment(sample1)
iterkurtosisMixed.increment(point1)
iterkurtosisMixed.increment(point2)
iterkurtosisMixed.increment(point3)
iterkurtosisMixed.increment(point4)
computedskewness = iterkurtosisMixed.getSkewness()
size = sample1.getSize() * 2
factor = sqrt(size) * sqrt(size - 1.0) / (size - 2.0)
computedskewness *= factor
for i in range(2):
  assert_almost_equal(referencemixedskewness[i], computedskewness[i])

computedkurtosis = iterkurtosisMixed.getKurtosis()
print("result=",computedkurtosis)
#for i in range(2):
  #assert_almost_equal(referencemixedkurtosis[i], computedkurtosis[i])

iteration = iterkurtosisMixed.getIteration()
assert(iteration == 8)
