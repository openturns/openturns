#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(5)

ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = [10.0, 20.0]
point2 = [11.0, 21.0]
point3 = [12.0, 22.0]
sample1 = [point1, point2, point3]
referencethreshold = [0, 3]

# Iterative threshold, one point at a time
dimension = 2
algo = ot.IterativeThresholdExceedance(dimension, ot.Greater(), 15.0)
algo.increment(point1)
algo.increment(point2)
algo.increment(point3)
computedthreshold = algo.getThresholdExceedance()
ott.assert_almost_equal(referencethreshold, computedthreshold)
iteration = algo.getIterationNumber()
ott.assert_almost_equal(iteration, 3)

# Iterative threshold, one single sample
iterthresholdSample = ot.IterativeThresholdExceedance(dimension, ot.Greater(), 15.0)
iterthresholdSample.increment(sample1)
computedthreshold = iterthresholdSample.getThresholdExceedance()
ott.assert_almost_equal(referencethreshold, computedthreshold)
iteration = iterthresholdSample.getIterationNumber()
ott.assert_almost_equal(iteration, 3)

# Iterative threshold, one single sample, then one point at a time
iterthresholdMixed = ot.IterativeThresholdExceedance(dimension, ot.Greater(), 15.0)
iterthresholdMixed.increment(sample1)
iterthresholdMixed.increment(point1)
iterthresholdMixed.increment(point2)
iterthresholdMixed.increment(point3)
computedthreshold = iterthresholdMixed.getThresholdExceedance()
referencethreshold = [0, 6]
ott.assert_almost_equal(referencethreshold, computedthreshold)
iteration = iterthresholdMixed.getIterationNumber()
ott.assert_almost_equal(iteration, 6)
