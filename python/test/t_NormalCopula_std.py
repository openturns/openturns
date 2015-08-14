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
    point = NumericalPoint(dim, 0.2)

    # Show PDF and CDF of point
    pointPDF = copula.computePDF(point)
    pointCDF = copula.computeCDF(point)
    print("Point = ", repr(point), " pdf=%.6f" %
          pointPDF, " cdf=%.6f" % pointCDF)

    # Get 50% quantile
    quantile = copula.computeQuantile(0.5)
    print("Quantile=", repr(quantile))
    print("CDF(quantile)=%.6f" % copula.computeCDF(quantile))

    # Extract the marginals
    for i in range(dim):
        margin = copula.getMarginal(i)
        print("margin=", repr(margin))
        print("margin PDF=%.6f" % margin.computePDF(NumericalPoint(1, 0.25)))
        print("margin CDF=%.6f" % margin.computeCDF(NumericalPoint(1, 0.25)))
        print("margin quantile=", repr(margin.computeQuantile(0.95)))
        print("margin realization=", repr(margin.getRealization()))

    # Extract a 2-D marginal
    indices = Indices(2, 0)
    indices[0] = 1
    indices[1] = 0
    print("indices=", repr(indices))
    margins = copula.getMarginal(indices)
    print("margins=", repr(margins))
    print("margins PDF=%.6f" % margins.computePDF(NumericalPoint(2, 0.25)))
    print("margins CDF=%.6f" % margins.computeCDF(NumericalPoint(2, 0.25)))
    quantile = NumericalPoint(margins.computeQuantile(0.95))
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

except:
    import sys
    print("t_NormalCopula_std.py", sys.exc_info()[0], sys.exc_info()[1])
