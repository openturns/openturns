#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)


def cleanNumericalPoint(inNumericalPoint):
    dim = inNumericalPoint.getDimension()
    for i in range(dim):
        if (fabs(inNumericalPoint[i]) < 1.e-10):
            inNumericalPoint[i] = 0.0
    return inNumericalPoint

try:

        # Instanciate one distribution object
    referenceDistribution = [TruncatedNormal(2.0, 1.5, 1.0, 4.0), TruncatedNormal(
        2.0, 1.5, 1.0, 200.0), TruncatedNormal(2.0, 1.5, -200.0, 4.0)]
    distribution = [TruncatedDistribution(Normal(2.0, 1.5), 1.0, 4.0), TruncatedDistribution(
        Normal(2.0, 1.5), 1.0, TruncatedDistribution.LOWER), TruncatedDistribution(Normal(2.0, 1.5), 4.0, TruncatedDistribution.UPPER)]
    for testCase in range(3):
        print("Distribution ", distribution[testCase])

        # Is this distribution elliptical ?
        print("Elliptical = ", distribution[testCase].isElliptical())

        # Is this distribution continuous ?
        print("Continuous = ", distribution[testCase].isContinuous())

        # Test for realization of distribution
        oneRealization = distribution[testCase].getRealization()
        print("oneRealization=", repr(oneRealization))

        # Test for sampling
        size = 10000
        oneSample = distribution[testCase].getSample(size)
        print("oneSample first=", repr(
            oneSample[0]), " last=", repr(oneSample[size - 1]))
        print("mean=", repr(oneSample.computeMean()))
        print("covariance=", repr(oneSample.computeCovariance()))

        # Define a point
        point = NumericalPoint(distribution[testCase].getDimension(), 2.5)
        print("Point= ", repr(point))

        # Show PDF and CDF of point
        eps = 1e-5

        DDF = distribution[testCase].computeDDF(point)
        print("ddf      =", repr(DDF))
        print("ddf (ref)=", repr(
            referenceDistribution[testCase].computeDDF(point)))

        PDF = distribution[testCase].computePDF(point)
        print("pdf      =%.6f" % PDF)
        print("pdf (ref)=%.6f" %
              referenceDistribution[testCase].computePDF(point))

        CDF = distribution[testCase].computeCDF(point)
        print("cdf=%.6f" % CDF)
        CCDF = distribution[testCase].computeComplementaryCDF(point)
        print("ccdf=%.6f" % CCDF)
        print("cdf (ref)=%.6f" %
              referenceDistribution[testCase].computeCDF(point))
        PDFgr = distribution[testCase].computePDFGradient(point)
        print("pdf gradient      =", repr(cleanNumericalPoint(PDFgr)))
        print("pdf gradient (ref)=", repr(
            cleanNumericalPoint(referenceDistribution[testCase].computePDFGradient(point))))

        CDFgr = distribution[testCase].computeCDFGradient(point)
        print("cdf gradient      =", repr(cleanNumericalPoint(CDFgr)))
        print("cdf gradient (ref)=", repr(
            cleanNumericalPoint(referenceDistribution[testCase].computeCDFGradient(point))))

        # quantile
        quantile = distribution[testCase].computeQuantile(0.95)
        print("quantile=", repr(quantile))
        print(
            "quantile=", repr(referenceDistribution[testCase].computeQuantile(0.95)))
        print("cdf(quantile)=%.6f" %
              distribution[testCase].computeCDF(quantile))
        mean = distribution[testCase].getMean()
        print("mean      =", repr(mean))
        print("mean (ref)=", repr(referenceDistribution[testCase].getMean()))
        standardDeviation = distribution[testCase].getStandardDeviation()
        print("standard deviation      =", repr(standardDeviation))
        print("standard deviation (ref)=", repr(
            referenceDistribution[testCase].getStandardDeviation()))
        skewness = distribution[testCase].getSkewness()
        print("skewness      =", repr(skewness))
        print("skewness (ref)=", repr(
            referenceDistribution[testCase].getSkewness()))
        kurtosis = distribution[testCase].getKurtosis()
        print("kurtosis      =", repr(kurtosis))
        print("kurtosis (ref)=", repr(
            referenceDistribution[testCase].getKurtosis()))
        covariance = distribution[testCase].getCovariance()
        print("covariance      =", repr(covariance))
        print("covariance (ref)=", repr(
            referenceDistribution[testCase].getCovariance()))
        parameters = distribution[testCase].getParametersCollection()
        print("parameters      =", repr(parameters))
        print("parameters (ref)=", repr(
            referenceDistribution[testCase].getParametersCollection()))
        for i in range(6):
            print("standard moment n=", i, " value=",
                  referenceDistribution[testCase].getStandardMoment(i))
        print("Standard representative=", referenceDistribution[
              testCase].getStandardRepresentative())

except:
    import sys
    print("t_TruncatedDistribution_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
