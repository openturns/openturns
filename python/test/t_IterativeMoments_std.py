#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = [10., 20.]
point2 = [11., 21.]
point3 = [12., 22.]
point4 = [15., 25.]
sample1 = ot.Sample([point1, point2, point3, point4])

referenceMean = sample1.computeMean()
referenceVariance = sample1.computeVariance()
referenceSkewness = sample1.computeSkewness()
referenceKurtosis = sample1.computeKurtosis()

mixedSample = ot.Sample(sample1)
mixedSample.add(point1)
mixedSample.add(point2)
mixedSample.add(point3)
mixedSample.add(point4)
referenceMixedMean = mixedSample.computeMean()
referenceMixedVariance = mixedSample.computeVariance()
referenceMixedSkewness = mixedSample.computeSkewness()
referenceMixedKurtosis = mixedSample.computeKurtosis()

# Iterative moments, one point at a time
dimension = 2
order = 4
iterPoint = ot.IterativeMoments(order, dimension)
iterPoint.increment(point1)
iterPoint.increment(point2)
iterPoint.increment(point3)
iterPoint.increment(point4)

ott.assert_almost_equal(iterPoint.getMean(), referenceMean)
ott.assert_almost_equal(iterPoint.getVariance(), referenceVariance)
ott.assert_almost_equal(iterPoint.getSkewness(), referenceSkewness)
ott.assert_almost_equal(iterPoint.getKurtosis(), referenceKurtosis)
ott.assert_almost_equal(iterPoint.getDimension(), 2)
ott.assert_almost_equal(iterPoint.getOrder(), 4)
ott.assert_almost_equal(iterPoint.getIteration(), 4)


# Iterative moments, one single sample
iterSample = ot.IterativeMoments(order, dimension)
iterSample.increment(sample1)
ott.assert_almost_equal(iterSample.getMean(), referenceMean)
ott.assert_almost_equal(iterSample.getVariance(), referenceVariance)
ott.assert_almost_equal(iterSample.getSkewness(), referenceSkewness)
ott.assert_almost_equal(iterSample.getKurtosis(), referenceKurtosis)
ott.assert_almost_equal(iterSample.getDimension(), 2)
ott.assert_almost_equal(iterSample.getOrder(), 4)
ott.assert_almost_equal(iterSample.getIteration(), 4)


# Iterative moments, sample and points
iterMixed = ot.IterativeMoments(4, dimension)
iterMixed.increment(sample1)
iterMixed.increment(point1)
iterMixed.increment(point2)
iterMixed.increment(point3)
iterMixed.increment(point4)
ott.assert_almost_equal(iterMixed.getMean(), referenceMixedMean)
ott.assert_almost_equal(iterMixed.getVariance(), referenceMixedVariance)
ott.assert_almost_equal(iterMixed.getSkewness(), referenceMixedSkewness)
ott.assert_almost_equal(iterMixed.getKurtosis(), referenceMixedKurtosis)
ott.assert_almost_equal(iterMixed.getDimension(), 2)
ott.assert_almost_equal(iterMixed.getOrder(), 4)
ott.assert_almost_equal(iterMixed.getIteration(), 8)
