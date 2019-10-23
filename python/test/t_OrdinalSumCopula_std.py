#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Instanciate one distribution object
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.5
collection = [ot.FrankCopula(3.0), ot.NormalCopula(R), ot.ClaytonCopula(2.0)]
bounds = [0.2, 0.7]
copula = ot.OrdinalSumCopula(collection, bounds)
print("Copula ", copula)

# Is this copula elliptical ?
print("Elliptical distribution= ", copula.isElliptical())

# Is this copula continuous ?
print("Continuous = ", copula.isContinuous())

# Is this copula elliptical ?
print("Elliptical = ", copula.hasEllipticalCopula())

# Is this copula independent ?
print("Independent = ", copula.hasIndependentCopula())

# Test for realization of copula
oneRealization = copula.getRealization()
print("oneRealization=", oneRealization)

# Test for sampling
size = 10000
oneSample = copula.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
print("mean=", oneSample.computeMean())
precision = ot.PlatformInfo.GetNumericalPrecision()
ot.PlatformInfo.SetNumericalPrecision(5)
print("covariance=", oneSample.computeCovariance())
ot.PlatformInfo.SetNumericalPrecision(precision)

# Define a point
dim = copula.getDimension()
point = [0.6]*dim
print("Point= ", point)

# Show PDF and CDF of point
# Scalar eps(1e-5)
DDF = copula.computeDDF(point)
print("ddf     =", DDF)
PDF = copula.computePDF(point)
print("pdf     =%.5f" % PDF)
CDF = copula.computeCDF(point)
print("cdf=%.5f" % CDF)
Survival = copula.computeSurvivalFunction(point)
print("Survival      =%.5f" % Survival)
print("Survival (ref)=%.5f" % copula.computeSurvivalFunction(point))
InverseSurvival = copula.computeInverseSurvivalFunction(0.95)
print("Inverse survival=", InverseSurvival)
print("Survival(inverse survival)=%.5f" %
      copula.computeSurvivalFunction(InverseSurvival))
# Get 50% quantile
quantile = copula.computeQuantile(0.5)
print("Quantile=", quantile)
print("CDF(quantile)=", copula.computeCDF(quantile))

if (dim <= 2):
    # Confidence regions
    interval, threshold = copula.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95)
    print("Minimum volume interval=", interval)
    print("threshold=%.5f" % threshold)
    levelSet, beta = copula.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=%.5f" % beta)
    interval, beta = copula.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=%.5f" % beta)
    interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=%.5f" % beta)
    interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=%.5f" % beta)

    print("entropy=%.5f" % copula.computeEntropy())
    print("entropy (MC)=%.5f" % -
          copula.computeLogPDF(copula.getSample(1000000)).computeMean()[0])
mean = copula.getMean()
print("mean=", mean)
precision = ot.PlatformInfo.GetNumericalPrecision()
ot.PlatformInfo.SetNumericalPrecision(5)
# Covariance and correlation
covariance = copula.getCovariance()
print("covariance=", covariance)
correlation = copula.getCorrelation()
print("correlation=", correlation)
spearman = copula.getSpearmanCorrelation()
print("spearman=", spearman)
kendall = copula.getKendallTau()
print("kendall=", kendall)
ot.PlatformInfo.SetNumericalPrecision(precision)
parameters = copula.getParametersCollection()
print("parameters=", parameters)
x = 0.6
y = [0.2]*(dim-1)
print("conditional PDF=%.6f" % copula.computeConditionalPDF(x, y))
print("conditional CDF=%.6f" % copula.computeConditionalCDF(x, y))
print("conditional quantile=%.6f" % copula.computeConditionalQuantile(x, y))
pt = ot.Point([0.1 * i + 0.05 for i in range(dim)])
print("sequential conditional PDF=",
      copula.computeSequentialConditionalPDF(point))
resCDF = copula.computeSequentialConditionalCDF(pt)
print("sequential conditional CDF(", pt, ")=", resCDF)
print("sequential conditional quantile(", resCDF, ")=",
      copula.computeSequentialConditionalQuantile(resCDF))
