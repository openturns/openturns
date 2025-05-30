#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

left = ot.Uniform(-1.0, 2.0)
right = ot.Normal(1.0, 2.0)
distribution = ot.ProductDistribution(left, right)
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical =", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous =", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
print("covariance=", oneSample.computeCovariance())

# Define a point
point = [2.5] * distribution.getDimension()
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
print("ddf      =", DDF)
PDF = distribution.computePDF(point)
print("pdf      =%.6g" % PDF)
CDF = distribution.computeCDF(point)
print("cdf      =%.6g" % CDF)
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient      =", PDFgr)
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient      =", CDFgr)
quantile = distribution.computeQuantile(0.95)
print("quantile     =", quantile)
print("cdf(quantile)=%.6g" % distribution.computeCDF(quantile))
print("entropy=%.6g" % distribution.computeEntropy())
print(
    "entropy (MC)=%.6g"
    % -distribution.computeLogPDF(distribution.getSample(10000)).computeMean()[0]
)
mean = distribution.getMean()
print("mean      =", mean)
standardDeviation = distribution.getStandardDeviation()
print("standard deviation      =", standardDeviation)
skewness = distribution.getSkewness()
print("skewness      =", skewness)
kurtosis = distribution.getKurtosis()
print("kurtosis      =", kurtosis)
covariance = distribution.getCovariance()
print("covariance      =", covariance)
parameters = distribution.getParametersCollection()
print("parameters      =", parameters)
print("Standard representative=", distribution.getStandardRepresentative())

# Specific to this distribution
print("left=", distribution.getLeft())
print("right=", distribution.getRight())

# Probability of an interval
interval = ot.Interval(-1.0, 2.0)
ott.assert_almost_equal(distribution.computeProbability(interval), 0.641, 0.01, 0.0)

# For ticket 957
distribution = ot.Uniform() * ot.Uniform() * ot.Uniform()
print("distribution=", distribution)
print("mean=", distribution.getMean())
print("standard deviation=", distribution.getStandardDeviation())
