#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    meanPoint = Point(1)
    meanPoint[0] = 1.0
    sigma = Point(1)
    sigma[0] = 3.0
    R = CorrelationMatrix(1)
    R[0, 0] = 1.0

# Create a collection of distribution
    dimension = 2000
    print("Creating a composed distribution of dimension ", dimension)
    aCollection = DistributionCollection(
        dimension, Normal(meanPoint, sigma, R))
    for i in range(dimension):
        aCollection[i] = Normal(meanPoint, sigma, R)

# Create a a copula
    aCopula = IndependentCopula(dimension)

# Instanciate one distribution object
    distribution = ComposedDistribution(aCollection, aCopula)
    print("Distribution created.")

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Has this distribution an elliptical copula?
    print("Elliptical copula = ", distribution.hasEllipticalCopula())

    # Has this distribution an independent copula?
    print("Independent copula = ",  distribution.hasIndependentCopula())

    # Test for sampling
    size = 10
    anotherSample = distribution.getSample(size)

    # Define a point
    zero = Point(dimension, 0.0)

    # Show PDF and CDF of zero point
    zeroPDF = distribution.computePDF(zero)
    zeroCDF = distribution.computeCDF(zero)
    print(" pdf=%.6f" % zeroPDF, " cdf=%.6f" % zeroCDF)

    # Get 95% quantile
    quantile = distribution.computeQuantile(0.95)
    print("Quantile=", repr(quantile))
    print("CDF(quantile)=%.6f" % distribution.computeCDF(quantile))

    # Extract a 2-D marginal
    indices = Indices(2, 0)
    indices[0] = 1
    indices[1] = 0
    print("indices=", repr(indices))
    margins = distribution.getMarginal(indices)
    print("margins=", repr(margins))
    print("margins PDF=%.6f" % margins.computePDF(Point(2)))
    print("margins CDF=%.6f" % margins.computeCDF(Point(2)))
    quantile = Point(margins.computeQuantile(0.5))
    print("margins quantile=", repr(quantile))
    print("margins CDF(qantile)=%.6f" % margins.computeCDF(quantile))
    print("margins realization=", repr(margins.getRealization()))
    sample = margins.getSample(1000)
    print("margins sample mean=", repr(sample.computeMean()))
    print("margins sample covariance=", repr(sample.computeCovariance()))

except:
    import sys
    print("t_ComposedDistribution_large.py",
          sys.exc_info()[0], sys.exc_info()[1])
