#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# First constructor: distribution of the maximum of the components
# of a random vector given its joint distribution
# Second constructor: distribution of the maximum of independent
# random variables given their distributions
# Third constructor: distribution of the maximum of independent
# identically distributed random variables
R = ot.CorrelationMatrix(3)
R[0, 1] = 0.5
coll = [
    ot.MaximumDistribution(ot.Normal(5)),
    ot.MaximumDistribution([ot.Normal()] * 5),
    ot.MaximumDistribution(ot.Normal(), 5),
    ot.MaximumDistribution(ot.Normal([0.0] * 3, [1.0] * 3, R)),
]
for index, distribution in enumerate(coll):
    print("Distribution ", distribution)

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Confidence regions
    (
        interval,
        threshold,
    ) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    if index < 3:
        ott.assert_almost_equal(interval.getLowerBound(), [-0.11278], 1e-4, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [2.50221], 1e-4, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [-0.770165], 1e-4, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [2.33536], 1e-4, 0.0)
    print("threshold=", threshold)

    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=%.5g" % beta)

    (
        interval,
        beta,
    ) = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=%.5g" % beta)

    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    if index < 3:
        ott.assert_almost_equal(interval.getLowerBound(), [-7.65063], 1e-4, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [2.31868], 1e-4, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [-7.65063], 1e-4, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [2.10094], 1e-4, 0.0)
    print("beta=%.5g" % beta)

    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    if index < 3:
        ott.assert_almost_equal(interval.getLowerBound(), [0.123843], 1e-4, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [7.65063], 1e-4, 0.0)
    else:
        ott.assert_almost_equal(interval.getLowerBound(), [-0.502767], 1e-4, 0.0)
        ott.assert_almost_equal(interval.getUpperBound(), [7.65063], 1e-4, 0.0)
    print("beta=%.5g" % beta)

    spearman = distribution.getSpearmanCorrelation()
    print("spearman=", spearman)
    kendall = distribution.getKendallTau()
    print("kendall=", kendall)
    print("Standard representative=", distribution.getStandardRepresentative())

    ot.Log.Show(ot.Log.TRACE)
    ot.RandomGenerator.SetSeed(1234)
    validation = ott.DistributionValidation(distribution)
    validation.skipEntropy()  # slow
    validation.skipMinimumVolumeLevelSet()  # slow
    validation.run()

# Issue #1643
coll = [ot.Uniform(), ot.Normal()]
distribution = ot.MaximumDistribution(coll)
print("%.5g" % distribution.computePDF([1.1]))
