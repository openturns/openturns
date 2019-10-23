#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()


# Instanciate one distribution object
distribution = ot.Pareto(7.5, 5.0, -7.0)

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
print("oneSample first=", repr(
    oneSample[0]), " last=", repr(oneSample[size - 1]))
print("mean=", repr(oneSample.computeMean()))
print("covariance=", repr(oneSample.computeCovariance()))
print("skewness=", repr(oneSample.computeSkewness()))
print("kurtosis=", repr(oneSample.computeKurtosis()))

size = 100
for i in range(2):
    msg = ''
    if ot.FittingTest.Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure():
        msg = "accepted"
    else:
        msg = "rejected"
    print("Kolmogorov test for the generator, sample size=", size, " is", msg)
    size *= 10

# Define a point
point = ot.Point(distribution.getDimension(), 1.0)
print("Point= ", repr(point))
print("range= ", distribution.getRange())

# Show PDF and CDF of point
eps = 1e-5
DDF = distribution.computeDDF(point)
print("ddf     =", repr(DDF))
print("ddf (FD)=", repr(ot.Point(1, (distribution.computePDF(
    point + ot.Point(1, eps)) - distribution.computePDF(point + ot.Point(1, -eps))) / (2.0 * eps))))
PDF = distribution.computePDF(point)
print("pdf     = %.12g" % PDF)
print("pdf (FD)= %.9f" % ((distribution.computeCDF(point + ot.Point(1, eps)) -
                           distribution.computeCDF(point + ot.Point(1, -eps))) / (2.0 * eps), ))

CDF = distribution.computeCDF(point)
print("cdf= %.12g" % CDF)
# CF = distribution.computeCharacteristicFunction( point[0] )
# print "characteristic function=", CF
PDFgr = distribution.computePDFGradient(point)
print("pdf gradient     =", repr(PDFgr))
PDFgrFD = ot.Point(3)
PDFgrFD[0] = (ot.Pareto(distribution.getBeta() + eps, distribution.getAlpha(), distribution.getGamma()).computePDF(point) -
              ot.Pareto(distribution.getBeta() - eps, distribution.getAlpha(), distribution.getGamma()).computePDF(point)) / (2.0 * eps)
PDFgrFD[1] = (ot.Pareto(distribution.getBeta(), distribution.getAlpha() + eps, distribution.getGamma()).computePDF(point) -
              ot.Pareto(distribution.getBeta(), distribution.getAlpha() - eps, distribution.getGamma()).computePDF(point)) / (2.0 * eps)
PDFgrFD[2] = (ot.Pareto(distribution.getBeta(), distribution.getAlpha(), distribution.getGamma() + eps).computePDF(point) -
              ot.Pareto(distribution.getBeta(), distribution.getAlpha(), distribution.getGamma() - eps).computePDF(point)) / (2.0 * eps)
print("pdf gradient (FD)=", repr(PDFgrFD))
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", repr(CDFgr))
CDFgrFD = ot.Point(3)
CDFgrFD[0] = (ot.Pareto(distribution.getBeta() + eps, distribution.getAlpha(), distribution.getGamma()).computeCDF(point) -
              ot.Pareto(distribution.getBeta() - eps, distribution.getAlpha(), distribution.getGamma()).computeCDF(point)) / (2.0 * eps)
CDFgrFD[1] = (ot.Pareto(distribution.getBeta(), distribution.getAlpha() + eps, distribution.getGamma()).computeCDF(point) -
              ot.Pareto(distribution.getBeta(), distribution.getAlpha() - eps, distribution.getGamma()).computeCDF(point)) / (2.0 * eps)
CDFgrFD[2] = (ot.Pareto(distribution.getBeta(), distribution.getAlpha(), distribution.getGamma() + eps).computeCDF(point) -
              ot.Pareto(distribution.getBeta(), distribution.getAlpha(), distribution.getGamma() - eps).computeCDF(point)) / (2.0 * eps)
print("cdf gradient (FD)=", repr(CDFgrFD))
quantile = distribution.computeQuantile(0.95)
print("quantile=", repr(quantile))
print("cdf(quantile)=", distribution.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(
    distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print("Survival(inverseSurvival)=%.6f" %
      distribution.computeSurvivalFunction(inverseSurvival))
print("entropy=%.6f" % distribution.computeEntropy())

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(
    0.95)
print("Minimum volume level set=", levelSet)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
    0.95)
print("Bilateral confidence interval=", interval)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, False)
print("Unilateral confidence interval (lower tail)=", interval)
print("beta=", ot.Point(1, beta))
interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
    0.95, True)
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
for i in range(6):
    try:
        value = distribution.getStandardMoment(i)
        print("standard moment n=", i, " value=", value)
    except RuntimeError as ex:
        print(ex)
print("Standard representative=",
      distribution.getStandardRepresentative())
