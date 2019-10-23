#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import math as m


def cleanPoint(inPoint):
    dim = inPoint.getDimension()
    for i in range(dim):
        if (m.fabs(inPoint[i]) < 1.e-10):
            inPoint[i] = 0.0
    return inPoint


def cleanCovariance(inCovariance):
    dim = inCovariance.getDimension()
    for j in range(dim):
        for i in range(dim):
            if (m.fabs(inCovariance[i, j]) < 1.e-10):
                inCovariance[i, j] = 0.0
    return inCovariance


# Instanciate one distribution object
referenceDistribution = [ot.TruncatedNormal(2.0, 1.5, 1.0, 4.0),
                         ot.TruncatedNormal(2.0, 1.5, 1.0, 200.0),
                         ot.TruncatedNormal(2.0, 1.5, -200.0, 4.0),
                         ot.TruncatedNormal(2.0, 1.5, 1.0, 4.0)]
distribution = [ot.TruncatedDistribution(ot.Normal(2.0, 1.5), 1.0, 4.0),
                ot.TruncatedDistribution(
                    ot.Normal(2.0, 1.5), 1.0, ot.TruncatedDistribution.LOWER),
                ot.TruncatedDistribution(
                    ot.Normal(2.0, 1.5), 4.0, ot.TruncatedDistribution.UPPER),
                ot.TruncatedDistribution(ot.Normal(2.0, 1.5), ot.Interval([1.0], [4.0], [True], [True]))]

# add a 2-d test
dimension = 2
# This distribution takes too much time for the test
#size = 70
#ref = ot.Normal(dimension)
#sample = ref.getSample(size)
#ks = ot.KernelSmoothing().build(sample)
# Use a multivariate Normal distribution instead
ks = ot.Normal(2)
truncatedKS = ot.TruncatedDistribution(
    ks, ot.Interval([-0.5] * dimension, [2.0] * dimension))
distribution.append(truncatedKS)
referenceDistribution.append(ks)  # N/A
# Add a non-truncated example
weibull = ot.WeibullMin(2.0, 3.0)
distribution.append(ot.TruncatedDistribution(weibull))
referenceDistribution.append(weibull)
ot.RandomGenerator.SetSeed(0)

for testCase in range(len(distribution)):
    print('Distribution ', distribution[testCase])

    # Is this distribution elliptical ?
    print('Elliptical = ', distribution[testCase].isElliptical())

    # Is this distribution continuous ?
    print('Continuous = ', distribution[testCase].isContinuous())

    # Test for realization of distribution
    oneRealization = distribution[testCase].getRealization()
    print('oneRealization=', repr(oneRealization))

    # Test for sampling
    size = 10000
    oneSample = distribution[testCase].getSample(size)
    print('oneSample first=', repr(
        oneSample[0]), ' last=', repr(oneSample[size - 1]))
    print('mean=', repr(oneSample.computeMean()))
    print('covariance=', repr(oneSample.computeCovariance()))

    # Define a point
    point = ot.Point(distribution[testCase].getDimension(), 1.5)
    print('Point= ', repr(point))

    # Show PDF and CDF of point
    eps = 1e-5

    DDF = distribution[testCase].computeDDF(point)
    print('ddf      =', repr(DDF))
    print('ddf (ref)=', repr(
        referenceDistribution[testCase].computeDDF(point)))

    PDF = distribution[testCase].computePDF(point)
    print('pdf      =%.6f' % PDF)
    print('pdf (ref)=%.6f' %
          referenceDistribution[testCase].computePDF(point))

    CDF = distribution[testCase].computeCDF(point)
    print('cdf=%.6f' % CDF)
    CCDF = distribution[testCase].computeComplementaryCDF(point)
    print('ccdf=%.6f' % CCDF)
    print('cdf (ref)=%.6f' %
          referenceDistribution[testCase].computeCDF(point))
    try:
        PDFgr = distribution[testCase].computePDFGradient(point)
        print('pdf gradient      =', repr(cleanPoint(PDFgr)))
        print('pdf gradient (ref)=', repr(
            cleanPoint(referenceDistribution[testCase].computePDFGradient(point))))
    except:
        pass

    try:
        CDFgr = distribution[testCase].computeCDFGradient(point)
        print('cdf gradient      =', repr(cleanPoint(CDFgr)))
        print('cdf gradient (ref)=', repr(
            cleanPoint(referenceDistribution[testCase].computeCDFGradient(point))))
    except:
        pass

    # quantile
    quantile = distribution[testCase].computeQuantile(0.95)
    print('quantile=', repr(quantile))
    print(
        'quantile=', repr(referenceDistribution[testCase].computeQuantile(0.95)))
    print('cdf(quantile)=%.6f' %
          distribution[testCase].computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = ot.Point(
        distribution[testCase].computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" %
          distribution[testCase].computeSurvivalFunction(inverseSurvival))
    print("entropy=%.6f" % distribution[testCase].computeEntropy())

    # Confidence regions
    if distribution[testCase].getDimension() == 1:
        interval, threshold = distribution[
            testCase].computeMinimumVolumeIntervalWithMarginalProbability(0.95)
        print("Minimum volume interval=", interval)
        print("threshold=", ot.Point(1, threshold))
        levelSet, beta = distribution[
            testCase].computeMinimumVolumeLevelSetWithThreshold(0.95)
        print("Minimum volume level set=", levelSet)
        print("beta=", ot.Point(1, beta))
        interval, beta = distribution[
            testCase].computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
        print("Bilateral confidence interval=", interval)
        print("beta=", ot.Point(1, beta))
        interval, beta = distribution[
            testCase].computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, False)
        print("Unilateral confidence interval (lower tail)=", interval)
        print("beta=", ot.Point(1, beta))
        interval, beta = distribution[
            testCase].computeUnilateralConfidenceIntervalWithMarginalProbability(0.95, True)
        print("Unilateral confidence interval (upper tail)=", interval)
        print("beta=", ot.Point(1, beta))

    mean = distribution[testCase].getMean()
    print('mean      =', repr(mean))
    print('mean (ref)=', repr(referenceDistribution[testCase].getMean()))
    standardDeviation = distribution[testCase].getStandardDeviation()
    print('standard deviation      =', repr(standardDeviation))
    print('standard deviation (ref)=', repr(
        referenceDistribution[testCase].getStandardDeviation()))
    skewness = distribution[testCase].getSkewness()
    print('skewness      =', repr(skewness))
    print('skewness (ref)=', repr(
        referenceDistribution[testCase].getSkewness()))
    kurtosis = distribution[testCase].getKurtosis()
    print('kurtosis      =', repr(kurtosis))
    print('kurtosis (ref)=', repr(
        referenceDistribution[testCase].getKurtosis()))
    covariance = distribution[testCase].getCovariance()
    print('covariance      =', repr(cleanCovariance(covariance)))
    print('covariance (ref)=', repr(
        cleanCovariance(referenceDistribution[testCase].getCovariance())))
    parameters = distribution[testCase].getParametersCollection()
    print('parameters      =', repr(parameters))
    print('parameters (ref)=', repr(
        referenceDistribution[testCase].getParametersCollection()))
    print('parameter       =', repr(distribution[testCase].getParameter()))
    print('parameter desc  =',
          repr(distribution[testCase].getParameterDescription()))
    print('marginal 0      =', repr(distribution[testCase].getMarginal(0)))
    for i in range(6):
        print('standard moment n=', i, ' value=',
              referenceDistribution[testCase].getStandardMoment(i))
    print('Standard representative=', referenceDistribution[
          testCase].getStandardRepresentative())
# Check simplification
candidates = [ot.Normal(1.0, 2.0), ot.Uniform(1.0, 2.0), ot.Exponential(1.0, 2.0),
              ot.TruncatedDistribution(ot.WeibullMin(), 1.5, 7.8),
              ot.Beta(1.5, 6.3, -1.0, 2.0)]
intervals = [ot.Interval(-1.0, 4.0), ot.Interval(0.2, 2.4), ot.Interval(2.5, 65.0),
             ot.Interval(2.5, 6.0), ot.Interval(-2.5, 6.0)]
for i in range(len(candidates)):
    d = ot.TruncatedDistribution(candidates[i], intervals[i])
    print("d=", d, "simplified=", d.getSimplifiedVersion())

# Check that we can set the bounds independently
truncated = ot.TruncatedDistribution()
truncated.setDistribution(ot.Normal(20.0, 7.5))
truncated.setBounds(ot.Interval([0], [80], [True], [False]))
print('after setbounds q@0.9=', truncated.computeQuantile(0.9))
# Test for issue #1190
dist = ot.Normal(6.3E-19, 2.1E-19)
dist = ot.TruncatedDistribution(dist, 4.2E-19, ot.TruncatedDistribution.LOWER)
