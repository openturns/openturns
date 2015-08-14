#! /usr/bin/env python

from __future__ import print_function
import openturns as ot


parameters = ot.GammaMuSigma(0.1, 0.489898, -0.5)
distribution = ot.ParametrizedDistribution(parameters)

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

# Define a point
point = ot.NumericalPoint(distribution.getDimension(), 1.0)
print("Point= ", point)

# Show PDF and CDF of point
eps = 1e-5
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
print("ddf (FD)= %.6g" % ((distribution.computePDF(point + ot.NumericalPoint(1, eps)) -
                           distribution.computePDF(point + ot.NumericalPoint(1, -eps))) / (2.0 * eps)))
LPDF = distribution.computeLogPDF(point)
print("log pdf= %.6g" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.6g" % PDF)
print("pdf (FD)=%.6g" % ((distribution.computeCDF(point + ot.NumericalPoint(1, eps)) -
                          distribution.computeCDF(point + ot.NumericalPoint(1, -eps))) / (2.0 * eps)))
CDF = distribution.computeCDF(point)
print("cdf= %.6g" % CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf= %.6g" % CCDF)
Survival = distribution.computeSurvivalFunction(point)
print("survival= %.6g" % Survival)
CF = distribution.computeCharacteristicFunction(point[0])
print("characteristic function=(%.6g, %.6g)" % (CF.real, CF.imag))
LCF = distribution.computeLogCharacteristicFunction(point[0])
print("log characteristic function=(%.6g, %.6g)" % (LCF.real, LCF.imag))
PDFgr = distribution.computePDFGradient(point)
quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)=", distribution.computeCDF(quantile))
mean = distribution.getMean()
print("mean=", mean)
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
