#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    distribution = NormalGamma(1.0, 2.0, 3.0, 4.0)
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
    point = [1.5] * distribution.getDimension()
    print("Point= ", point)

    # Show PDF and CDF of point
    eps = 1e-5
    DDF = distribution.computeDDF(point)
    print("ddf     =", DDF)
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf=%.6f" % PDF)
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival=%.6f" % Survival)
    InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
    print("Inverse survival=", InverseSurvival)
    print("Survival(inverse survival)=%.6f" %
          distribution.computeSurvivalFunction(InverseSurvival))
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", PDFgr)
    PDFgrFD = Point(4)
    PDFgrFD[0] = (NormalGamma(distribution.getMu() + eps, distribution.getKappa(), distribution.getAlpha(), distribution.getBeta()).computePDF(point)
                  - NormalGamma(distribution.getMu() - eps, distribution.getKappa(), distribution.getAlpha(), distribution.getBeta()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[1] = (NormalGamma(distribution.getMu(), distribution.getKappa() + eps, distribution.getAlpha(), distribution.getBeta()).computePDF(point)
                  - NormalGamma(distribution.getMu(), distribution.getKappa() - eps, distribution.getAlpha(), distribution.getBeta()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[2] = (NormalGamma(distribution.getMu(), distribution.getKappa(), distribution.getAlpha() + eps, distribution.getBeta()).computePDF(point)
                  - NormalGamma(distribution.getMu(), distribution.getKappa(), distribution.getAlpha() - eps, distribution.getBeta()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[3] = (NormalGamma(distribution.getMu(), distribution.getKappa(), distribution.getAlpha(), distribution.getBeta() + eps).computePDF(point)
                  - NormalGamma(distribution.getMu(), distribution.getKappa(), distribution.getAlpha(), distribution.getBeta() - eps).computePDF(point)) / (2.0 * eps)
    print("pdf gradient (FD)=", PDFgrFD)
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", CDFgr)
    CDFgrFD = Point(4)
    CDFgrFD[0] = (NormalGamma(distribution.getMu() + eps, distribution.getKappa(), distribution.getAlpha(), distribution.getBeta()).computeCDF(point)
                  - NormalGamma(distribution.getMu() - eps, distribution.getKappa(), distribution.getAlpha(), distribution.getBeta()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[1] = (NormalGamma(distribution.getMu(), distribution.getKappa() + eps, distribution.getAlpha(), distribution.getBeta()).computeCDF(point)
                  - NormalGamma(distribution.getMu(), distribution.getKappa() - eps, distribution.getAlpha(), distribution.getBeta()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[2] = (NormalGamma(distribution.getMu(), distribution.getKappa(), distribution.getAlpha() + eps, distribution.getBeta()).computeCDF(point)
                  - NormalGamma(distribution.getMu(), distribution.getKappa(), distribution.getAlpha() - eps, distribution.getBeta()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[3] = (NormalGamma(distribution.getMu(), distribution.getKappa(), distribution.getAlpha(), distribution.getBeta() + eps).computeCDF(point)
                  - NormalGamma(distribution.getMu(), distribution.getKappa(), distribution.getAlpha(), distribution.getBeta() - eps).computeCDF(point)) / (2.0 * eps)
    print("cdf gradient (FD)=", CDFgrFD)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
    print("entropy=%.6f" % distribution.computeEntropy())
    # Confidence regions
    probability, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95)
    print("Minimum volume interval=", probability)
    print("threshold=%.6f" % threshold)
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(
        0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=%.6f" % beta)
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=%.6f" % beta)
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=%.6f" % beta)
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=%.6f" % beta)
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
    # These computations take too much time for a test
    # CovarianceMatrix spearman = distribution.getSpearmanCorrelation()
    # print("spearman=", spearman)
    # CovarianceMatrix kendall = distribution.getKendallTau()
    # print("kendall=", kendall)

    parameters = distribution.getParametersCollection()
    print("parameters=", parameters)
    for i in range(6):
        print("standard moment n=", i, ", value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())

except:
    import sys
    print("t_NormalGamma_std.py", sys.exc_info()[0], sys.exc_info()[1])
