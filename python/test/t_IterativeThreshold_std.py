#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = ot.Point([10., 20.])
point2 = ot.Point([11., 21.])
point3 = ot.Point([12., 22.])
sample1 = ot.Sample([point1, point2, point3])
referencethreshold = ot.Point([0, 3])

# Iterative threshold, one point at a time
dimension = 2
iterthresholdPoint = ot.IterativeThresholdExceedance(dimension, 15.0)
iterthresholdPoint.increment(point1)
iterthresholdPoint.increment(point2)
iterthresholdPoint.increment(point3)
computedthreshold = iterthresholdPoint.getThresholdExceedance()
ott.assert_almost_equal(referencethreshold, computedthreshold)
iteration = iterthresholdPoint.getIteration()
ott.assert_almost_equal(iteration, 3)

# Iterative threshold, one single sample
iterthresholdSample = ot.IterativeThresholdExceedance(dimension, 15.0)
iterthresholdSample.increment(sample1)
computedthreshold = iterthresholdSample.getThresholdExceedance()
ott.assert_almost_equal(referencethreshold, computedthreshold)
iteration = iterthresholdSample.getIteration()
ott.assert_almost_equal(iteration, 3)

# Iterative threshold, one single sample, then one point at a time
iterthresholdMixed = ot.IterativeThresholdExceedance(dimension, 15.0)
iterthresholdMixed.increment(sample1)
iterthresholdMixed.increment(point1)
iterthresholdMixed.increment(point2)
iterthresholdMixed.increment(point3)
computedthreshold = iterthresholdMixed.getThresholdExceedance()
referencethreshold *= 2
ott.assert_almost_equal(referencethreshold, computedthreshold)
iteration = iterthresholdMixed.getIteration()
ott.assert_almost_equal(iteration, 6)
