#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # Instanciate one distribution object
    distribution = Multinomial(5, NumericalPoint(3, 0.25))
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
    print("oneSample first=", repr(oneSample[0]), " last=", repr(oneSample[1]))
    print("mean=", repr(oneSample.computeMean()))
    print("covariance=", repr(oneSample.computeCovariance()))

    print("support=\n" + str(distribution.getSupport()))
    interval = Interval(NumericalPoint(distribution.getDimension(), 1.0),
                        NumericalPoint(distribution.getDimension(), 3.0))
    print("support restricted to the interval=\n" + str(interval) +
          " gives=\n" + str(distribution.getSupport(interval)))

    # Define a point
    point = NumericalPoint(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # Show PDF and CDF at point
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    CDF = distribution.computeCDF(point)
    print("cdf=%.5f" % CDF)
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)= %.6f" % distribution.computeCDF(quantile))
    mean = distribution.getMean()

    print("mean=", repr(mean))
    covariance = distribution.getCovariance()
    print("covariance=", repr(covariance))
    parameters = distribution.getParametersCollection()
    print("parameters=", repr(parameters))

except:
    import sys
    print("t_Multinomial.py", sys.exc_info()[0], sys.exc_info()[1])
