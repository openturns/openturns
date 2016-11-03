#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Instanciate one distribution object
    distribution = TruncatedNormal(0.5, 3.0, -2.0, 2.0)
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
    point = NumericalPoint(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    eps = 1e-5

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(DDF))
    # by the finite difference technique
    print("ddf (FD)=", repr(NumericalPoint(1, (distribution.computePDF(
        point + NumericalPoint(1, eps)) - distribution.computePDF(point + NumericalPoint(1, -eps))) / (2.0 * eps))))

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    # by the finite difference technique from CDF
    print("pdf (FD)=%.6f" % ((distribution.computeCDF(point + NumericalPoint(1, eps)) -
                              distribution.computeCDF(point + NumericalPoint(1, -eps))) / (2.0 * eps)))

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)

    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", repr(PDFgr))
    # by the finite difference technique
    PDFgrFD = NumericalPoint(4)
    PDFgrFD[0] = (TruncatedNormal(distribution.getMu() + eps, distribution.getSigma(), distribution.getA(), distribution.getB()).computePDF(point) -
                  TruncatedNormal(distribution.getMu() - eps, distribution.getSigma(), distribution.getA(), distribution.getB()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[1] = (TruncatedNormal(distribution.getMu(), distribution.getSigma() + eps, distribution.getA(), distribution.getB()).computePDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma() - eps, distribution.getA(), distribution.getB()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[2] = (TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA() + eps, distribution.getB()).computePDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA() - eps, distribution.getB()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[3] = (TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA(), distribution.getB() + eps).computePDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA(), distribution.getB() - eps).computePDF(point)) / (2.0 * eps)
    print("pdf gradient (FD)=", repr(PDFgrFD))

    # derivative of the logPDF with regards the parameters of the distribution
    logPDFgr = distribution.computeLogPDFGradient(point)
    print("log-pdf gradient     =", repr(logPDFgr))
    # by the finite difference technique
    logPDFgrFD = NumericalPoint(4)
    logPDFgrFD[0] = (TruncatedNormal(distribution.getMu() + eps, distribution.getSigma(), distribution.getA(), distribution.getB()).computeLogPDF(point) -
                  TruncatedNormal(distribution.getMu() - eps, distribution.getSigma(), distribution.getA(), distribution.getB()).computeLogPDF(point)) / (2.0 * eps)
    logPDFgrFD[1] = (TruncatedNormal(distribution.getMu(), distribution.getSigma() + eps, distribution.getA(), distribution.getB()).computeLogPDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma() - eps, distribution.getA(), distribution.getB()).computeLogPDF(point)) / (2.0 * eps)
    logPDFgrFD[2] = (TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA() + eps, distribution.getB()).computeLogPDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA() - eps, distribution.getB()).computeLogPDF(point)) / (2.0 * eps)
    logPDFgrFD[3] = (TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA(), distribution.getB() + eps).computeLogPDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA(), distribution.getB() - eps).computeLogPDF(point)) / (2.0 * eps)
    print("log-pdf gradient (FD)=", repr(logPDFgrFD))

    # derivative of the PDF with regards the parameters of the distribution
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", repr(CDFgr))
    CDFgrFD = NumericalPoint(4)
    CDFgrFD[0] = (TruncatedNormal(distribution.getMu() + eps, distribution.getSigma(), distribution.getA(), distribution.getB()).computeCDF(point) -
                  TruncatedNormal(distribution.getMu() - eps, distribution.getSigma(), distribution.getA(), distribution.getB()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[1] = (TruncatedNormal(distribution.getMu(), distribution.getSigma() + eps, distribution.getA(), distribution.getB()).computeCDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma() - eps, distribution.getA(), distribution.getB()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[2] = (TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA() + eps, distribution.getB()).computeCDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA() - eps, distribution.getB()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[3] = (TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA(), distribution.getB() + eps).computeCDF(point) -
                  TruncatedNormal(distribution.getMu(), distribution.getSigma(), distribution.getA(), distribution.getB() - eps).computeCDF(point)) / (2.0 * eps)
    print("cdf gradient (FD)=",  repr(CDFgrFD))

    # quantile
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = NumericalPoint(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" % distribution.computeSurvivalFunction(inverseSurvival))

    # Confidence regions
    interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", NumericalPoint(1, threshold))
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", NumericalPoint(1, beta))
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", NumericalPoint(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", NumericalPoint(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", NumericalPoint(1, beta))

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
        print("standard moment n=", i, " value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())


except:
    import sys
    print("t_TruncatedNormal_std.py", sys.exc_info()[0], sys.exc_info()[1])
