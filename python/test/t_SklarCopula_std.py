#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    dim = 3
    R = CorrelationMatrix(dim)
    for i in range(dim - 1):
        R[i, i + 1] = 0.25
    copula = SklarCopula(
        Distribution(Normal([1.0, 2.0, 3.0], [2.0, 3.0, 1.0], R)))
    copulaRef = NormalCopula(R)
    print("Copula ", repr(copula))
    print("Copula ", copula)
    print("Mean      =", repr(copula.getMean()))
    print("Mean (ref)=", repr(copulaRef.getMean()))
    ResourceMap.SetAsUnsignedInteger("GaussKronrod-MaximumSubIntervals", 20)
    ResourceMap.SetAsScalar("GaussKronrod-MaximumError",  1.0e-4)
    print("Covariance      =", repr(copula.getCovariance()))
    ResourceMap.SetAsUnsignedInteger("GaussKronrod-MaximumSubIntervals", 100)
    ResourceMap.SetAsScalar("GaussKronrod-MaximumError",  1.0e-12)
    print("Covariance (ref)=", repr(copulaRef.getCovariance()))

    # Is this copula an elliptical distribution?
    print("Elliptical distribution= ", copula.isElliptical())

    # Is this copula elliptical ?
    print("Elliptical copula= ", copula.hasEllipticalCopula())

    # Is this copula independent ?
    print("Independent copula= ", copula.hasIndependentCopula())

    # Test for realization of distribution
    oneRealization = copula.getRealization()
    print("oneRealization=", repr(oneRealization))

    # Test for sampling
    size = 10
    oneSample = copula.getSample(size)
    print("oneSample=", repr(oneSample))

   # Test for sampling
    size = 1000
    anotherSample = copula.getSample(size)
    print("anotherSample mean=", repr(anotherSample.computeMean()))
    print("anotherSample covariance=", repr(anotherSample.computeCovariance()))

    # Define a point
    point = Point(dim, 0.2)

    # Show PDF and CDF of point
    pointPDF = copula.computePDF(point)
    pointCDF = copula.computeCDF(point)
    pointPDFRef = copulaRef.computePDF(point)
    pointCDFRef = copulaRef.computeCDF(point)
    print("Point = ", repr(point), " pdf      =%.6f" %
          pointPDF, " cdf      =%.6f" % pointCDF)
    print("Point = ", repr(point), " pdf (ref)=%.6f" %
          pointPDFRef, " cdf (ref)=%.6f" % pointCDFRef)

    # Get 50% quantile
    quantile = copula.computeQuantile(0.5)
    quantileRef = copulaRef.computeQuantile(0.5)
    print("Quantile      =", repr(quantile))
    print("Quantile (ref)=", repr(quantileRef))
    print("CDF(quantile)=%.6f" % copula.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = Point(copula.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" %
          copula.computeSurvivalFunction(inverseSurvival))
    # Takes too much time
    # print("entropy=%.6f" % copula.computeEntropy())
    # Confidence regions
    if copula.getDimension() <= 2:
        threshold = Point()
        print("Minimum volume interval=",
              copula.computeMinimumVolumeInterval(0.95, threshold))
        print("threshold=", threshold)
        beta = Point()
        levelSet = copula.computeMinimumVolumeLevelSet(0.95, beta)
        print("Minimum volume level set=", levelSet)
        print("beta=", beta)
        print("Bilateral confidence interval=",
              copula.computeBilateralConfidenceInterval(0.95, beta))
        print("beta=", beta)
        print("Unilateral confidence interval (lower tail)=",
              copula.computeUnilateralConfidenceInterval(0.95, False, beta))
        print("beta=", beta)
        print("Unilateral confidence interval (upper tail)=",
              copula.computeUnilateralConfidenceInterval(0.95, True, beta))
        print("beta=", beta)

    # Extract the marginals
    for i in range(dim):
        margin = copula.getMarginal(i)
        marginRef = copulaRef.getMarginal(i)
        print("margin=", repr(margin))
        print("margin PDF      =%.6f" %
              margin.computePDF(Point(1, 0.25)))
        print("margin PDF (ref)=%.6f" %
              marginRef.computePDF(Point(1, 0.25)))
        print("margin CDF      =%.6f" %
              margin.computeCDF(Point(1, 0.25)))
        print("margin CDF (ref)=%.6f" %
              marginRef.computeCDF(Point(1, 0.25)))
        print("margin quantile      =", repr(margin.computeQuantile(0.95)))
        print("margin quantile (ref)=", repr(marginRef.computeQuantile(0.95)))
        print("margin realization=", repr(margin.getRealization()))

    # Extract a 2-D marginal
    indices = Indices(2, 0)
    indices[0] = 1
    indices[1] = 0
    print("indices=", repr(indices))
    margin = copula.getMarginal(indices)
    marginRef = copulaRef.getMarginal(indices)
    print("margin=", repr(margin))
    print("margin PDF      =%.6f" % margin.computePDF(Point(2, 0.25)))
    print("margin PDF (ref)=%.6f" %
          marginRef.computePDF(Point(2, 0.25)))
    print("margin CDF      =%.6f" % margin.computeCDF(Point(2, 0.25)))
    print("margin CDF (ref)=%.6f" %
          marginRef.computeCDF(Point(2, 0.25)))
    print("margin quantile      =", repr(margin.computeQuantile(0.95)))
    print("margin quantile (ref)=", repr(marginRef.computeQuantile(0.95)))
    print("margin realization=", repr(margin.getRealization()))

except:
    import sys
    print("t_SklarCopula.py", sys.exc_info()[0], sys.exc_info()[1])
