#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.LogUniform(-0.5, 1.5)
assert distribution.getClassName() == "LogUniform"

# Get mean and covariance
ott.assert_almost_equal(distribution.getMean(), [1.93758], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.getCovariance(), ot.CovarianceMatrix(1, [1.1752]), 1e-4, 0.0
)

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [2.1377483735183858], 1e-12, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [-0.5], 1e-12, 0.0)

# PDF value
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -0.693147, 1e-6, 0.0)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.5, 1e-12, 0.0)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.25, 1e-12, 0.0)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.75, 1e-12, 0.0)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real], [-0.089619617796325171], 1e-12, 0.0)
ott.assert_almost_equal([CF.imag], [0.53192379992471173], 1e-12, 0.0)
logCF = distribution.computeLogCharacteristicFunction(point[0])
ott.assert_almost_equal([logCF.real], [-0.61725964213876217], 1e-12, 0.0)
ott.assert_almost_equal([logCF.imag], [1.7377108115955473], 1e-12, 0.0)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [0.25, -0.25], 1e-12, 0.0)

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [-0.375, -0.125], 1e-12, 0.0)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [4.0552], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.67032], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 0.0
)
ott.assert_almost_equal(distribution.computeEntropy(), 1.193147, 1e-6, 0.0)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.606531], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.0552], 1e-5, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-12, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.12329848197059994, 1e-12, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.637628], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.26311], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [0.606531], 1e-6, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.0552], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [0.67032], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.48169], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [1.93758], 1e-5, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [1.08407], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [0.67539], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [0.623015], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [1.1752]), 1e-4, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [-0.5, 1.5], 0.0, 0.0)
standard = distribution.getStandardRepresentative()
assert standard.getImplementation().getClassName() == "LogUniform"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipCharacteristicFunction()  # FIXME
validation.run()
