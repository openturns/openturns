#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    distribution = Skellam(10.0, 5.0)
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
    point = Point(distribution.getDimension(), 12.0)
    print("Point= ", point)

    # Show PDF and CDF of point
    eps = 1e-5
    LPDF = distribution.computeLogPDF(point)
    print("log pdf= %.12g" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     = %.12g" % PDF)
    print("pdf (FD)= %.12g" % (distribution.computeCDF(
        point + Point(1, 0)) - distribution.computeCDF(point + Point(1, -1))))
    CDF = distribution.computeCDF(point)
    print("cdf= %.12g" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf= %.12g" % CCDF)
    CF = distribution.computeCharacteristicFunction(point[0])
    print("characteristic function= (%.12g%+.12gj)" % (CF.real, CF.imag))
    LCF = distribution.computeLogCharacteristicFunction(point[0])
    print("log characteristic function= (%.12g%+.12gj)" % (LCF.real, LCF.imag))
    GF = distribution.computeGeneratingFunction(0.3 + 0.7j)
    print("generating function= (%.12g%+.12gj)" % (GF.real, GF.imag))
    LGF = distribution.computeLogGeneratingFunction(0.3 + 0.7j)
    print("log generating function= (%.12g%+.12gj)" % (LGF.real, LGF.imag))
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
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
        print("standard moment n=", i, ", value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())

except:
    import sys
    print("t_Skellam_std.py", sys.exc_info()[0], sys.exc_info()[1])
