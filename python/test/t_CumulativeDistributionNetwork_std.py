#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    graph = ot.BipartiteGraph([[0, 1], [0, 1]])
    distribution = ot.CumulativeDistributionNetwork([ot.Normal(2)]*2, graph)
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
    oneSample = distribution.getSample( size )
    print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
    print("mean=", oneSample.computeMean())
    print("covariance=", oneSample.computeCovariance())

    # Define a point
    point = [1.0]*distribution.getDimension()
    print("Point= ", point)

    # Show PDF and CDF of point
    LPDF = distribution.computeLogPDF( point )
    print("log pdf =%.4e" % LPDF)
    PDF = distribution.computePDF( point )
    print("pdf     =%.4e" % PDF)
    CDF = distribution.computeCDF( point )
    print("cdf     =%.4e" % CDF)
    CCDF = distribution.computeComplementaryCDF( point )
    print("ccdf    =%.4e" % CCDF)
    Survival = distribution.computeSurvivalFunction( point )
    print("survival=%.4e" % Survival)
    quantile = distribution.computeQuantile( 0.95 )
    print("quantile=", quantile)
    print("cdf(quantile)= %.12g" % distribution.computeCDF(quantile))
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
    correlation = distribution.getCorrelation()
    print("correlation=", correlation)
    spearman = distribution.getSpearmanCorrelation()
    print("spearman=", spearman)
    kendall = distribution.getKendallTau()
    print("kendall=", kendall)

except:
    import sys
    print("t_CumulativeDistributionNetwork_std.py", sys.exc_info()[0], sys.exc_info()[1])
