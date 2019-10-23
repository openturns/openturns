#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    dim = 3
    R = CorrelationMatrix(dim)
    for i in range(dim - 1):
        R[i, i + 1] = 0.25

    copula = NormalCopula(R)
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
    # Get 95% survival function
    inverseSurvival = Point(copula.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" %
          copula.computeSurvivalFunction(inverseSurvival))
    print("entropy=%.6f" % copula.computeEntropy())
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

    # Covariance and correlation
    covariance = copula.getCovariance()
    print("covariance=", covariance)
    correlation = copula.getCorrelation()
    print("correlation=", correlation)
    spearman = copula.getSpearmanCorrelation()
    print("spearman=", spearman)
    kendall = copula.getKendallTau()
    print("kendall=", kendall)
    x = 0.6
    y = [0.2]*(dim-1)
    print("conditional PDF=%.6f" % copula.computeConditionalPDF(x, y))
    print("conditional CDF=%.6f" % copula.computeConditionalCDF(x, y))
    print("conditional quantile=%.6f" %
          copula.computeConditionalQuantile(x, y))
    pt = Point([0.1 * i + 0.05 for i in range(dim)])
    print("sequential conditional PDF=",
          copula.computeSequentialConditionalPDF(point))
    resCDF = copula.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print("sequential conditional quantile(", resCDF, ")=",
          copula.computeSequentialConditionalQuantile(resCDF))

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

    # Creation of the correlation matrix from a Spearman correlation matrix
    spearman = CorrelationMatrix(dim)
    for i in range(1, dim):
        spearman[i, i - 1] = 0.25

    correlation = NormalCopula.GetCorrelationFromSpearmanCorrelation(spearman)
    print("Normal copula correlation=", repr(correlation),
          " from the Spearman correlation=", repr(spearman))

    # computeProbability
    spearman_corr = CorrelationMatrix([[1., .74], [.74, 1.]])
    copula = NormalCopula(
        NormalCopula.GetCorrelationFromSpearmanCorrelation(spearman_corr))
    interval = Interval([0.958722, 0.902063], [1., 1.])
    prob = copula.computeProbability(interval)
    print("prob=%.6f" % prob)

    print(NormalCopula(1).getParametersCollection())
except:
    import sys
    print("t_NormalCopula_std.py", sys.exc_info()[0], sys.exc_info()[1])
