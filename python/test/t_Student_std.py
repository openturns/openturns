#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    distribution = Student(6.5, -0.5, 2.0)
    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", repr(oneRealization))

    # Test for sampling
    size = 10000
    oneSample = distribution.getSample(size)
    print("oneSample first=", repr(
        oneSample[0]), " last=", repr(oneSample[size - 1]))
    print("mean=", repr(oneSample.computeMean()))
    print("covariance=", repr(oneSample.computeCovariance()))

    size = 100
    for i in range(2):
        msg = ''
        if FittingTest.Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure():
            msg = "accepted"
        else:
            msg = "rejected"
        print(
            "Kolmogorov test for the generator, sample size=", size, " is", msg)
        size *= 10

    # Define a point
    point = NumericalPoint(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    eps = 1e-5

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(DDF))
    # by the finite difference technique
    print("ddf (FD)=", repr(NumericalPoint(1, (distribution.computePDF(
        point + NumericalPoint(1, eps)) - distribution.computePDF(point + NumericalPoint(1, -eps))) / (2.0 * eps))))

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    # by the finite difference technique from CDF
    print("pdf (FD)=%.6f" % ((distribution.computeCDF(point + NumericalPoint(1, eps)) -
                              distribution.computeCDF(point + NumericalPoint(1, -eps))) / (2.0 * eps)))

    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    PDFgr = distribution.computePDFGradient(point)
    print("pdf gradient     =", repr(PDFgr))
    # by the finite difference technique
    PDFgrFD = NumericalPoint(3)
    PDFgrFD[0] = (Student(distribution.getNu() + eps, distribution.getMu(), distribution.getSigma()[0]).computePDF(point) -
                  Student(distribution.getNu() - eps, distribution.getMu(), distribution.getSigma()[0]).computePDF(point)) / (2.0 * eps)
    PDFgrFD[1] = (Student(distribution.getNu(), distribution.getMu() + eps, distribution.getSigma()[0]).computePDF(point) -
                  Student(distribution.getNu(), distribution.getMu() - eps, distribution.getSigma()[0]).computePDF(point)) / (2.0 * eps)
    PDFgrFD[2] = (Student(distribution.getNu(), distribution.getMu(), distribution.getSigma()[0] + eps).computePDF(point) -
                  Student(distribution.getNu(), distribution.getMu(), distribution.getSigma()[0] - eps).computePDF(point)) / (2.0 * eps)
    print("pdf gradient (FD)=", repr(PDFgrFD))

    # derivative of the PDF with regards the parameters of the distribution
    CDFgr = distribution.computeCDFGradient(point)
    print("cdf gradient     =", repr(CDFgr))
    CDFgrFD = NumericalPoint(3)
    CDFgrFD[0] = (Student(distribution.getNu() + eps, distribution.getMu(), distribution.getSigma()[0]).computeCDF(point) -
                  Student(distribution.getNu() - eps, distribution.getMu(), distribution.getSigma()[0]).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[1] = (Student(distribution.getNu(), distribution.getMu() + eps, distribution.getSigma()[0]).computeCDF(point) -
                  Student(distribution.getNu(), distribution.getMu() - eps, distribution.getSigma()[0]).computeCDF(point)) / (2.0 * eps)
    CDFgrFD[2] = (Student(distribution.getNu(), distribution.getMu(), distribution.getSigma()[0] + eps).computeCDF(point) -
                  Student(distribution.getNu(), distribution.getMu(), distribution.getSigma()[0] - eps).computeCDF(point)) / (2.0 * eps)

    print("cdf gradient (FD)=",  repr(CDFgrFD))

    # quantile
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = NumericalPoint(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" % distribution.computeSurvivalFunction(inverseSurvival))

    # Confidence regions
    interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", NumericalPoint(1, threshold))
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", NumericalPoint(1, beta))
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", NumericalPoint(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", NumericalPoint(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", NumericalPoint(1, beta))

    mean = distribution.getMean()
    print("mean=", repr(mean))
    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", repr(standardDeviation))
    skewness = distribution.getSkewness()
    print("skewness=", repr(skewness))
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", repr(kurtosis))
    covariance = distribution.getCovariance()
    print("covariance=", repr(covariance))
    parameters = distribution.getParametersCollection()
    print("parameters=", repr(parameters))
    for i in range(6):
        print("standard moment n=", i, " value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())

    # Specific to this distribution
    beta = point.normSquare()
    densityGenerator = distribution.computeDensityGenerator(beta)
    print("density generator=%.6f" % densityGenerator)
    print("pdf via density generator=%.6f" %
          EllipticalDistribution.computePDF(distribution, point))
    densityGeneratorDerivative = distribution.computeDensityGeneratorDerivative(
        beta)
    print("density generator derivative     =%.6f" %
          densityGeneratorDerivative)
    print("density generator derivative (FD)=%.6f" % ((distribution.computeDensityGenerator(
        beta + eps) - distribution.computeDensityGenerator(beta - eps)) / (2.0 * eps)))
    densityGeneratorSecondDerivative = distribution.computeDensityGeneratorSecondDerivative(
        beta)
    print("density generator second derivative     =%.6f" %
          densityGeneratorSecondDerivative)
    print("density generator second derivative (FD)=%.6f" % ((distribution.computeDensityGeneratorDerivative(
        beta + eps) - distribution.computeDensityGeneratorDerivative(beta - eps)) / (2.0 * eps)))

except:
    import sys
    print("t_Student_std.py", sys.exc_info()[0], sys.exc_info()[1])
