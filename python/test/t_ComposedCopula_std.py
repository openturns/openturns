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
    point = NumericalPoint(copula.getDimension(), 0.6)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    # NumericalScalar eps(1e-5)
    DDF = copula.computeDDF(point)
    print("ddf     =", repr(DDF))
    PDF = copula.computePDF(point)
    print("pdf     =%.6f" % PDF)
    CDF = copula.computeCDF(point)
    print("cdf=%.6f" % CDF)
    #    NumericalPoint PDFgr = copula.computePDFGradient( point )
    #    print "pdf gradient     =", PDFgr
    #    NumericalPoint CDFgr = copula.computeCDFGradient( point )
    #    print "cdf gradient     =", CDFgr
    quantile = copula.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)=%.6f" % copula.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = NumericalPoint(copula.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" % copula.computeSurvivalFunction(inverseSurvival))
    mean = copula.getMean()
    print("mean=", repr(mean))
    covariance = copula.getCovariance()
    print("covariance=", repr(covariance))
    parameters = copula.getParametersCollection()
    print("parameters=", repr(parameters))

    # Specific to this copula

    # Extract a 5-D marginal
    dim = 5
    point = NumericalPoint(dim, 0.25)
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
        3.0, NumericalPoint(2, 1.0), NumericalPoint(2, 3.0), CorrelationMatrix(2)))
    copula = ComposedCopula(collection)
    print("isoprobabilistic transformation (general non-normal)=",
          copula.getIsoProbabilisticTransformation())
    # Special case, independent copula
    collection[0] = SklarCopula(Normal(2))
    collection[1] = IndependentCopula(2)
    collection[2] = NormalCopula(CorrelationMatrix(2))
    copula = ComposedCopula(collection)
    print("isoprobabilistic transformation (independent)=",
          copula.getIsoProbabilisticTransformation())
    # Special case, single contributor
    collection = [SklarCopula(Student(
        3.0, NumericalPoint(2, 1.0), NumericalPoint(2, 3.0), CorrelationMatrix(2)))]
    copula = ComposedCopula(collection)
    print("isoprobabilistic transformation (single contributor)=",
          copula.getIsoProbabilisticTransformation())

    # test CopulaCollection
    copulas = CopulaCollection(
        [FrankCopula(3.0), NormalCopula(R), ClaytonCopula(2.0)])
    test_distribution = ComposedDistribution([Normal(), Normal()])
    sample_size = 10
    sample = test_distribution.getSample(sample_size)
    myEstimatedDist = KernelSmoothing().build(sample)
    sklarCopula = myEstimatedDist.getCopula()
    copulas.add(sklarCopula)

except:
    import sys
    print("t_ComposedCopula.py", sys.exc_info()[0], sys.exc_info()[1])
