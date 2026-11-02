#! /usr/bin/env python

import math as m

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.Chi(1.5)
assert distribution.getClassName() == "Chi"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [1.66733], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# Show PDF and CDF of point
eps = 1e-5
DDF = distribution.computeDDF(point)
ott.assert_almost_equal(DDF, [-0.294304], 1e-5, 0.0)

PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.588608409324, 1e-9, 0.0)

CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.527937109835, 1e-9, 0.0)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real, CF.imag], [0.433495931529, 0.694719223625], 1e-9, 0.0)
PDFgr = distribution.computePDFGradient(point)
ott.assert_almost_equal(PDFgr, [0.115577], 1e-5, 0.0)

CDFgr = distribution.computeCDFGradient(point)
ott.assert_almost_equal(CDFgr, [-0.291714], 1e-5, 0.0)

quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [2.23164], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-9, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.182299], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
ott.assert_almost_equal(distribution.computeEntropy(), 0.878173, 1e-6, 1e-6)
# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.0147509], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [2.24137], 1e-5, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-5, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.117852, 1e-5, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [0.114512], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [2.50516], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [0.0], 0.0, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [2.23164], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [0.182299], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [7.89498], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-5, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [1.04605], 1e-5, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [0.40578]), 1e-5, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [1.5], 0.0, 0.0)
assert distribution.getStandardRepresentative().getImplementation().getClassName() == "Chi"

# Specific to this distribution
nu = distribution.getNu()
ott.assert_almost_equal(nu, 1.5, 0.0, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [0.637009], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [0.762585], 1e-5, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [3.42425], 1e-5, 0.0)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# --- Chi family: gradients valid for small shapes ---
assert m.isfinite(ot.Chi(0.01).computeCDFGradient([0.5])[0])
assert m.isfinite(ot.ChiSquare(0.01).computeCDFGradient([0.5])[0])
assert m.isfinite(ot.NonCentralChiSquare(0.01, 0.0).computePDFGradient([0.5])[0])
with ott.assert_raises(Exception):
    ot.ChiFactory().build(ot.Sample([[1.0], [-2.0], [3.0]]))
with ott.assert_raises(Exception):
    ot.ChiSquareFactory().build(ot.Sample([[1.0], [-2.0], [3.0]]))

# Density at the support lower bound for nu = 1: sqrt(2 / pi)
edge = ot.Chi(1.0)
ott.assert_almost_equal(edge.computePDF(0.0), m.sqrt(2.0 / m.pi), 1e-12, 0.0)
ott.assert_almost_equal(edge.computeLogPDF(0.0), m.log(m.sqrt(2.0 / m.pi)), 1e-12, 0.0)
# Roughness: analytic value 2^(1-nu) * Gamma(nu-1/2) / Gamma(nu/2)^2 for nu > 0.5
ott.assert_almost_equal(
    ot.Chi(3.0).getRoughness(),
    m.exp((1.0 - 3.0) * m.log(2.0) + m.lgamma(2.5) - 2.0 * m.lgamma(1.5)),
)
# The L2 norm of the PDF diverges for nu <= 0.5
with ott.assert_raises(Exception):
    ot.Chi(0.5).getRoughness()
