#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Instanciate one distribution object
distribution = ot.Frechet(6.0, 1.5, -1.0)
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
print("covariance=", oneSample.computeCovariance())
size = 100
for i in range(2):
    print("Kolmogorov test for the generator, sample size=", size, " is ", ot.FittingTest.Kolmogorov(
        distribution.getSample(size), distribution).getBinaryQualityMeasure())
    size *= 10

# Define a point
point = ot.NumericalPoint(distribution.getDimension(), 1.0)
print("Point= ", point)

# Show PDF and CDF of point
eps = 1e-5
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf= %.12g" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6f" % PDF)
print("pdf (FD)=%.6f" % ((distribution.computeCDF(
    point + [eps]) - distribution.computeCDF(point + [-eps])) / (2.0 * eps)))
CDF = distribution.computeCDF(point)
print("cdf= %.12g" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf= %.12g" % CCDF)
Survival = distribution.computeSurvivalFunction(point)
print("survival= %.12g" % Survival)
CF = distribution.computeCharacteristicFunction(point[0])
print("characteristic function=(%.6g, %.6g)" % (CF.real, CF.imag))
LCF = distribution.computeLogCharacteristicFunction(point[0])
print("log characteristic function=(%.6g, %.6g)" % (LCF.real, LCF.imag))
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", PDFgr)
PDFgrFD = ot.NumericalPoint(3)
PDFgrFD[0] = (ot.Frechet(distribution.getAlpha() + eps, distribution.getBeta(), distribution.getGamma()).computePDF(point) -
              ot.Frechet(distribution.getAlpha() - eps, distribution.getBeta(), distribution.getGamma()).computePDF(point)) / (2.0 * eps)
PDFgrFD[1] = (ot.Frechet(distribution.getAlpha(), distribution.getBeta() + eps, distribution.getGamma()).computePDF(point) -
              ot.Frechet(distribution.getAlpha(), distribution.getBeta() - eps, distribution.getGamma()).computePDF(point)) / (2.0 * eps)
PDFgrFD[2] = (ot.Frechet(distribution.getAlpha(), distribution.getBeta(), distribution.getGamma() + eps).computePDF(point) -
              ot.Frechet(distribution.getAlpha(), distribution.getBeta(), distribution.getGamma() - eps).computePDF(point)) / (2.0 * eps)
print("pdf gradient (FD)=", PDFgrFD)
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", CDFgr)
CDFgrFD = ot.NumericalPoint(3)
CDFgrFD[0] = (ot.Frechet(distribution.getAlpha() + eps, distribution.getBeta(), distribution.getGamma()).computeCDF(point) -
              ot.Frechet(distribution.getAlpha() - eps, distribution.getBeta(), distribution.getGamma()).computeCDF(point)) / (2.0 * eps)
CDFgrFD[1] = (ot.Frechet(distribution.getAlpha(), distribution.getBeta() + eps, distribution.getGamma()).computeCDF(point) -
              ot.Frechet(distribution.getAlpha(), distribution.getBeta() - eps, distribution.getGamma()).computeCDF(point)) / (2.0 * eps)
CDFgrFD[2] = (ot.Frechet(distribution.getAlpha(), distribution.getBeta(), distribution.getGamma() + eps).computeCDF(point) -
              ot.Frechet(distribution.getAlpha(), distribution.getBeta(), distribution.getGamma() - eps).computeCDF(point)) / (2.0 * eps)
print("cdf gradient (FD)=", CDFgrFD)
quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
mean = distribution.getMean()
print("mean=", mean)
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", standardDeviation)
# skewness = distribution.getSkewness()
# print("skewness=", skewness)
# kurtosis = distribution.getKurtosis()
# print("kurtosis=", kurtosis)
covariance = distribution.getCovariance()
print("covariance=", covariance)
correlation = distribution.getCorrelation()
print("correlation=", correlation)
spearman = distribution.getSpearmanCorrelation()
print("spearman=", spearman)
kendall = distribution.getKendallTau()
print("kendall=", kendall)
parameters = distribution.getParametersCollection()
print("parameters=", parameters)
for i in range(6):
    print("standard moment n=", i, ", value=",
          distribution.getStandardMoment(i))
print("Standard representative=", distribution.getStandardRepresentative())
