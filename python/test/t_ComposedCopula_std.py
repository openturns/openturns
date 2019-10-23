#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    R = CorrelationMatrix(3)
    R[0, 1] = 0.5
    R[0, 2] = 0.25
    collection = [FrankCopula(3.0), NormalCopula(R), ClaytonCopula(2.0)]
    copula = ComposedCopula(collection)

    print("Copula ", repr(copula))
    print("Copula ", copula)

    # Is this copula elliptical ?
    print("Elliptical distribution= ", copula.isElliptical())

    # Is this copula continuous ?
    print("Continuous = ", copula.isContinuous())

    # Is this copula elliptical ?
    print("Elliptical = ", copula.hasEllipticalCopula())

    # Is this copula independent ?
    print("Independent = ", copula.hasIndependentCopula())

    # Test for realization of copula
    oneRealization = copula.getRealization()
    print("oneRealization=", repr(oneRealization))

    # Test for sampling
    size = 10000
    oneSample = copula.getSample(size)
    print("oneSample first=", repr(
        oneSample[0]), " last=", repr(oneSample[size - 1]))
    print("mean=", repr(oneSample.computeMean()))
    print("covariance=", repr(oneSample.computeCovariance()))

    # Define a point
    point = Point(copula.getDimension(), 0.6)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    # Scalar eps(1e-5)
    DDF = copula.computeDDF(point)
    print("ddf     =", repr(DDF))
    PDF = copula.computePDF(point)
    print("pdf     =%.6f" % PDF)
    CDF = copula.computeCDF(point)
    print("cdf=%.6f" % CDF)
    #    Point PDFgr = copula.computePDFGradient( point )
    #    print "pdf gradient     =", PDFgr
    #    Point CDFgr = copula.computeCDFGradient( point )
    #    print "cdf gradient     =", CDFgr
    quantile = copula.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)=%.6f" % copula.computeCDF(quantile))
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

    mean = copula.getMean()
    print("mean=", repr(mean))
    covariance = copula.getCovariance()
    print("covariance=", repr(covariance))
    parameters = copula.getParametersCollection()
    print("parameters=", repr(parameters))
    # Covariance and correlation
    precision = PlatformInfo.GetNumericalPrecision()
    PlatformInfo.SetNumericalPrecision(4)
    covariance = copula.getCovariance()
    print("covariance=", covariance)
    correlation = copula.getCorrelation()
    print("correlation=", correlation)
    spearman = copula.getSpearmanCorrelation()
    print("spearman=", spearman)
    kendall = copula.getKendallTau()
    print("kendall=", kendall)
    PlatformInfo.SetNumericalPrecision(precision)
    dim = copula.getDimension()
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

    # Specific to this copula

    # Extract a 5-D marginal
    dim = 5
    point = Point(dim, 0.25)
    indices = Indices(dim, 0)
    indices[0] = 1
    indices[1] = 2
    indices[2] = 3
    indices[3] = 5
    indices[4] = 6
    print("indices=", repr(indices))
    margins = copula.getMarginal(indices)
    print("margins=", repr(margins))
    print("margins PDF=%.6f" % margins.computePDF(point))
    print("margins CDF=%.6f" % margins.computeCDF(point))
    quantile = margins.computeQuantile(0.95)
    print("margins quantile=", repr(quantile))
    print("margins CDF(quantile)=%.6f" % margins.computeCDF(quantile))
    print("margins realization=", repr(margins.getRealization()))
    # Tests the isoprobabilistic transformation
    # General case with normal standard distribution
    print("isoprobabilistic transformation (general normal)=",
          copula.getIsoProbabilisticTransformation())
    # General case with non-normal standard distribution
    collection[0] = SklarCopula(Student(
        3.0, Point(2, 1.0), Point(2, 3.0), CorrelationMatrix(2)))
    copula = ComposedCopula(collection)
    print("isoprobabilistic transformation (general non-normal)=",
          copula.getIsoProbabilisticTransformation())
    dim = copula.getDimension()
    x = 0.6
    y = [0.2]*(dim-1)
    print("conditional PDF=%.6f" % copula.computeConditionalPDF(x, y))
    print("conditional CDF=%.6f" % copula.computeConditionalCDF(x, y))
    print("conditional quantile=%.6f" %
          copula.computeConditionalQuantile(x, y))
    pt = Point([0.1 * i + 0.05 for i in range(dim)])
    print("sequential conditional PDF=",
          copula.computeSequentialConditionalPDF(pt))
    resCDF = copula.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print("sequential conditional quantile(", resCDF, ")=",
          copula.computeSequentialConditionalQuantile(resCDF))
    # Special case, independent copula
    collection[0] = SklarCopula(Normal(2))
    collection[1] = IndependentCopula(2)
    collection[2] = NormalCopula(CorrelationMatrix(2))
    copula = ComposedCopula(collection)
    print("isoprobabilistic transformation (independent)=",
          copula.getIsoProbabilisticTransformation())
    dim = copula.getDimension()
    x = 0.6
    y = [0.2]*(dim-1)
    print("conditional PDF=%.6f" % copula.computeConditionalPDF(x, y))
    print("conditional CDF=%.6f" % copula.computeConditionalCDF(x, y))
    print("conditional quantile=%.6f" %
          copula.computeConditionalQuantile(x, y))
    pt = Point([0.1 * i + 0.05 for i in range(dim)])
    print("sequential conditional PDF=",
          copula.computeSequentialConditionalPDF(pt))
    resCDF = copula.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print("sequential conditional quantile(", resCDF, ")=",
          copula.computeSequentialConditionalQuantile(resCDF))
    # Special case, single contributor
    collection = [SklarCopula(Student(
        3.0, Point(2, 1.0), Point(2, 3.0), CorrelationMatrix(2)))]
    copula = ComposedCopula(collection)
    print("isoprobabilistic transformation (single contributor)=",
          copula.getIsoProbabilisticTransformation())
    dim = copula.getDimension()
    x = 0.6
    y = [0.2]*(dim-1)
    print("conditional PDF=%.6f" % copula.computeConditionalPDF(x, y))
    print("conditional CDF=%.6f" % copula.computeConditionalCDF(x, y))
    print("conditional quantile=%.6f" %
          copula.computeConditionalQuantile(x, y))
    pt = Point([0.1 * i + 0.05 for i in range(dim)])
    print("sequential conditional PDF=",
          copula.computeSequentialConditionalPDF(pt))
    resCDF = copula.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print("sequential conditional quantile(", resCDF, ")=",
          copula.computeSequentialConditionalQuantile(resCDF))

    # test ComposedCopula.getMarginal in reverse
    copula = ComposedCopula([IndependentCopula(2), NormalCopula(2)])
    print(copula.getMarginal([3, 2, 1, 0]))

except:
    import sys
    print("t_ComposedCopula.py", sys.exc_info()[0], sys.exc_info()[1])
