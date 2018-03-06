#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    f = SymbolicFunction("x", "x^2 + 2 * sin(x)")
    distribution = CompositeDistribution(f, Normal())
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
    # RandomGenerator::SetSeed(0)
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
    point = Point(distribution.getDimension(), 1.0)
    print("Point= ", point)

    # Show PDF and CDF of point
    eps = 1e-5
    DDF = distribution.computeDDF(point)
    print("ddf     =", DDF)
    LPDF = distribution.computeLogPDF(point)
    print("log pdf= %.12g" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     = %.10g" % PDF)
    print("pdf (FD)= %.10g" % ((distribution.computeCDF(point + Point(1, eps)) -
                                distribution.computeCDF(point + Point(1, -eps))) / (2.0 * eps)))
    CDF = distribution.computeCDF(point)
    print("cdf= %.12g" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf= %.12g" % CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival= %.12g" % Survival)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
    quantileTail = distribution.computeQuantile(0.95, True)
    print("quantile (tail)=", quantileTail)
    CDFTail = distribution.computeComplementaryCDF(quantileTail)
    print("cdf (tail)= %.12g" % CDFTail)
    # Get 95% survival function
    inverseSurvival = Point(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" %
          distribution.computeSurvivalFunction(inverseSurvival))
    # Takes too much time
    # print("entropy=%.6f" % distribution.computeEntropy())

    # Confidence regions
    interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", Point(1, threshold))
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(
        0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", Point(1, beta))
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", Point(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", Point(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", Point(1, beta))

    CF = distribution.computeCharacteristicFunction(point[0])
    print("characteristic function=(%.6g+%.6gj)" % (CF.real, CF.imag))
    LCF = distribution.computeLogCharacteristicFunction(point[0])
    print("log characteristic function=(%.6g+%.6gj)" % (LCF.real, LCF.imag))
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", PDFgr)
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", CDFgr)
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

    # Specific to this distribution
    antecedent = distribution.getAntecedent()
    print("antecedent=", antecedent)
    function = distribution.getFunction()
    print("function=", function)
    newDistribution = CompositeDistribution(function, antecedent)
    print("newDistribution=", newDistribution)

except:
    import sys
    print("t_CompositeDistribution_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
