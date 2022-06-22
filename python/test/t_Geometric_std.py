#! /usr/bin/env python

import openturns as ot
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

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", repr(oneSample[0]), " last=", repr(oneSample[1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))

# Define a point
point = ot.Point(distribution.getDimension(), 3.0)
print("Point= ", repr(point))

# Show PDF and CDF of point
eps = 1e-5
# PDF value
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)
# by the finite difference technique from CDF
print("pdf (FD)=%.6f" % (distribution.computeCDF(
    point + ot.Point(1, 0)) - distribution.computeCDF(point + ot.Point(1, -1))))

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", repr(PDFgr))
# by the finite difference technique
PDFgrFD = ot.Point(1)
PDFgrFD[0] = (ot.Geometric(distribution.getP() + eps).computePDF(point) -
              ot.Geometric(distribution.getP() - eps).computePDF(point)) / (2.0 * eps)
print("pdf gradient (FD)=", repr(PDFgrFD))

# derivative of the PDF with regards the parameters of the distribution
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", repr(CDFgr))
# by the finite difference technique
CDFgrFD = ot.Point(1)
CDFgrFD[0] = (ot.Geometric(distribution.getP() + eps).computeCDF(point) -
              ot.Geometric(distribution.getP() - eps).computeCDF(point)) / (2.0 * eps)
print("cdf gradient (FD)=", repr(CDFgrFD))

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
assert p == 0.484375, "wrong discrete interval proba"
