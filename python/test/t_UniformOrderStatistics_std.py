#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp

ot.TESTPREAMBLE()

# Instantiate one distribution object
distribution = otexp.UniformOrderStatistics(4)
print("Distribution ", repr(distribution))
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
point = [0.1, 0.15, 0.25, 0.45]
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf=%.5e" % LPDF)
PDF = distribution.computePDF(point)
print("pdf     =%.5e" % PDF)
CDF = distribution.computeCDF(point)
print("cdf     =%.5e" % CDF)
# Too expensive for a test
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf    =%.5e" % CCDF)
Survival = distribution.computeSurvivalFunction(point)
print("survival=%.5e" % Survival)
InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
print("Inverse survival=", InverseSurvival)
print(
    "Survival(inverse survival)=%.5e"
    % distribution.computeSurvivalFunction(InverseSurvival)
)
quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)=%.5e" % distribution.computeCDF(quantile))

print("entropy     =%.5e" % distribution.computeEntropy())
print(
    "entropy (MC)=%.5e"
    % -distribution.computeLogPDF(distribution.getSample(1000000)).computeMean()[0]
)
mean = distribution.getMean()
print("mean=", mean)
#
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", standardDeviation)
skewness = distribution.getSkewness()
print("skewness=", skewness)
kurtosis = distribution.getKurtosis()
print("kurtosis=", kurtosis)
covariance = distribution.getCovariance()
print("covariance=", covariance)
correlation = distribution.getCorrelation()
print("correlation=", correlation)
# Too slow for a test
# spearman = distribution.getSpearmanCorrelation()
# print("spearman=", spearman)
# kendall = distribution.getKendallTau()
# print("kendall=", kendall)
parameters = distribution.getParametersCollection()
print("parameters=", parameters)
print("Standard representative=", distribution.getStandardRepresentative())
