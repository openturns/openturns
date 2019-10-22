#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    dimension = 3
    meanPoint = Point(dimension, 1.0)
    meanPoint[0] = 0.5
    meanPoint[1] = -0.5
    sigma = Point(dimension, 1.0)
    sigma[0] = 2.0
    sigma[1] = 3.0
    R = CorrelationMatrix(dimension)
    for i in range(1, dimension):
        R[i, i - 1] = 0.5

    # Create a collection of distribution
    aCollection = DistributionCollection()

    aCollection.add(Normal(meanPoint, sigma, R))
    meanPoint += Point(meanPoint.getDimension(), 1.0)
    aCollection.add(Normal(meanPoint, sigma, R))
    meanPoint += Point(meanPoint.getDimension(), 1.0)
    aCollection.add(Normal(meanPoint, sigma, R))

    # Instanciate one distribution object
    distribution = Mixture(
        aCollection, Point(aCollection.getSize(), 2.0))
    print("Distribution ", repr(distribution))
    print("Weights = ", repr(distribution.getWeights()))
    weights = distribution.getWeights()
    weights[0] = 2.0 * weights[0]
    distribution.setWeights(weights)
    print("After update, new weights = ", repr(distribution.getWeights()))
    distribution = Mixture(aCollection)
    print("Distribution ", repr(distribution))

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", repr(oneRealization))

    # Test for sampling
    size = 1000
    oneSample = distribution.getSample(size)
    print("oneSample first=", repr(
        oneSample[0]), " last=", repr(oneSample[size - 1]))
    print("mean=", repr(oneSample.computeMean()))
    print("covariance=", repr(oneSample.computeCovariance()))

    # Define a point
    point = Point(distribution.getDimension(), 1.0)
    print("Point= ", repr(point))

    # Show PDF and CDF of point
    eps = 1e-5

    # derivative of PDF with regards its arguments
    DDF = distribution.computeDDF(point)
    print("ddf     =", repr(DDF))
    # by the finite difference technique
    ddfFD = Point(dimension)
    for i in range(dimension):
        left = Point(point)
        left[i] += eps
        right = Point(point)
        right[i] -= eps
        ddfFD[i] = (distribution.computePDF(left) -
                    distribution.computePDF(right)) / (2.0 * eps)
    print("ddf (FD)=", repr(ddfFD))

    # PDF value
    LPDF = distribution.computeLogPDF(point)
    print("log pdf=%.6f" % LPDF)
    PDF = distribution.computePDF(point)
    print("pdf     =%.6f" % PDF)
    # by the finite difference technique from CDF
    if (dimension == 1):
        print("pdf (FD)=%.6f" % ((distribution.computeCDF(point + Point(1, eps)) -
                                  distribution.computeCDF(point + Point(1, -eps))) / (2.0 * eps)))

    x = 0.6
    y = [0.2]*(dimension-1)
    print("conditional PDF%.6f" % distribution.computeConditionalPDF(x, y))
    print("conditional CDF%.6f" % distribution.computeConditionalCDF(x, y))
    print("conditional quantile%.6f" %
          distribution.computeConditionalQuantile(x, y))
    pt = Point([i + 1.5 for i in range(dimension)])
    print("sequential conditional PDF=",
          distribution.computeSequentialConditionalPDF(point))
    resCDF = distribution.computeSequentialConditionalCDF(pt)
    print("sequential conditional CDF(", pt, ")=", resCDF)
    print("sequential conditional quantile(", resCDF, ")=",
          distribution.computeSequentialConditionalQuantile(resCDF))
    # derivative of the PDF with regards the parameters of the distribution
    CDF = distribution.computeCDF(point)
    print("cdf=%.6f" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf=%.6f" % CCDF)
    # PDFgr = distribution.computePDFGradient( point )
    # print "pdf gradient     =" , repr(PDFgr)
    # by the finite difference technique
    # PDFgrFD = Point(4)
    # PDFgrFD[0] = (Mixture(distribution.getR() + eps, distribution.getT(), distribution.getA(), distribution.getB()).computePDF(point) -
    #                   Mixture(distribution.getR() - eps, distribution.getT(), distribution.getA(), distribution.getB()).computePDF(point)) / (2.0 * eps)
    #     PDFgrFD[1] = (Mixture(distribution.getR(), distribution.getT() + eps, distribution.getA(), distribution.getB()).computePDF(point) -
    #                   Mixture(distribution.getR(), distribution.getT() - eps, distribution.getA(), distribution.getB()).computePDF(point)) / (2.0 * eps)
    #     PDFgrFD[2] = (Mixture(distribution.getR(), distribution.getT(), distribution.getA() + eps, distribution.getB()).computePDF(point) -
    #                   Mixture(distribution.getR(), distribution.getT(), distribution.getA() - eps, distribution.getB()).computePDF(point)) / (2.0 * eps)
    #     PDFgrFD[3] = (Mixture(distribution.getR(), distribution.getT(), distribution.getA(), distribution.getB() + eps).computePDF(point) -
    #                   Mixture(distribution.getR(), distribution.getT(), distribution.getA(), distribution.getB() - eps).computePDF(point)) / (2.0 * eps)
    # print "pdf gradient (FD)=" , repr(PDFgrFD)

    # derivative of the PDF with regards the parameters of the distribution
    # CDFgr = distribution.computeCDFGradient( point )
    # print "cdf gradient     =" , repr(CDFgr)
    # CDFgrFD = Point(4)
    # CDFgrFD[0] = (Mixture(distribution.getR() + eps, distribution.getT(), distribution.getA(), distribution.getB()).computeCDF(point) -
    #                   Mixture(distribution.getR() - eps, distribution.getT(), distribution.getA(), distribution.getB()).computeCDF(point)) / (2.0 * eps)
    #     CDFgrFD[1] = (Mixture(distribution.getR(), distribution.getT() + eps, distribution.getA(), distribution.getB()).computeCDF(point) -
    #                   Mixture(distribution.getR(), distribution.getT() - eps, distribution.getA(), distribution.getB()).computeCDF(point)) / (2.0 * eps)
    #     CDFgrFD[2] = (Mixture(distribution.getR(), distribution.getT(), distribution.getA() + eps, distribution.getB()).computeCDF(point) -
    #                   Mixture(distribution.getR(), distribution.getT(), distribution.getA() - eps, distribution.getB()).computeCDF(point)) / (2.0 * eps)
    #     CDFgrFD[3] = (Mixture(distribution.getR(), distribution.getT(), distribution.getA(), distribution.getB() + eps).computeCDF(point) -
    #                   Mixture(distribution.getR(), distribution.getT(), distribution.getA(), distribution.getB() - eps).computeCDF(point)) / (2.0 * eps)
    # print "cdf gradient (FD)=",  repr(CDFgrFD)

    # quantile
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", repr(quantile))
    print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = Point(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" %
          distribution.computeSurvivalFunction(inverseSurvival))
    # Confidence regions
    if distribution.getDimension() <= 2:
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
    covariance = distribution.getCovariance()
    print("covariance=", repr(covariance))
    parameters = distribution.getParametersCollection()
    print("parameters=", repr(parameters))
    parameter = distribution.getParameter()
    print("parameter=", parameter)
    parameterDescription = distribution.getParameterDescription()
    print("parameter description=", parameterDescription)
    distribution.setParameter(parameter)
    for i in range(6):
        print("standard moment n=", i, " value=",
              distribution.getStandardMoment(i))
    print("Standard representative=", distribution.getStandardRepresentative())

    # Constructor with separate weights. Also check small weights removal
    weights = [1.0e-20, 2.5, 32.0]
    atoms = DistributionCollection(
        [Normal(1.0, 1.0), Normal(2.0, 2.0), Normal(3.0, 3.0)])
    newMixture = Mixture(atoms, weights)
    print("newMixture pdf= %.12g" % newMixture.computePDF(2.5))
    print("atoms kept in mixture=", newMixture.getDistributionCollection())
    print("newMixture=", newMixture)

except:
    import sys
    print("t_Mixture_std.py", sys.exc_info()[0], sys.exc_info()[1])
