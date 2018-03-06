#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Instanciate one distribution object
distribution = ot.VonMises(-0.5, 1.5)
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
point = ot.Point(distribution.getDimension(), 1.0)
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
PDFgrFD = ot.Point(2)
PDFgrFD[0] = (ot.VonMises(distribution.getMu() + eps, distribution.getKappa()).computePDF(point) -
              ot.VonMises(distribution.getMu() - eps, distribution.getKappa()).computePDF(point)) / (2.0 * eps)
PDFgrFD[1] = (ot.VonMises(distribution.getMu(), distribution.getKappa() + eps).computePDF(point) -
              ot.VonMises(distribution.getMu(), distribution.getKappa() - eps).computePDF(point)) / (2.0 * eps)
print("pdf gradient (FD)=", PDFgrFD)
CDFgr = distribution.computeCDFGradient(point)
print("cdf gradient     =", CDFgr)
CDFgrFD = ot.Point(2)
CDFgrFD[0] = (ot.VonMises(distribution.getMu() + eps, distribution.getKappa()).computeCDF(point) -
              ot.VonMises(distribution.getMu() - eps, distribution.getKappa()).computeCDF(point)) / (2.0 * eps)
CDFgrFD[1] = (ot.VonMises(distribution.getMu(), distribution.getKappa() + eps).computeCDF(point) -
              ot.VonMises(distribution.getMu(), distribution.getKappa() - eps).computeCDF(point)) / (2.0 * eps)
print("cdf gradient (FD)=", CDFgrFD)
quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print("Survival(inverseSurvival)=%.6f" %
      distribution.computeSurvivalFunction(inverseSurvival))
print("entropy=%.6f" % distribution.computeEntropy())

# Confidence regions
interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
    0.95)
print("Minimum volume interval=", interval)
print("threshold=", ot.Point(1, threshold))
levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
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
print("mean=", mean)
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
print("Circular mean=", distribution.getCircularMean())
print("Circular variance= %.12g" % distribution.getCircularVariance())
