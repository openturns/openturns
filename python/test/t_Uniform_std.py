#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Uniform(-0.5, 1.5)
assert distribution.getClassName() == "Uniform"

# Get mean and covariance
mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.5], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.333333]), 1e-5, 0.0)

# Is this distribution elliptical ?
assert distribution.isElliptical()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [0.759753], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# Show PDF and CDF of point
eps = 1e-5

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [0.0], 0.0, 0.0)

# PDF value
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -0.693147, 1e-6, 1e-6)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.500000, 1e-6, 1e-6)

# Density at the support edges, see #2928 (mixture junction values)
edge = ot.Uniform(2.0, 4.0)
ott.assert_almost_equal(edge.computePDF(2.0), 0.5, 0.0, 0.0)
ott.assert_almost_equal(edge.computePDF(4.0), 0.5, 0.0, 0.0)
ott.assert_almost_equal(edge.computeLogPDF(2.0), math.log(0.5), 1e-12, 0.0)
ott.assert_almost_equal(edge.computePDFGradient([2.0]), [0.25, -0.25], 1e-12, 0.0)
ott.assert_almost_equal(edge.computePDFGradient([4.0]), [0.25, -0.25], 1e-12, 0.0)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.750000, 1e-6, 1e-6)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.250000, 1e-6, 1e-6)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real, CF.imag], [0.738460262604, 0.403422680111], 1e-9, 0.0)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [0.25, -0.25], 1e-5, 0.0)

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [-0.125, -0.375], 1e-5, 0.0)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [1.4], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 1e-6)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [-0.4], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
ott.assert_almost_equal(distribution.computeEntropy(), 0.693147, 1e-6, 1e-6)
# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-0.45], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.45], 1e-5, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-9, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.5, 1e-9, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-0.45], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.45], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-9, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [-0.5], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.4], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-9, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [-0.4], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [1.5], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-9, 0.0)
mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.5], 1e-5, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [0.57735], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [0.0], 0.0, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [1.8], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.333333]), 1e-5, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [-0.5, 1.5], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Uniform"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipPDFAtLowerBound()  # the density is 1/(b-a) at the finite lower bound, see #2928
validation.run()
