#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


ot.ResourceMap.SetAsUnsignedInteger("MeixnerDistribution-CDFDiscretization", 100)
ot.ResourceMap.SetAsScalar("MeixnerDistribution-MaximumAbsoluteError", 1.0e-6)
ot.ResourceMap.SetAsScalar("MeixnerDistribution-MaximumRelativeError", 1.0e-6)
ot.ResourceMap.SetAsScalar("MeixnerDistribution-MaximumConstraintError", 1.0e-6)
ot.ResourceMap.SetAsScalar("MeixnerDistribution-MaximumObjectiveError", 1.0e-6)

# Instantiate one distribution object
distribution = ot.MeixnerDistribution(1.5, 0.5, 2.5, -0.5)
assert distribution.getClassName() == "MeixnerDistribution"

# Is this distribution elliptical ?
assert not distribution.isElliptical()

# Is this distribution continuous ?
assert distribution.isContinuous()

# Test for realization of distribution
oneRealization = distribution.getRealization()
ott.assert_almost_equal(oneRealization, [2.06954], 1e-5, 0.0)

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
ott.assert_almost_equal(point, [1.0], 0.0, 0.0)

# Show PDF and CDF of point
LPDF = distribution.computeLogPDF(point)
ott.assert_almost_equal(LPDF, -1.517348, 1e-6, 0.0)
PDF = distribution.computePDF(point)
ott.assert_almost_equal(PDF, 0.21929265894623332, 1e-12, 0.0)

ott.assert_almost_equal(
    distribution.computePDFGradient(point),
    [-0.0855978, 0.0793061, -0.0152517, 0.0605973],
    1e-6,
    0.0,
)
CDF = distribution.computeCDF(point)
ott.assert_almost_equal(CDF, 0.640774, 1e-6, 0.0)
ott.assert_almost_equal(
    distribution.computeCDFGradient(point),
    [-0.219293, -0.4305, -0.1123, -0.219293],
    1e-4,
    0.0,
)
CCDF = distribution.computeComplementaryCDF(point)
ott.assert_almost_equal(CCDF, 0.359226, 1e-6, 0.0)
CF = distribution.computeCharacteristicFunction(point[0])
ott.assert_almost_equal([CF.real], [0.245823021921], 1e-10, 0.0)
ott.assert_almost_equal([CF.imag], [0.0770940595934], 1e-10, 0.0)
LCF = distribution.computeLogCharacteristicFunction(point[0])
ott.assert_almost_equal([LCF.real], [-1.35623659307], 1e-10, 0.0)
ott.assert_almost_equal([LCF.imag], [0.303901355916], 1e-10, 0.0)
quantile = distribution.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [3.39477], 1e-5, 0.0)
ott.assert_almost_equal(distribution.computeCDF(quantile), 0.95, 1e-6, 0.0)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [-2.27095], 1e-5, 0.0)
ott.assert_almost_equal(
    distribution.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 0.0
)
ott.assert_almost_equal(distribution.computeEntropy(), 1.961981, 1e-6, 0.0)

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-2.90577], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [3.94843], 1e-5, 0.0)
ott.assert_almost_equal(threshold, 0.95, 1e-12, 0.0)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
ott.assert_almost_equal(beta, 0.030032538327191351, 1e-12, 0.0)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
ott.assert_almost_equal(interval.getLowerBound(), [-2.80555], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [4.06095], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
ott.assert_almost_equal(interval.getLowerBound(), [-18.5819], 1e-4, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [3.39477], 1e-5, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
ott.assert_almost_equal(interval.getLowerBound(), [-2.27095], 1e-5, 0.0)
ott.assert_almost_equal(interval.getUpperBound(), [24.6896], 1e-4, 0.0)
ott.assert_almost_equal(beta, 0.95, 1e-12, 0.0)

mean = distribution.getMean()
ott.assert_almost_equal(mean, [0.457532], 1e-6, 0.0)
covariance = distribution.getCovariance()
ott.assert_almost_equal(covariance, ot.CovarianceMatrix(1, [2.99587]), 1e-5, 0.0)
parameters = distribution.getParametersCollection()
assert len(parameters) == 1
ott.assert_almost_equal(parameters[0], [1.5, 0.5, 2.5, -0.5], 0.0, 0.0)
standard = distribution.getStandardRepresentative()
ott.assert_almost_equal(standard.getParameter(), [1.0, 0.5, 2.5, 0.0], 0.0, 0.0)

# Specific to this distribution
alpha = distribution.getAlpha()
ott.assert_almost_equal(alpha, 0.5, 1e-12, 0.0)
beta = distribution.getBeta()
ott.assert_almost_equal(beta, 1.5, 1e-12, 0.0)
delta = distribution.getDelta()
ott.assert_almost_equal(delta, 2.5, 1e-12, 0.0)
gamma = distribution.getGamma()
ott.assert_almost_equal(gamma, -0.5, 1e-12, 0.0)
standardDeviation = distribution.getStandardDeviation()
ott.assert_almost_equal(standardDeviation, [1.73086], 1e-5, 0.0)
skewness = distribution.getSkewness()
ott.assert_almost_equal(skewness, [0.221285], 1e-6, 0.0)
kurtosis = distribution.getKurtosis()
ott.assert_almost_equal(kurtosis, [3.44897], 1e-5, 0.0)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()
