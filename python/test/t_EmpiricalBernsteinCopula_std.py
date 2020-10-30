#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    dim = 2
    copula = EmpiricalBernsteinCopula(Normal(2).getSample(12), 3)
    print("Copula ", repr(copula))
    print("Copula ", copula)
    print("Mean ", repr(copula.getMean()))
    print("Covariance ", repr(copula.getCovariance()))

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
    size = 10000
    anotherSample = copula.getSample(size)
    print("anotherSample mean=", repr(anotherSample.computeMean()))
    print("anotherSample covariance=", repr(anotherSample.computeCovariance()))

    # Define a point
    point = Point(dim, 0.2)

    # Show PDF and CDF of point
    pointPDF = copula.computePDF(point)
    pointCDF = copula.computeCDF(point)
    print("Point = ", repr(point), " pdf=%.6f" %
          pointPDF, " cdf=%.6f" % pointCDF)

    # Get 50% quantile
    quantile = copula.computeQuantile(0.5)
    print("Quantile=", repr(quantile))
    print("CDF(quantile)=%.6f" % copula.computeCDF(quantile))

    # Get 5% quantile
    quantile = copula.computeQuantile(0.95, True)
    print("Quantile=", repr(quantile))

    # Get 95% survival function
    inverseSurvival = Point(copula.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" %
          copula.computeSurvivalFunction(inverseSurvival))
    print("entropy=%.6f" % copula.computeEntropy())

    # Confidence regions
    interval, threshold = copula.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", Point(1, threshold))
    levelSet, beta = copula.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", Point(1, beta))
    interval, beta = copula.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", Point(1, beta))
    interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", Point(1, beta))
    interval, beta = copula.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", Point(1, beta))
    print("parameters=", copula.getParameter())
    copula.setParameter(copula.getParameter())

    # Extract the marginals
    for i in range(dim):
        margin = copula.getMarginal(i)
        print("margin=", repr(margin))
        print("margin PDF=%.6f" % margin.computePDF(Point(1, 0.25)))
        print("margin CDF=%.6f" % margin.computeCDF(Point(1, 0.25)))
        print("margin quantile=", repr(margin.computeQuantile(0.95)))
        print("margin realization=", repr(margin.getRealization()))

    # Extract a 2-D marginal
    indices = Indices(2, 0)
    indices[0] = 1
    indices[1] = 0
    print("indices=", repr(indices))
    margins = copula.getMarginal(indices)
    print("margins=", repr(margins))
    print("margins PDF=%.6f" % margins.computePDF(Point(2, 0.25)))
    print("margins CDF=%.6f" % margins.computeCDF(Point(2, 0.25)))
    quantile = Point(margins.computeQuantile(0.95))
    print("margins quantile=", repr(quantile))
    print("margins CDF(qantile)=%.6f" % margins.computeCDF(quantile))
    print("margins realization=", repr(margins.getRealization()))

    copula6D = EmpiricalBernsteinCopula(Normal(6).getSample(8), 4)
    print("Entropy in higher dimension=%.6f" % copula6D.computeEntropy())

    dim = 6
    x = 0.6
    y = [0.2]*(dim-1)
    print("conditional PDF=%.6f" %
          copula6D.computeConditionalPDF(x, y))
    print("conditional PDF ref=%.6f" %
          (copula6D.computePDF(y + [x]) / copula6D.getMarginal([0,1,2,3,4,]).computePDF(y)))
    print("conditional CDF=%.6f" %
          copula6D.computeConditionalCDF(x, y))
    print("conditional quantile=%.6f" %
          copula6D.computeConditionalQuantile(x, y))
    pt = Point([0.05*(1+i) for i in range(dim)])
    print("sequential conditional PDF=",
          copula6D.computeSequentialConditionalPDF(pt))
    resCDF = copula6D.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print("sequential conditional quantile(", resCDF, ")=",
          copula6D.computeSequentialConditionalQuantile(resCDF))

except:
    import sys
    print("t_EmpiricalBernsteinCopula_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
