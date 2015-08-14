#! /usr/bin/env python
# -*- coding: iso-8859-15 -*-

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    distribution = Rice(5, 4)
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
    point = NumericalPoint(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    eps = 1e-5
    PDF = distribution.computePDF(point)
    print("pdf     = %.12g" % PDF)
    print("pdf (FD)= %.9f" % ((distribution.computeCDF(point + NumericalPoint(1, eps)) -
                               distribution.computeCDF(point + NumericalPoint(1, -eps))) / (2.0 * eps), ))
    CDF = distribution.computeCDF(point)
    print("cdf= %.12g" % CDF)
    # CF = distribution.computeCharacteristicFunction( point[0] )
    # print "characteristic function=", CF
    PDFgrFD = NumericalPoint(2)
    PDFgrFD[0] = (Rice(distribution.getSigma() + eps, distribution.getNu()).computePDF(point) -
                  Rice(distribution.getSigma() - eps, distribution.getNu()).computePDF(point)) / (2.0 * eps)

    PDFgrFD[1] = (Rice(distribution.getSigma(), distribution.getNu() + eps).computePDF(point) -
                  Rice(distribution.getSigma(), distribution.getNu() - eps).computePDF(point)) / (2.0 * eps)
    print("pdf gradient (FD)=", repr(PDFgrFD))
    CDFgrFD = NumericalPoint(2)
    CDFgrFD[0] = (Rice(distribution.getSigma() + eps, distribution.getNu()).computeCDF(point) -
                  Rice(distribution.getSigma() - eps, distribution.getNu()).computeCDF(point)) / (2.0 * eps)

    CDFgrFD[1] = (Rice(distribution.getNu() + eps, distribution.getNu()).computeCDF(point) -
                  Rice(distribution.getNu() - eps, distribution.getNu()).computeCDF(point)) / (2.0 * eps)
    print("cdf gradient (FD)=", repr(CDFgrFD))
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
    mean = distribution.getMean()
    print("mean=", repr(mean))
    covariance = distribution.getCovariance()
    print("covariance=", repr(covariance))
    parameters = distribution.getParametersCollection()
    print("parameters=", repr(parameters))
    for i in range(6):
        print("standard moment n=", i, " value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())

    # Specific to this distribution
    nu = distribution.getNu()
    print("nu=", nu)
    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", repr(standardDeviation))
    skewness = distribution.getSkewness()
    print("skewness=", repr(skewness))
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", repr(kurtosis))
    sigma = distribution.getSigma()
    print("sigma=", sigma)

except:
    import sys
    print("t_Rice_std.py", sys.exc_info()[0], sys.exc_info()[1])
