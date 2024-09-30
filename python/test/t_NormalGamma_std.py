#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.NormalGamma(1.0, 2.0, 3.0, 4.0)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Define a point
point = [1.5] * distribution.getDimension()
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf=%.6f" % PDF)
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)
Survival = distribution.computeSurvivalFunction(point)
print("survival=%.6f" % Survival)
InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
print("Inverse survival=", InverseSurvival)
print(
    "Survival(inverse survival)=%.6f"
    % distribution.computeSurvivalFunction(InverseSurvival)
)
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", PDFgr)

CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", CDFgr)

quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
print("entropy=%.6f" % distribution.computeEntropy())
# Confidence regions
(
    probability,
    threshold,
) = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
ott.assert_almost_equal(probability.getLowerBound(), [0.03904, -1.80326], 1e-4, 0.0)
ott.assert_almost_equal(probability.getUpperBound(), [2.00954, 3.80326], 1e-4, 0.0)
print("threshold=%.6f" % threshold)
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=%.6f" % beta)
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
print("Bilateral confidence interval=", interval)
print("beta=%.6f" % beta)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=%.6f" % beta)
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=%.6f" % beta)
mean = distribution.getMean()
print("mean=", mean)
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", standardDeviation)
skewness = distribution.getSkewness()
print("skewness=", skewness)
kurtosis = distribution.getKurtosis()
print("kurtosis=", kurtosis)
covariance = distribution.getCovariance()
print("covariance=", covariance)
correlation = distribution.getCorrelation()
print("correlation=", correlation)
# These computations take too much time for a test
# CovarianceMatrix spearman = distribution.getSpearmanCorrelation()
# print("spearman=", spearman)
# CovarianceMatrix kendall = distribution.getKendallTau()
# print("kendall=", kendall)

parameters = distribution.getParametersCollection()
print("parameters=", parameters)
print("Standard representative=", distribution.getStandardRepresentative())

ot.Log.Show(ot.Log.TRACE)
checker = ott.DistributionChecker(distribution)
checker.skipMoments()  # slow
checker.skipCorrelation()  # slow
checker.skipParameters()
checker.run()
