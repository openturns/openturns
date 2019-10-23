#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Instanciate one distribution object
    for dim in range(1, 2):
        theta = Point(dim + 1)
        for i in range(dim + 1):
            theta[i] = (i + 1.0) / 4.0
        distribution = Dirichlet(theta)
        description = [''] * dim
        for j in range(1, dim + 1):
            oss = 'Marginal ' + str(j)
            description[j - 1] = oss
        distribution.setDescription(description)
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

        if (dim == 1):
            size = 100
            for i in range(2):
                RandomGenerator.SetSeed(0)
                msg = ''
                if FittingTest.Kolmogorov(distribution.getSample(size), distribution).getBinaryQualityMeasure():
                    msg = "accepted"
                else:
                    msg = "rejected"
                print(
                    "Kolmogorov test for the generator, sample size=", size, " is", msg)
                size *= 10

        # Define a point
        point = Point(
            distribution.getDimension(), 0.5 / distribution.getDimension())
        print("Point= ", repr(point))

        # Show PDF and CDF of point
        LPDF = distribution.computeLogPDF(point)
        print("log pdf= %.8g" % LPDF)
        PDF = distribution.computePDF(point)
        print("pdf     = %.8g" % PDF)
        CDF = distribution.computeCDF(point)
        print("cdf= %.8g" % CDF)
        quantile = distribution.computeQuantile(0.95)
        print("quantile=", repr(quantile))
        print("cdf(quantile)= %.6f" % distribution.computeCDF(quantile))
        # Get 95% survival function
        inverseSurvival = Point(
            distribution.computeInverseSurvivalFunction(0.95))
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

        x = 0.6
        y = [0.2]*(dim-1)
        print("conditional PDF=%.6f" %
              distribution.computeConditionalPDF(x, y))
        print("conditional CDF=%.6f" %
              distribution.computeConditionalCDF(x, y))
        print("conditional quantile=%.6f" %
              distribution.computeConditionalQuantile(x, y))
        pt = Point([0.1 * i + 0.15 for i in range(dim)])
        print("sequential conditional PDF=",
              distribution.computeSequentialConditionalPDF(point))
        resCDF = distribution.computeSequentialConditionalCDF(pt)
        print("sequential conditional CDF(", pt, ")=", resCDF)
        print("sequential conditional quantile(", resCDF, ")=",
              distribution.computeSequentialConditionalQuantile(resCDF))

        # Extract the marginals
        for i in range(dim):
            margin = distribution.getMarginal(i)
            print("margin=", margin)
            print("margin PDF= %.8g" %
                  margin.computePDF(Point(1, 0.5)))
            print("margin CDF= %.8g" %
                  margin.computeCDF(Point(1, 0.5)))
            print("margin quantile=", repr(margin.computeQuantile(0.95)))
            print("margin realization=", repr(margin.getRealization()))
        if (dim >= 2):
            # Extract a 2-D marginal
            indices = Indices(2, 0)
            indices[0] = 1
            indices[1] = 0
            print("indices=", indices)
            margins = distribution.getMarginal(indices)
            print("margins=", margins)
            print("margins PDF=", margins.computePDF(Point(2, 0.5)))
            print("margins CDF= %.8g" %
                  margins.computeCDF(Point(2, 0.5)))
            quantile = margins.computeQuantile(0.95)
            print("margins quantile=", repr(quantile))
            print("margins CDF(quantile)= %.6f" % margins.computeCDF(quantile))
            print("margins realization=", repr(margins.getRealization()))

except:
    import sys
    print("t_Dirichlet_std.py", sys.exc_info()[0], sys.exc_info()[1])
