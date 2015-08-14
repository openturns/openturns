#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    dimension = 3
    meanPoint = NumericalPoint(dimension, 1.0)
    meanPoint[0] = 0.5
    meanPoint[1] = -0.5
    sigma = NumericalPoint(dimension, 1.0)
    sigma[0] = 2.0
    sigma[1] = 3.0

    sample = NumericalSample(0, dimension)
    # Create a collection of distribution
    aCollection = DistributionCollection()

    aCollection.add(Normal(meanPoint, sigma, IdentityMatrix(dimension)))
    sample.add(meanPoint)
    meanPoint += NumericalPoint(meanPoint.getDimension(), 1.0)
    aCollection.add(Normal(meanPoint, sigma, IdentityMatrix(dimension)))
    sample.add(meanPoint)
    meanPoint += NumericalPoint(meanPoint.getDimension(), 1.0)
    aCollection.add(Normal(meanPoint, sigma, IdentityMatrix(dimension)))
    sample.add(meanPoint)

    # Instanciate one distribution object
    distribution = KernelMixture(Normal(), sigma, sample)
    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)
    distributionRef = Mixture(aCollection)

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", repr(oneRealization))

    # Test for sampling
    size = 100
    oneSample = distribution.getSample(size)
    print("oneSample first=", repr(
        oneSample[0]), " last=", repr(oneSample[size - 1]))
    print("mean=", repr(oneSample.computeMean()))
    print("covariance=", repr(oneSample.computeCovariance()))

    # Define a point
    point = NumericalPoint(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    eps = 1e-5

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(DDF))
    print("ddf (ref)=", repr(distributionRef.computeDDF(point)))
    # by the finite difference technique
    ddfFD = NumericalPoint(dimension)
    for i in range(dimension):
        left = NumericalPoint(point)
        left[i] += eps
        right = NumericalPoint(point)
        right[i] -= eps
        ddfFD[i] = (distribution.computePDF(left) -
                    distribution.computePDF(right)) / (2.0 * eps)
    print("ddf (FD)=", repr(ddfFD))

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    print("pdf (ref)=%.6f" % distributionRef.computePDF(point))
    # by the finite difference technique from CDF
    if (dimension == 1):
        print("pdf (FD)=%.6f" % ((distribution.computeCDF(point + NumericalPoint(1, eps)) -
                                  distribution.computeCDF(point + NumericalPoint(1, -eps))) / (2.0 * eps)))

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    print("cdf (ref)=%.6f" % distributionRef.computeCDF(point))

    # quantile
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("quantile (ref)=", repr(distributionRef.computeQuantile(0.95)))
    print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
    mean = distribution.getMean()
    print("mean=", repr(mean))
    print("mean (ref)=", repr(distributionRef.getMean()))
    covariance = distribution.getCovariance()
    print("covariance=", repr(covariance))
    print("covariance (ref)=", repr(distributionRef.getCovariance()))
    #parameters = distribution.getParametersCollection()
    # print "parameters=" , parameters

except:
    import sys
    print("t_KernelMixture_std.py", sys.exc_info()[0], sys.exc_info()[1])
