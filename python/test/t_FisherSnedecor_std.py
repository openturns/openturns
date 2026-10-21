#! /usr/bin/env python

import os
import math as m
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.FisherSnedecor(5.5, 10.5)
assert distribution.getClassName() == "FisherSnedecor"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [1.85322], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)

# PDF value
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -0.659362, 1e-6, 1e-6)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.517181, 1e-6, 1e-6)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.531405, 1e-6, 1e-6)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.468595, 1e-6, 1e-6)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real, CF.imag], [0.385738, 0.612601], 1e-6, 1e-6)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [0.0333323, 0.00890862], 1e-5, 0.0)

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [-0.0101307, 0.00377782], 1e-5, 0.0)

# derivative of the logPDF with regards the parameters of the distribution
logPDFgr = distribution.computeLogPDFGradient(point)
ott.assert_almost_equal(logPDFgr, [0.0644499, 0.0172253], 1e-5, 0.0)

# quantile
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [3.2027], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.950000, 1e-6, 1e-6)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.230549], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
ott.assert_almost_equal(distribution.computeEntropy(), 1.100253, 1e-6, 1e-6)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.03856], 0.0, 1e-4)
ott.assert_almost_equal(interval.getUpperBound(), [3.21723], 0.0, 1e-4)
ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.0429883, 1e-5, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.168551], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.04601], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [3.2027], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [0.230549], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeProbability(interval), 0.95, 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [1.23529], 1e-5, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [1.09323], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [3.56105], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [42.039], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [1.19515]), 1e-5, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [5.5, 10.5], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "FisherSnedecor"

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# --- FisherSnedecor: persistence round-trip ---
studyFile = "myStudyBugFixFisher.xml"
study = ot.Study(studyFile)
study.add("fisher", ot.FisherSnedecor(4.0, 5.0))
study.save()
study2 = ot.Study(studyFile)
study2.load()
r = ot.FisherSnedecor(1.0, 1.0)
study2.fillObject("fisher", r)
ott.assert_almost_equal(r.getParameter(), [4.0, 5.0], 0.0, 0.0)
ott.assert_almost_equal(r.computePDF([1.0]), ot.FisherSnedecor(4.0, 5.0).computePDF([1.0]), 1e-12, 0.0)
os.remove(studyFile)

# Density at the support lower bound for d1 = 2: 1
edge = ot.FisherSnedecor(2.0, 5.0)
ott.assert_almost_equal(edge.computePDF(0.0), 1.0, 1e-12, 0.0)
ott.assert_almost_equal(edge.computeLogPDF(0.0), 0.0, 1e-12, 1e-12)
# Roughness: analytic value a * exp(lbeta(d1 - 1, d2 + 1) - 2 * lbeta(d1 / 2, d2 / 2)) with a = d1 / d2
d1, d2 = 5.0, 3.0
ott.assert_almost_equal(
    ot.FisherSnedecor(d1, d2).getRoughness(),
    d1
    / d2
    * m.exp(
        m.lgamma(d1 - 1.0)
        + m.lgamma(d2 + 1.0)
        - m.lgamma(d1 + d2)
        - 2.0
        * (
            m.lgamma(0.5 * d1)
            + m.lgamma(0.5 * d2)
            - m.lgamma(0.5 * (d1 + d2))
        )
    ),
)
# The L2 norm of the PDF diverges for d1 <= 1
with ott.assert_raises(Exception):
    ot.FisherSnedecor(1.0, 3.0).getRoughness()
