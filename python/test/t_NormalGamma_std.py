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

# Test invalid constructor params
with ott.assert_raises(TypeError):
    ot.NormalGamma(1.0, -1.0, 3.0, 4.0)
with ott.assert_raises(TypeError):
    ot.NormalGamma(1.0, 2.0, -1.0, 4.0)
with ott.assert_raises(TypeError):
    ot.NormalGamma(1.0, 2.0, 3.0, -1.0)

# Test covariance not defined for alpha <= 1
dist_low_alpha = ot.NormalGamma(1.0, 2.0, 0.5, 4.0)
with ott.assert_raises(RuntimeError):
    dist_low_alpha.getCovariance()

# Test PDF/LogPDF with wrong dimension
with ott.assert_raises(TypeError):
    distribution.computePDF([1.0])
with ott.assert_raises(TypeError):
    distribution.computeLogPDF([1.0])

# Test PDF outside range
print("logPDF outside range (y<=a)=", distribution.computeLogPDF([-0.1, 1.0]))
print("logPDF outside range (y>=b)=", distribution.computeLogPDF([100.0, 1.0]))

# Test CDF/survival edge cases
ott.assert_almost_equal(distribution.computeCDF([-0.1, 1.0]), 0.0), "CDF y<=a"
ott.assert_almost_equal(distribution.computeCDF([1.0, -100.0]), 0.0), "CDF x<<0"
ott.assert_almost_equal(distribution.computeCDF([1.0, 100.0]), 0.7618966944464557), "CDF x>>0"
ott.assert_almost_equal(distribution.computeCDF([100.0, 100.0]), 1.0), "CDF x>>0,y>>0"
ott.assert_almost_equal(distribution.computeSurvivalFunction([100.0, 1.0]), 0.0), "Survival y>b="
ott.assert_almost_equal(distribution.computeSurvivalFunction([1.0, 100.0]), 0.0), "Survival x>>0"
ott.assert_almost_equal(distribution.computeSurvivalFunction([1.0, -100.0]), 0.238103305554), "Survival x<<0"
ott.assert_almost_equal(distribution.computeSurvivalFunction([-0.1, -100.0]), 1.0), "Survival x<<0,y<a"

# Test computeProbability
interval = ot.Interval([0.5, 0.0], [1.5, 2.0])
ott.assert_almost_equal(distribution.computeProbability(interval), 0.3028653231821), "Probability(0.5<=y<=1.5, 0<=x<=2)"
empty_interval = ot.Interval([0.5, 2.0], [0.4, 3.0])
ott.assert_almost_equal(distribution.computeProbability(empty_interval), 0.0), "Probability empty"
full_interval = distribution.getRange()
ott.assert_almost_equal(distribution.computeProbability(full_interval), 1.0), "Probability full"

# Test getMarginal with invalid index
with ott.assert_raises(TypeError):
    distribution.getMarginal(2)

# Test setParameter wrong size
with ott.assert_raises(TypeError):
    distribution.setParameter([1.0, 2.0])

# Test setters with same value (should be no-op)
dist2 = ot.NormalGamma(1.0, 2.0, 3.0, 4.0)
dist2.setMu(1.0)
dist2.setKappa(2.0)
dist2.setAlpha(3.0)
dist2.setBeta(4.0)
print("Setters with same value: mu=", dist2.getMu(), "kappa=", dist2.getKappa(), "alpha=", dist2.getAlpha(), "beta=", dist2.getBeta())

# Test setters with invalid values
with ott.assert_raises(TypeError):
    dist2.setKappa(-1.0)
with ott.assert_raises(TypeError):
    dist2.setAlpha(-1.0)
with ott.assert_raises(TypeError):
    dist2.setBeta(-1.0)

# Test setters with valid new values
dist2.setMu(2.0)
dist2.setKappa(3.0)
dist2.setAlpha(4.0)
dist2.setBeta(5.0)
print("Setters new values: mu=", dist2.getMu(), "kappa=", dist2.getKappa(), "alpha=", dist2.getAlpha(), "beta=", dist2.getBeta())

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.skipParameters()
validation.run()
