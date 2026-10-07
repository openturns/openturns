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

# Exercise all the support sign patterns of computePDF
patterns = [
    (ot.Uniform(1.0, 2.0), ot.Uniform(3.0, 4.0), 5.0),  # Q1
    (ot.Uniform(-2.0, -1.0), ot.Uniform(3.0, 4.0), -5.0),  # Q2
    (ot.Uniform(-2.0, -1.0), ot.Uniform(-4.0, -3.0), 5.0),  # Q3
    (ot.Uniform(1.0, 2.0), ot.Uniform(-4.0, -3.0), -5.0),  # Q4
    (ot.Uniform(-1.0, 2.0), ot.Uniform(3.0, 4.0), 2.0),  # Q1 U Q2
    (ot.Uniform(-1.0, 2.0), ot.Uniform(-4.0, -3.0), -2.0),  # Q3 U Q4
    (ot.Uniform(1.0, 2.0), ot.Normal(), 1.0),  # Q1 U Q4
    (ot.Uniform(-2.0, -1.0), ot.Normal(), -1.0),  # Q2 U Q3
]
for left, right, x in patterns:
    pattern_dist = ot.ProductDistribution(left, right)
    pdf = pattern_dist.computePDF([x])
    print("pdf(%g)=%.6g" % (x, pdf))
    assert pdf > 0.0
    ott.assert_almost_equal(
        pattern_dist.computeCDF(pattern_dist.getRange().getUpperBound()), 1.0
    )

# Error cases
distribution = ot.ProductDistribution(
    ot.Uniform(-1.0, 2.0), ot.Normal(1.0, 2.0)
)
with ott.assert_raises(TypeError):
    distribution.computePDF([1.0, 2.0])
with ott.assert_raises(TypeError):
    distribution.computeCDF([1.0, 2.0])
with ott.assert_raises(TypeError):
    distribution.setParameter([1.0])
with ott.assert_raises(TypeError):
    distribution.setLeft(ot.Normal(2))
with ott.assert_raises(TypeError):
    distribution.setLeft(ot.Poisson(2.0))
with ott.assert_raises(TypeError):
    distribution.setRight(ot.Normal(2))

validation = ott.DistributionValidation(distribution)
validation.setMomentsSamplingSize(100000)
validation.setEntropySamplingSize(10000)
validation.setDomainSamplingSize(10000)
validation.setFittingSamplingSize(1000)
validation.run()
