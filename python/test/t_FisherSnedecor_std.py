#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from cmath import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    distribution = FisherSnedecor(5.5, 10.5)
    print("Distribution ", repr(distribution))
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
    point = Point(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    eps = 1e-5

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    # print "ddf     =" , repr(DDF)
    # by the finite difference technique
    # print "ddf (FD)=" ,repr(Point(1, (distribution.computePDF(
    # point + Point(1, eps) ) - distribution.computePDF( point  +
    # Point(1, -eps) )) / (2.0 * eps)))

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    # by the finite difference technique from CDF
    print("pdf (FD)=%.6f" % ((distribution.computeCDF(point + Point(1, eps)) -
                              distribution.computeCDF(point + Point(1, -eps))) / (2.0 * eps)))

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    CF = distribution.computeCharacteristicFunction(point[0])
    print("characteristic function=(%.6f+%.6fj)" % (CF.real, CF.imag))
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", repr(PDFgr))
    # by the finite difference technique
    PDFgrFD = Point(2)
    PDFgrFD[0] = (FisherSnedecor(distribution.getD1() + eps, distribution.getD2()).computePDF(point)
                  - FisherSnedecor(distribution.getD1() - eps, distribution.getD2()).computePDF(point)) / (2.0 * eps)
    PDFgrFD[1] = (FisherSnedecor(distribution.getD1(), distribution.getD2() + eps).computePDF(point)
                  - FisherSnedecor(distribution.getD1(), distribution.getD2() - eps).computePDF(point)) / (2.0 * eps)
    print("pdf gradient (FD)=", repr(PDFgrFD))

    # derivative of the PDF with regards the parameters of the distribution
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", repr(CDFgr))
    CDFgrFD = Point(2)
    CDFgrFD[0] = (FisherSnedecor(distribution.getD1() + eps, distribution.getD2()).computeCDF(point)
                  - FisherSnedecor(distribution.getD1() - eps, distribution.getD2()).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[1] = (FisherSnedecor(distribution.getD1(), distribution.getD2() + eps).computeCDF(point)
                  - FisherSnedecor(distribution.getD1(), distribution.getD2() - eps).computeCDF(point)) / (2.0 * eps)
    print("cdf gradient (FD)=",  repr(CDFgrFD))

    # derivative of the logPDF with regards the parameters of the distribution
    logPDFgr = distribution.computeLogPDFGradient(point)
    print("log-pdf gradient     =", repr(logPDFgr))
    # by the finite difference technique
    logPDFgrFD = Point(2)
    logPDFgrFD[0] = (FisherSnedecor(distribution.getD1() + eps, distribution.getD2()).computeLogPDF(point)
                     - FisherSnedecor(distribution.getD1() - eps, distribution.getD2()).computeLogPDF(point)) / (2.0 * eps)
    logPDFgrFD[1] = (FisherSnedecor(distribution.getD1(), distribution.getD2() + eps).computeLogPDF(point)
                     - FisherSnedecor(distribution.getD1(), distribution.getD2() - eps).computeLogPDF(point)) / (2.0 * eps)
    print("log-pdf gradient (FD)=", repr(logPDFgrFD))

    # quantile
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
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
    print("t_FisherSnedecor_std.py", sys.exc_info()[0], sys.exc_info()[1])
