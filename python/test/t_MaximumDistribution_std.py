#! /usr/bin/env python

from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

# First constructor: distribution of the maximum of the components
# of a random vector given its joint distribution
# Second constructor: distribution of the maximum of independent
# random variables given their distributions
# Third constructor: distribution of the maximum of independent
# identically distributed random variables
coll = [MaximumDistribution(Normal(5)),
        MaximumDistribution([Normal()]*5),
        MaximumDistribution(Normal(), 5)]
for distribution in coll:
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
    RandomGenerator.SetSeed(0)
    size = 100
    for i in range(2):
        msg = ''
        if FittingTest.Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure():
            msg = "accepted"
        else:
            msg = "rejected"
        print(
            "Kolmogorov test for the generator, sample size=", size, " is", msg)
        size *= 10

    # Define a point
    point = [1.0]*distribution.getDimension()
    print("Point= ", point)

    # Show PDF and CDF of point
    DDF = distribution.computeDDF(point)
    print("ddf     =", DDF)
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.5g" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf    =%.5g" % PDF)
    CDF = distribution.computeCDF(point)
    print("cdf =%.5g" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.5g" % CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival=%.5g" % Survival)
    InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
    print("Inverse survival=", InverseSurvival)
    print("Survival(inverse survival)=%.5g" %
          distribution.computeSurvivalFunction(InverseSurvival))
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=%.5g" % distribution.computeCDF(quantile))
    quantileTail = distribution.computeQuantile(0.95, True)
    print("quantile (tail)=", quantileTail)
    CDFTail = distribution.computeComplementaryCDF(quantileTail)
    print("cdf (tail)=%.5g" % CDFTail)
    # Confidence regions
    interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", threshold)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(
        0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=%.5g" % beta)
    levelSet, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95)
    print("Bilateral confidence interval=", levelSet)
    print("beta=%.5g" % beta)
    levelSet, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=%.5g" % beta)
    levelSet, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=%.5g" % beta)
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
    for i in range(6):
        print("standard moment n=", i, ", value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())
# Issue #1643
coll = [Uniform(), Normal()]
distribution = MaximumDistribution(coll)
print("%.5g" % distribution.computePDF([1.1]))
