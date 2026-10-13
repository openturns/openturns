#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.Pareto(7.5, 5.0, -7.0)

assert distribution.getClassName() == "Pareto"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [2.14939], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)
ott.assert_almost_equal(distribution.getRange().getLowerBound(), [0.5], 1e-5, 0.0)

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [-0.339467], 1e-5, 0.0)

PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.452622771263, 1e-9, 0.0)

CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.275803565979, 1e-9, 0.0)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [0.301749, 0.0613129, 0.339467], 1e-5, 0.0)

CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [-0.482798, 0.0467386, -0.452623], 1e-5, 0.0)

quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [6.65423], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-9, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.577336], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 0.0
)
ott.assert_almost_equal(distribution.computeEntropy(), 1.605465, 1e-6, 0.0)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.5], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [6.65423], 1e-5, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.0183093, 1e-5, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.538073], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [8.68459], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [0.5], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [6.65423], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [0.577336], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [10126.8], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [2.375], 1e-5, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [2.42061], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [4.64758], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [73.8], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [5.85938]), 1e-5, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [7.5, 5.0, -7.0], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Pareto"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipPDFAtLowerBound()  # the density is alpha / beta at the finite lower bound
validation.run()

# Density at the finite lower bound gamma + beta: alpha / beta
ott.assert_almost_equal(distribution.computePDF(0.5), 5.0 / 7.5, 1e-12, 0.0)
ott.assert_almost_equal(distribution.computeLogPDF(0.5), math.log(5.0 / 7.5), 1e-12, 0.0)
