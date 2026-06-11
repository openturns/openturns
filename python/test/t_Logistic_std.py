#! /usr/bin/env python

import math
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Logistic(-0.5, 1.5)
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
point = ot.Point(distribution.getDimension(), 1.0)
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
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)
CF = distribution.computeCharacteristicFunction(point[0])
print("characteristic function= (%.12g%+.12gj)" % (CF.real, CF.imag))
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

# Test extreme values in asymptotic expansions
distribution0 = ot.Logistic(0.0, 1.0)
for z in [100.0, 1000.0, -100.0, -1000.0]:
    point = [z]
    ddf = distribution0.computeDDF(point)
    assert ddf[0] == ddf[0]
    assert abs(ddf[0]) < 1.0
    pdfGrad = distribution0.computePDFGradient(point)
    assert pdfGrad[0] == pdfGrad[0]
    assert pdfGrad[1] == pdfGrad[1]
    assert abs(pdfGrad[0]) < 1.0
    assert abs(pdfGrad[1]) < 1.0
    cdfGrad = distribution0.computeCDFGradient(point)
    assert cdfGrad[0] == cdfGrad[0]
    assert cdfGrad[1] == cdfGrad[1]
    assert abs(cdfGrad[0]) < 1.0
    assert abs(cdfGrad[1]) < 1.0

for x in [100.0, 500.0, 1000.0, -100.0, -500.0, -1000.0]:
    cf = distribution0.computeCharacteristicFunction(x)
    assert cf.real == cf.real
    assert cf.imag == cf.imag
    logcf = distribution0.computeLogCharacteristicFunction(x)
    assert logcf.real == logcf.real
    assert logcf.imag == logcf.imag
    assert logcf.real < 0.0

for x in [1.0, 10.0, 50.0]:
    cf = distribution0.computeCharacteristicFunction(x)
    logcf = distribution0.computeLogCharacteristicFunction(x)
    ott.assert_almost_equal(abs(cf), math.exp(logcf.real), 1e-12, 0.0)
