#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    ResourceMap.SetAsUnsignedInteger(
        "MeixnerDistribution-CDFIntegrationNodesNumber", 8)
    ResourceMap.SetAsUnsignedInteger(
        "MeixnerDistribution-CDFDiscretization", 100)
    ResourceMap.SetAsScalar(
        "MeixnerDistribution-MaximumAbsoluteError", 1.0e-6)
    ResourceMap.SetAsScalar(
        "MeixnerDistribution-MaximumRelativeError", 1.0e-6)
    ResourceMap.SetAsScalar(
        "MeixnerDistribution-MaximumConstraintError", 1.0e-6)
    ResourceMap.SetAsScalar(
        "MeixnerDistribution-MaximumObjectiveError", 1.0e-6)
    # Instanciate one distribution object
    distribution = MeixnerDistribution(1.5, 0.5, 2.5, -0.5)
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
    size = 100
    for i in range(2):
        if FittingTest.Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure():
            msg = "accepted"
        else:
            msg = rejected
        print(
            "Kolmogorov test for the generator, sample size=", size, " is ", msg)
        size *= 10

    # Define a point
    point = Point(distribution.getDimension(), 1.0)
    print("Point= ", point)

    # Show PDF and CDF of point
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    eps = 1.0e-5
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    print("pdf (FD)=%.6f" % ((distribution.computeCDF(point + Point(1, eps)) -
                              distribution.computeCDF(point + Point(1, -eps))) / (2.0 * eps)))
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    CF = distribution.computeCharacteristicFunction(point[0])
    print("characteristic function=(%.12g%+.12gj)" % (CF.real, CF.imag))
    LCF = distribution.computeLogCharacteristicFunction(point[0])
    print("log characteristic function=(%.12g%+.12gj)" % (LCF.real, LCF.imag))
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = Point(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" %
          distribution.computeSurvivalFunction(inverseSurvival))
    print("entropy=%.6f" % distribution.computeEntropy())

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

    mean = distribution.getMean()
    print("mean=", mean)
    covariance = distribution.getCovariance()
    print("covariance=", covariance)
    parameters = distribution.getParametersCollection()
    print("parameters=", parameters)
    for i in range(3):
        print("standard moment n=", i, ", value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())

    # Specific to this distribution
    alpha = distribution.getAlpha()
    print("alpha=%.6f" % alpha)
    beta = distribution.getBeta()
    print("beta=%.6f" % beta)
    delta = distribution.getDelta()
    print("delta=%.6f" % delta)
    gamma = distribution.getGamma()
    print("gamma=%.6f" % gamma)
    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", standardDeviation)
    skewness = distribution.getSkewness()
    print("skewness=", skewness)
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", kurtosis)

except:
    import sys
    print("t_MeixnerDistribution_std.py", sys.exc_info()[0], sys.exc_info()[1])
