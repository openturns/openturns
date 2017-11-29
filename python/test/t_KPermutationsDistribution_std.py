#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    distribution = KPermutationsDistribution(5, 12)
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
    point = Point(distribution.getDimension(), 4.0)
    print("Point= ", point)

    # Show PDF and CDF of point
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=", LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =", PDF)
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=", distribution.computeCDF(quantile))
    print("entropy=%.6f" % distribution.computeEntropy())
    mean = distribution.getMean()
    print("mean=", mean)
    covariance = distribution.getCovariance()
    print("covariance=", covariance)
    parameters = distribution.getParametersCollection()
    print("parameters=", parameters)

except:
    import sys
    print("t_KPermutationsDistribution_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
