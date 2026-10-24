#! /usr/bin/env python

import math as m

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Burr(1.5, 4.5)
assert distribution.getClassName() == "Burr"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [0.393842], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.5)
ott.assert_almost_equal(point, [1.5], 0.0, 0.0)

# Show PDF and CDF of point
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.0267008010312, 1e-9, 0.0)

CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.990836694178, 1e-9, 0.0)

CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [0.0108262, 0.00955539], 1e-5, 0.0)

quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [0.963592], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-9, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.0508413], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
ott.assert_almost_equal(distribution.computeEntropy(), -0.031868, 1e-6, 1e-6)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.000634766])
ott.assert_almost_equal(interval.getUpperBound(), [0.96401], 1e-3, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.169994, 1e-5, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.0316926], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.1727], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [0.963592], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [0.0508413], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [118.536], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.37922], 1e-5, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [0.310221], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [2.16221], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [13.2624], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.0962368]), 1e-5, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [1.5, 4.5], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Burr"

# computeProba test with bound far away
p = distribution.computeProbability(
    ot.Interval(-ot.SpecFunc.Infinity, ot.SpecFunc.Infinity)
)
ott.assert_almost_equal(p, 1.0)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# --- Burr: nonexistent moments throw, tail log-density is finite ---
with ott.assert_raises(Exception):
    ot.Burr(2.0, 0.5).getMean()
ott.assert_almost_equal(ot.Burr(2.0, 2.0).getMean()[0], m.pi / 4.0, 1e-12, 0.0)
assert m.isfinite(ot.Burr(2.0, 2.0).computeLogPDF([1.0e150]))

# Density at the support lower bound for c = 1: c * k
edge = ot.Burr(1.0, 2.0)
ott.assert_almost_equal(edge.computePDF(0.0), 2.0, 1e-12, 0.0)
ott.assert_almost_equal(edge.computeLogPDF(0.0), m.log(2.0), 1e-12, 0.0)

# Extreme parameters: log(c * k) underflows while log(c) + log(k) stays finite
extreme = ot.Burr(1.0e-200, 1.0e-200)
assert m.isinf(extreme.computeLogPDF(0.0))
assert m.isinf(extreme.computePDF(0.0))
