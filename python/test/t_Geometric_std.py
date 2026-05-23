#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = ot.Geometric(0.7)
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
point = ot.Point(distribution.getDimension(), 3.0)
print("Point= ", repr(point))

# Show PDF and CDF of point
eps = 1e-5
# PDF value
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
print("entropy=%.6f" % distribution.computeEntropy())
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

# computeProbability regression test for discrete distributions
p = ot.Geometric(0.5).computeProbability(ot.Interval(2, 6))
ott.assert_almost_equal(p, 0.484375)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.run()

# Test getSupport does not include k=0 (support starts at 1)
dist_geo = ot.Geometric(0.5)
support = dist_geo.getSupport(ot.Interval(-1.0, 0.5))
ott.assert_almost_equal(support.getSize(), 0)

# Test getSupport returns correct values for a valid interval
support = dist_geo.getSupport(ot.Interval(1.0, 3.0))
ott.assert_almost_equal(support, ot.Sample([[1.0], [2.0], [3.0]]))

# Test computePDFGradient at p=1, k=1 does not return NaN
dist_p1 = ot.Geometric(1.0)
pdf_grad = dist_p1.computePDFGradient([1.0])
assert not any(map(lambda x: x != x, pdf_grad)), "PDF gradient should not be NaN at p=1"
