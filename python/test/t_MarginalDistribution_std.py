#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dimension = 5
indices = [2, 0, 1]
fullDistribution = ot.JointDistribution(
    [ot.Normal(), ot.Uniform(), ot.Exponential(), ot.WeibullMin()],
    ot.ComposedCopula([ot.GumbelCopula(), ot.ClaytonCopula()]),
)
distribution = ot.MarginalDistribution(fullDistribution, indices)

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
if distribution.getDimension() == 1:
    size = 100
    for i in range(2):
        print(
            "Kolmogorov test for the generator, sample size=",
            size,
            " is ",
            ot.FittingTest.Kolmogorov(
                distribution.getSample(size), distribution
            ).getBinaryQualityMeasure(),
        )
        size *= 10

# Define a point
point = [1.0] * distribution.getDimension()
print("Point= ", point)

# Show PDF and CDF of point
DDF = distribution.computeDDF(point)
print("ddf     =", DDF)
LPDF = distribution.computeLogPDF(point)
print("log pdf=", LPDF)
PDF = distribution.computePDF(point)
print("pdf     =", PDF)
CDF = distribution.computeCDF(point)
print("cdf=", CDF)
CCDF = distribution.computeComplementaryCDF(point)
print("ccdf=", CCDF)
Survival = distribution.computeSurvivalFunction(point)
print("survival=", Survival)
quantile = distribution.computeQuantile(0.95)
print("quantile=", quantile)
print("cdf(quantile)=", distribution.computeCDF(quantile))
quantileTail = distribution.computeQuantile(0.95, True)
print("quantile (tail)=", quantileTail)
CDFTail = distribution.computeComplementaryCDF(quantileTail)
print("cdf (tail)=", CDFTail)
# Get 95% survival function
inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
print("InverseSurvival=", repr(inverseSurvival))
print(
    "Survival(inverseSurvival)=%.6f"
    % distribution.computeSurvivalFunction(inverseSurvival)
)
print("entropy=%.6f" % distribution.computeEntropy())
# Confidence regions
threshold = ot.Point()
print(
    "Minimum volume interval=",
    distribution.computeMinimumVolumeInterval(0.95, threshold),
)
print("threshold=", threshold)
beta = ot.Point()
levelSet = distribution.computeMinimumVolumeLevelSet(0.95, beta)
print("Minimum volume level set=", levelSet)
print("beta=", beta)
print(
    "Bilateral confidence interval=",
    distribution.computeBilateralConfidenceInterval(0.95, beta),
)
print("beta=", beta)
print(
    "Unilateral confidence interval (lower tail)=",
    distribution.computeUnilateralConfidenceInterval(0.95, False, beta),
)
print("beta=", beta)
print(
    "Unilateral confidence interval (upper tail)=",
    distribution.computeUnilateralConfidenceInterval(0.95, True, beta),
)
print("beta=", beta)

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
print("Standard representative=", distribution.getStandardRepresentative())
