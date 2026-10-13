#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.WeibullMin(2.0, 1.5, -0.5)
assert distribution.getClassName() == "WeibullMin"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [1.49188], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# Show PDF and CDF of point
eps = 1e-5

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [-0.107263], 1e-5, 0.0)

# PDF value
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -1.081042, 1e-6, 1e-6)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.339242, 1e-6, 1e-6)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.477703, 1e-6, 1e-6)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [-0.0891733, 0.191956, 0.107263], 1e-5, 0.0)

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [-0.254431, -0.0975938, -0.339242], 1e-5, 0.0)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [3.65622], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 1e-6)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [-0.223897], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)

ott.assert_almost_equal(distribution.computeEntropy(), 1.480087, 1e-6, 1e-6)
# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-0.48974], 1e-4, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [3.66304], 1e-4, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.0537054, 1e-5, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-0.327556], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.27485], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [-0.5], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [3.65622], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [-0.223897], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [19.7578], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [1.30549], 1e-5, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [1.22587], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [1.07199], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [4.3904], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [1.50276]), 1e-5, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [2.0, 1.5, -0.5], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "WeibullMin"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# Density at the support lower bound for alpha = 1: 1 / beta
edge = ot.WeibullMin(2.0, 1.0, -0.5)
ott.assert_almost_equal(edge.computePDF(-0.5), 0.5, 1e-12, 0.0)
ott.assert_almost_equal(edge.computeLogPDF(-0.5), -math.log(2.0), 1e-12, 0.0)
