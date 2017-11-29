#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # 1D tests
    distribution = Dirac(0.7)
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
    sampleCovariance = oneSample.computeCovariance()[0, 0]
    if (fabs(sampleCovariance) < 1.0e-16):
        sampleCovariance = 0.0
    print("covariance=", sampleCovariance)

    # Define a point
    point = Point(distribution.getDimension(), 0.0)
    print("Point= ", point)

    # Show PDF and CDF of point
    PDF = distribution.computePDF(point)
    print("pdf     =", PDF)
    print("pdf (FD)=", (distribution.computeCDF(point + Point(1, 0)) -
                        distribution.computeCDF(point + Point(1, -1))))
    CDF = distribution.computeCDF(point)
    print("cdf=", CDF)

    # Define a point
    point = Point(distribution.getSupport(distribution.getRange())[0])
    print("Point= ", point)

    # Show PDF and CDF of point
    PDF = distribution.computePDF(point)
    print("pdf     =", PDF)
    print("pdf (FD)=", (distribution.computeCDF(point + Point(1, 0)) -
                        distribution.computeCDF(point + Point(1, -1))))
    CDF = distribution.computeCDF(point)
    print("cdf=", CDF)
    CF = distribution.computeCharacteristicFunction(0.5)
    print("characteristic function= (%.12g%+.12gj)" % (CF.real, CF.imag))
    GF = distribution.computeGeneratingFunction(0.5 + 0.3j)
    print("generating function= (%.12g%+.12gj)" % (GF.real, GF.imag))
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=", distribution.computeCDF(quantile))
    print("entropy=%.6f" % distribution.computeEntropy())
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
    parameters = distribution.getParametersCollection()
    print("parameters=", parameters)
    for i in range(6):
        print("standard moment n=", i, " value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())

    # N-D tests

    dim = 4
    distribution = Dirac(Point(dim, 2.3))
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
    sampleCovariance = oneSample.computeCovariance()
    for i in range(dim):
        for j in range(i + 1):
            if (fabs(sampleCovariance[i, j]) < 1.0e-16):
                sampleCovariance[i, j] = 0.0
    print("covariance=", sampleCovariance)

    # Define a point
    point = Point(dim, 0.0)
    print("Point= ", point)

    # Show PDF and CDF of point
    PDF = distribution.computePDF(point)
    print("pdf     =", PDF)
    print("pdf (FD)=", (distribution.computeCDF(point + Point(dim, 0)) -
                        distribution.computeCDF(point + Point(dim, -1))))
    CDF = distribution.computeCDF(point)
    print("cdf=", CDF)

    # Define a point
    point = Point(distribution.getSupport(distribution.getRange())[0])
    print("Point= ", point)

    # Show PDF and CDF of point
    PDF = distribution.computePDF(point)
    print("pdf     =", PDF)
    print("pdf (FD)=", (distribution.computeCDF(point + Point(dim, 0)) -
                        distribution.computeCDF(point + Point(dim, -1))))
    CDF = distribution.computeCDF(point)
    print("cdf=", CDF)

    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=", distribution.computeCDF(quantile))
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
    parameters = distribution.getParametersCollection()
    print("parameters=", parameters)
    for i in range(6):
        print("standard moment n=", i, " value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())

except:
    import sys
    print("t_Dirac.py", sys.exc_info()[0], sys.exc_info()[1])
