#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    allDistributions = [Student(6.5, -0.5, 2.0)]
    dim = 2
    R = CorrelationMatrix(dim)
    mu = list()
    sigma = list()
    for i in range(dim):
        mu.append(i)
        sigma.append((1.0 + i) / dim)
        for j in range(i):
            R[i, j] = 1.0 / (1.0 + dim + i + j)
    allDistributions.append(Student(7.5, mu, sigma, R))
    for distribution in allDistributions:
        dim = distribution.getDimension()
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

        if dim == 1:
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
        point = Point(distribution.getDimension(), 1.0)
        print("Point= ", repr(point))

        # Show PDF and CDF of point
        eps = 1e-5

        # derivative of PDF with regards its arguments
        DDF = distribution.computeDDF(point)
        print("ddf     =", repr(DDF))
        # by the finite difference technique
        if dim == 1:
            print("ddf (FD)=", repr(Point(1, (distribution.computePDF(
                point + Point(1, eps)) - distribution.computePDF(point + Point(1, -eps))) / (2.0 * eps))))

        # PDF value
        LPDF = distribution.computeLogPDF(point)
        print("log pdf=%.6f" % LPDF)
        PDF = distribution.computePDF(point)
        print("pdf     =%.6f" % PDF)
        # by the finite difference technique from CDF
        if dim == 1:
            print("pdf (FD)=%.6f" % ((distribution.computeCDF(point + Point(1, eps)) -
                                      distribution.computeCDF(point + Point(1, -eps))) / (2.0 * eps)))

        CDF = distribution.computeCDF(point)
        print("cdf=%.6f" % CDF)
        CCDF = distribution.computeComplementaryCDF(point)
        print("ccdf=%.6f" % CCDF)
        # by the finite difference technique
        if dim == 1:
            PDFgrFD = Point(3)
            PDFgrFD[0] = (Student(distribution.getNu() + eps, distribution.getMu(), distribution.getSigma()[0]).computePDF(point) -
                          Student(distribution.getNu() - eps, distribution.getMu(), distribution.getSigma()[0]).computePDF(point)) / (2.0 * eps)
            PDFgrFD[1] = (Student(distribution.getNu(), distribution.getMu() + eps, distribution.getSigma()[0]).computePDF(point) -
                          Student(distribution.getNu(), distribution.getMu() - eps, distribution.getSigma()[0]).computePDF(point)) / (2.0 * eps)
            PDFgrFD[2] = (Student(distribution.getNu(), distribution.getMu(), distribution.getSigma()[0] + eps).computePDF(point) -
                          Student(distribution.getNu(), distribution.getMu(), distribution.getSigma()[0] - eps).computePDF(point)) / (2.0 * eps)
            print("pdf gradient (FD)=", repr(PDFgrFD))

    # quantile
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = Point(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" %
          distribution.computeSurvivalFunction(inverseSurvival))
    print("entropy=%.6f" % distribution.computeEntropy())

    # Confidence regions
    interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
        0.95)
    print("Minimum volume interval=", interval)
    print("threshold=", Point(1, threshold))
    levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(
        0.95)
    print("Minimum volume level set=", levelSet)
    print("beta=", Point(1, beta))
    interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
        0.95)
    print("Bilateral confidence interval=", interval)
    print("beta=", Point(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False)
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", Point(1, beta))
    interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True)
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", Point(1, beta))

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

    x = 0.6
    y = [0.2]*(dim-1)
    print("conditional PDF=%.6f" % distribution.computeConditionalPDF(x, y))
    print("conditional CDF=%.6f" % distribution.computeConditionalCDF(x, y))
    print("conditional quantile=%.6f" %
          distribution.computeConditionalQuantile(x, y))
    pt = Point([i + 1.5 for i in range(dim)])
    print("sequential conditional PDF=",
          distribution.computeSequentialConditionalPDF(point))
    resCDF = distribution.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print("sequential conditional quantile(", resCDF, ")=",
          distribution.computeSequentialConditionalQuantile(resCDF))

except:
    import sys
    print("t_Student_std.py", sys.exc_info()[0], sys.exc_info()[1])
