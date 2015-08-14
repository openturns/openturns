#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)


for distribution in [Gamma(1.5, 2.5, -0.5), Gamma(15.0, 2.5)]:
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
            msg = "rejected"
        print(
            "Kolmogorov test for the generator, sample size=", size, " is ", msg)
        size *= 10

    # Define a point
    point = NumericalPoint(distribution.getDimension(), 1.0)
    print("Point= ", point)

    # Show PDF and CDF of point
    eps = 1e-5
    DDF = distribution.computeDDF(point)
    print("ddf     =", DDF)
    print("ddf (FD)= %.6g" % ((distribution.computePDF(point + NumericalPoint(1, eps)) -
                               distribution.computePDF(point + NumericalPoint(1, -eps))) / (2.0 * eps)))
    LPDF = distribution.computeLogPDF(point)
    print("log pdf= %.6g" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6g" % PDF)
    print("pdf (FD)=%.6g" % ((distribution.computeCDF(point + NumericalPoint(1, eps)) -
                              distribution.computeCDF(point + NumericalPoint(1, -eps))) / (2.0 * eps)))
    CDF = distribution.computeCDF(point)
    print("cdf= %.6g" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf= %.6g" % CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival= %.6g" % Survival)
    CF = distribution.computeCharacteristicFunction(point[0])
    print("characteristic function=(%.6g, %.6g)" % (CF.real, CF.imag))
    LCF = distribution.computeLogCharacteristicFunction(point[0])
    print("log characteristic function=(%.6g, %.6g)" % (LCF.real, LCF.imag))
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", PDFgr)
    PDFgrFD = NumericalPoint(3)
    PDFgrFD[0] = (Gamma(distribution.getK() + eps, distribution.getLambda(), distribution.getGamma()).computePDF(point) -
                  Gamma(distribution.getK() - eps, distribution.getLambda(), distribution.getGamma()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[1] = (Gamma(distribution.getK(), distribution.getLambda() + eps, distribution.getGamma()).computePDF(point) -
                  Gamma(distribution.getK(), distribution.getLambda() - eps, distribution.getGamma()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[2] = (Gamma(distribution.getK(), distribution.getLambda(), distribution.getGamma() + eps).computePDF(point) -
                  Gamma(distribution.getK(), distribution.getLambda(), distribution.getGamma() - eps).computePDF(point)) / (2.0 * eps)
    print("pdf gradient (FD)=", PDFgrFD)
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", CDFgr)
    CDFgrFD = NumericalPoint(3)
    CDFgrFD[0] = (Gamma(distribution.getK() + eps, distribution.getLambda(), distribution.getGamma()).computeCDF(point) -
                  Gamma(distribution.getK() - eps, distribution.getLambda(), distribution.getGamma()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[1] = (Gamma(distribution.getK(), distribution.getLambda() + eps, distribution.getGamma()).computeCDF(point) -
                  Gamma(distribution.getK(), distribution.getLambda() - eps, distribution.getGamma()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[2] = (Gamma(distribution.getK(), distribution.getLambda(), distribution.getGamma() + eps).computeCDF(point) -
                  Gamma(distribution.getK(), distribution.getLambda(), distribution.getGamma() - eps).computeCDF(point)) / (2.0 * eps)
    print("cdf gradient (FD)=", CDFgrFD)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=", distribution.computeCDF(quantile))
    mean = distribution.getMean()
    print("mean=", mean)
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
    mu = distribution.getMu()
    print("mu= %.6g" % mu)
    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", standardDeviation)
    skewness = distribution.getSkewness()
    print("skewness=", skewness)
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", kurtosis)
    sigma = distribution.getSigma()
    print("sigma= %.6g" % sigma)
    newDistribution = Gamma(
        mu, sigma, distribution.getGamma(), Gamma.MUSIGMA)
    print("k from (mu, sigma)= %.6g" % newDistribution.getK())
    print("lambda from (mu, sigma)= %.6g" % newDistribution.getLambda())
