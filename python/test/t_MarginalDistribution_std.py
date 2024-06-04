#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dimension = 5
indices = [2, 0, 1]
coll = [ot.Normal(dimension), ot.Multinomial(10, [1.0 / (dimension + 2)] * dimension)]

for nDistribution in range(len(coll)):
    fullDistribution = coll[nDistribution]
    distribution = ot.MarginalDistribution(fullDistribution, indices)
    # To check if indices accessors are correct
    otherDistribution = distribution
    otherDistribution.setIndices(distribution.getIndices())
    print("Test indices accessors", otherDistribution == distribution)
    # To check if distribution accessors are correct
    otherDistribution.setDistribution(distribution.getDistribution())
    print("Test distribution accessors", otherDistribution == distribution)

    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Is this distribution discrete ?
    print("Discrete = ", distribution.isDiscrete())

    # Is this distribution integral ?
    print("Integral = ", distribution.isIntegral())

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
    point = [1.0] * distribution.getDimension()
    print("Point= ", point)

    # Test the DDF/logDDF only in the continuous case
    if distribution.isContinuous():
        DDF = distribution.computeDDF(point)
        print("ddf     =", DDF)
        LPDF = distribution.computeLogPDF(point)
        print("log pdf=%.5e" % LPDF)

    # Show PDF and CDF of point
    PDF = distribution.computePDF(point)
    print("pdf     =%.5e" % PDF)
    CDF = distribution.computeCDF(point)
    print("cdf     =%.5e" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf    =%.5e" % CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival=%.5e" % Survival)
    if distribution.isContinuous():
        InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
        print("Inverse survival=", InverseSurvival)
        print(
            "Survival(inverse survival)=%.5e"
            % distribution.computeSurvivalFunction(InverseSurvival)
        )
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=%.5e" % distribution.computeCDF(quantile))
    quantileTail = distribution.computeQuantile(0.95, True)
    print("quantile (tail)=", quantileTail)
    CDFTail = distribution.computeComplementaryCDF(quantileTail)
    print("cdf (tail)=%.5e" % CDFTail)
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
    if distribution.isContinuous():
        spearman = distribution.getSpearmanCorrelation()
        print("spearman=", spearman)
        kendall = distribution.getKendallTau()
        print("kendall=", kendall)
    print(
        "Standard representative=", distribution.getStandardRepresentative().__str__()
    )
    # Additional tests to make codecov happy
    distribution = ot.MarginalDistribution(fullDistribution, 0)
