#! /usr/bin/env python

import cmath
import math as m
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# Instantiate one distribution object
distribution = ot.WeibullMax(2.0, 1.5, -0.5)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Define a point
point = ot.Point(distribution.getDimension(), -1.0)
print("Point= ", repr(point))

# Show PDF and CDF of point
eps = 1e-5

# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
print("ddf     =", repr(DDF))

# PDF value
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", repr(PDFgr))

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", repr(CDFgr))

# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)

print("entropy=%.6f" % distribution.computeEntropy())
# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95
)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95
)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
(
    interval,
    beta,
) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
print("Unilateral confidence interval (upper tail)=", interval)
print("beta=", ot.Point(1, beta))

mean = distribution.getMean()
print("mean=", repr(mean))
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = distribution.getSkewness()
print("skewness=", repr(skewness))
kurtosis = distribution.getKurtosis()
print("kurtosis=", repr(kurtosis))
covariance = distribution.getCovariance()
print("covariance=", repr(covariance))
parameters = distribution.getParametersCollection()
print("parameters=", repr(parameters))
print("Standard representative=", distribution.getStandardRepresentative())

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# --- WeibullMax: characteristic function matches Monte Carlo ---
ot.RandomGenerator.SetSeed(11)
weibull = ot.WeibullMax(2.0, 1.0, 0.0)
mcSample = weibull.getSample(500000)
u = 0.5
mcCF = sum(cmath.exp(1j * u * x[0]) for x in mcSample) / mcSample.getSize()
gotCF = weibull.computeCharacteristicFunction(u)
ott.assert_almost_equal([gotCF.real], [mcCF.real], 5e-3, 0.0)
ott.assert_almost_equal([gotCF.imag], [mcCF.imag], 5e-3, 0.0)

# Roughness: analytic value alpha / (2 * beta) * exp(lgamma(2 - 1/alpha) + (1/alpha - 1) * log(2))
ott.assert_almost_equal(
    ot.WeibullMax(1.0, 1.5).getRoughness(),
    1.5 / 2.0 * m.exp(m.lgamma(2.0 - 1.0 / 1.5) + (1.0 / 1.5 - 1.0) * m.log(2.0)),
)
# The L2 norm of the PDF diverges for alpha <= 0.5
with ott.assert_raises(Exception):
    ot.WeibullMax(1.0, 0.5).getRoughness()
