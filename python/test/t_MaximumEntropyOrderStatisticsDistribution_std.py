#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


# Instanciate one distribution object
distribution = ot.MaximumEntropyOrderStatisticsDistribution(
    [ot.Trapezoidal(-2.0, -1.1, -1.0, 1.0),
     ot.LogUniform(1.0, 1.2),
     ot.Triangular(3.0, 4.5, 5.0),
     ot.Beta(2.5, 6.0, 4.7, 5.2)])

dim = distribution.getDimension()
print("Distribution ", distribution)

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
point = ot.NumericalPoint([0.0, 3.2, 4.2, 5.0])
print("Point= ", point)

# Show PDF and CDF of point
#eps = 1e-5
# derivative of PDF with regards its arguments
DDF = distribution.computeDDF(point)
print("ddf     =", repr(DDF))
# PDF value
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.6f" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)
condPDF = distribution.computeConditionalPDF(4., [0., 2.])
print("condPDF     =%.6f" % condPDF)

# derivative of the PDF with regards the parameters of the distribution
CDF = distribution.computeCDF(point)
print("cdf=%.6f" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=%.6f" % CCDF)
condCDF = distribution.computeConditionalCDF(4., [0., 2.])
print("condCDF     =%.6f" % condCDF)
#PDFgr = distribution.computePDFGradient(point)
# print "pdf gradient     =", repr(PDFgr)
# quantile
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
condQuantile = distribution.computeConditionalQuantile(0.4, [0., 2.])
print("condQuantile     =%.6f" % condQuantile)
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

# Extract the marginals
for i in range(dim):
    margin = distribution.getMarginal(i)
    print("margin=", repr(margin))
    print("margin PDF=%.6f" % margin.computePDF(point[i]))
    print("margin CDF=%.6f" % margin.computeCDF(point[i]))
    print("margin quantile=", repr(margin.computeQuantile(0.5)))
    print("margin realization=", repr(margin.getRealization()))
    print("margin range=", repr(margin.getRange()))

# Extract a 2-D marginal
indices = [0, 1]
print("indices=", repr(indices))
margins = distribution.getMarginal(indices)
print("margins=", repr(margins))
print("margins PDF=%.6f" % margins.computePDF([point[i] for i in indices]))
print("margins CDF=%.6f" % margins.computeCDF([point[i] for i in indices]))
quantile = ot.NumericalPoint(margins.computeQuantile(0.8))
print("margins quantile=", repr(quantile))
print("margins CDF(qantile)=%.6f" % margins.computeCDF(quantile))
print("margins realization=", repr(margins.getRealization()))
