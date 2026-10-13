#! /usr/bin/env python

import math

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.ChiSquare(1.5)
assert distribution.getClassName() == "ChiSquare"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [2.77998], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# Show PDF and CDF of point
eps = 1e-5
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [-0.220728], 1e-5, 0.0)

PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.294304204662, 1e-9, 0.0)

CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.527937109835, 1e-9, 0.0)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real, CF.imag], [0.368925208371, 0.403687932533], 1e-9, 0.0)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [0.0577886], 1e-5, 0.0)

CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [-0.291714], 1e-5, 0.0)

quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [4.9802], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-9, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.0332328], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
ott.assert_almost_equal(distribution.computeEntropy(), 1.374963, 1e-6, 1e-6)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.9802], 1e-5, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.0269275, 1e-5, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.013113], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [6.27581], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.9802], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [0.0332328], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [62.3307], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [1.5], 1e-12, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [3.0]), 1e-12, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [1.5], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Gamma"

# Specific to this distribution
nu = distribution.getNu()
ott.assert_almost_equal(nu, 1.5, 0.0, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [1.73205], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [2.3094], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [11.0], 1e-12, 0.0)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipPDFAtLowerBound()  # the density is infinite at the finite lower bound for nu < 2
validation.run()

# Density at the support lower bound for nu = 2: 1 / 2
edge = ot.ChiSquare(2.0)
ott.assert_almost_equal(edge.computePDF(0.0), 0.5, 1e-12, 0.0)
ott.assert_almost_equal(edge.computeLogPDF(0.0), -math.log(2.0), 1e-12, 0.0)

# Check range upper bound for various nu
for nu in [0.5, 1.0, 2.0, 5.0, 10.0]:
    d = ot.ChiSquare(nu)
    # range upper bound should be > 0.99 quantile
    q = d.computeQuantile(0.99)[0]
    assert d.getRange().getUpperBound()[0] > q, "Range too small for nu=%s" % nu
