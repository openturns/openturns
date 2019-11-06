#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import time

ticksCollection = [[0.0, 1.0, 3.0, 6.0], [1.0, 2.0, 3.0]]

kind = [ot.MixedHistogramUserDefined.CONTINUOUS, ot.MixedHistogramUserDefined.DISCRETE]
probabilityTable = [0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45]
distribution = ot.MixedHistogramUserDefined(ticksCollection, kind, probabilityTable)
distribution.setDescription(["A", "B"])
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Get mean and covariance
print("Mean= ", repr(distribution.getMean()))
print("Covariance= ", repr(distribution.getCovariance()))

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", repr(
    oneSample[0]), " last=", repr(oneSample[size - 1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))
    
# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
print("Point= ", repr(point))

# Show PDF and CDF of point
eps = 1e-5

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
# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
mean = distribution.getMean()
print("mean=", repr(mean))
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", repr(standardDeviation))
skewness = distribution.getSkewness()
print("skewness=", repr(skewness))
kurtosis = distribution.getKurtosis()
print("kurtosis=", repr(kurtosis))
for i in range(6):
    print("standard moment n=", i, " value=",
          distribution.getStandardMoment(i))
print("Standard representative=", distribution.getStandardRepresentative())


# higher dimension
dim = 8
ticksCollection = [[0.0, 1.0, 3.0, 6.0]]*dim
kind = [ot.MixedHistogramUserDefined.DISCRETE] * dim
kind[dim-2] = ot.MixedHistogramUserDefined.CONTINUOUS
probabilityTable = ot.RandomGenerator.Generate(4**(dim-1)*3)
x = [3.0]*dim
x[dim-2] = 5.0
bench = 0
distribution = ot.MixedHistogramUserDefined(ticksCollection, kind, probabilityTable)
t0 = time.time()
print('pdf=%.6g' % distribution.computePDF(x))
t1 = time.time()
#print('pdf t=%.6g' % (t1-t0))
t0 = time.time()
print('cdf=%.6g' % distribution.computeCDF(x))
t1 = time.time()
#print('cdf t=%.6g' % (t1-t0))

# test default ctor
ot.MixedHistogramUserDefined().getSample(5)
