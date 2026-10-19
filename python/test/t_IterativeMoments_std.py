#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# We create a Sample
point1 = [10.0, 20.0]
point2 = [11.0, 21.0]
point3 = [12.0, 22.0]
point4 = [15.0, 25.0]
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
ott.assert_almost_equal(iterPoint.getIterationNumber(), 4)


# Iterative moments, one single sample
iterSample = ot.IterativeMoments(order, dimension)
iterSample.increment(sample1)
ott.assert_almost_equal(iterSample.getMean(), referenceMean)
ott.assert_almost_equal(iterSample.getVariance(), referenceVariance)
ott.assert_almost_equal(iterSample.getSkewness(), referenceSkewness)
ott.assert_almost_equal(iterSample.getKurtosis(), referenceKurtosis)
ott.assert_almost_equal(iterSample.getDimension(), 2)
ott.assert_almost_equal(iterSample.getOrder(), 4)
ott.assert_almost_equal(iterSample.getIterationNumber(), 4)


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
ott.assert_almost_equal(iterMixed.getIterationNumber(), 8)

# Error branches on an empty statistics

empty = ot.IterativeMoments(4, dimension)
for method in [
    "getMean",
    "getVariance",
    "getSkewness",
    "getKurtosis",
    "getCoefficientOfVariation",
    "getStandardErrorOfTheMean",
    "getStandardDeviation",
]:
    with ott.assert_raises(Exception):
        getattr(empty, method)()

# Declared order too low
order1 = ot.IterativeMoments(1, dimension)
order1.increment(point1)
ott.assert_almost_equal(order1.getMean(), point1, 1e-14, 1e-14)
for method in ["getVariance", "getSkewness", "getKurtosis"]:
    with ott.assert_raises(Exception):
        getattr(order1, method)()

# Degenerate iteration numbers: variance/skewness/kurtosis of tiny samples
tiny = ot.IterativeMoments(4, dimension)
tiny.increment(point1)
ott.assert_almost_equal(tiny.getVariance(), [0.0, 0.0], 0, 0)
tiny.increment(point2)
ott.assert_almost_equal(tiny.getSkewness(), [0.0, 0.0], 0, 0)
tiny.increment(point3)
ott.assert_almost_equal(tiny.getKurtosis(), [0.0, 0.0], 0, 0)

# Constant sample: skewness and kurtosis are not defined
constant = ot.IterativeMoments(4, 1)
constant.increment(ot.Sample([[2.0]] * 4))
with ott.assert_raises(Exception):
    constant.getSkewness()
with ott.assert_raises(Exception):
    constant.getKurtosis()

# Order-2 recurrence branch plus derived statistics
iter2 = ot.IterativeMoments(2, dimension)
iter2.increment(sample1)
ott.assert_almost_equal(iter2.getMean(), referenceMean)
ott.assert_almost_equal(iter2.getVariance(), referenceVariance)
ott.assert_almost_equal(
    iter2.getCoefficientOfVariation(),
    [referenceVariance[0] ** 0.5 / referenceMean[0],
     referenceVariance[1] ** 0.5 / referenceMean[1]],
    1e-12,
    1e-12,
)
ott.assert_almost_equal(
    iter2.getStandardDeviation(),
    [referenceVariance[0] ** 0.5, referenceVariance[1] ** 0.5],
    1e-12,
    1e-12,
)
ott.assert_almost_equal(
    iter2.getStandardErrorOfTheMean(),
    [(referenceVariance[0] / 4) ** 0.5, (referenceVariance[1] / 4) ** 0.5],
    1e-12,
    1e-12,
)
assert len(repr(iter2)) > 0
assert len(str(iter2)) > 0
assert iter2.getCentralMoments().getSize() == 2

# Persistence roundtrip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("t_IterativeMoments_std_tmp.xml"))
study.add("moments", iter2)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("t_IterativeMoments_std_tmp.xml"))
loadedStudy.load()
loaded = ot.IterativeMoments(2, dimension)
loadedStudy.fillObject("moments", loaded)
ott.assert_almost_equal(loaded.getMean(), referenceMean)
ott.assert_almost_equal(loaded.getVariance(), referenceVariance)
os.remove("t_IterativeMoments_std_tmp.xml")
