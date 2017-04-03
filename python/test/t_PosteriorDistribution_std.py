#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

observationsSize = 5
# Create a collection of distribution
conditionedDistribution = ot.Normal()
conditioningDistributionCollection = []
# First conditioning distribution: continuous/continuous
atoms = [ot.Uniform(0.0, 1.0), ot.Uniform(1.0, 2.0)]
conditioningDistributionCollection.append(ot.ComposedDistribution(atoms))
# Second conditioning distribution: discrete/continuous
atoms = [ot.Binomial(3, 0.5), ot.Uniform(1.0, 2.0)]
# conditioningDistributionCollection.append(ot.ComposedDistribution(atoms))
# Third conditioning distribution: dirac/continuous
atoms = [ot.Dirac(0.0), ot.Uniform(1.0, 2.0)]
conditioningDistributionCollection.append(ot.ComposedDistribution(atoms))


for conditioning in conditioningDistributionCollection:
    print("conditioning distribution=", conditioning)
    observationsDistribution = ot.Distribution(conditionedDistribution)
    observationsDistribution.setParameter(conditioning.getMean())
    observations = observationsDistribution.getSample(observationsSize)
    distribution = ot.PosteriorDistribution(
        ot.ConditionalDistribution(conditionedDistribution, conditioning), observations)
    dim = distribution.getDimension()
    print("Distribution ", distribution)
    print("Distribution ", distribution)
    print("range=", distribution.getRange())
    mean = distribution.getMean()
    print("Mean ", mean)
    print("Covariance ", distribution.getCovariance())
    # Is this distribution an elliptical distribution?
    print("Elliptical distribution= ", distribution.isElliptical())

    # Has this distribution an elliptical copula?
    print("Elliptical copula= ", distribution.hasEllipticalCopula())

    # Has this distribution an independent copula?
    print("Independent copula= ", distribution.hasIndependentCopula())

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    print("oneRealization=", oneRealization)

    # Test for sampling
    # size = 10
    # oneSample = distribution.getSample(size)
    # print("oneSample=", oneSample)

    # Test for sampling
#       size = 10000
#       Sample anotherSample = distribution.getSample(size)
#       print("anotherSample mean=", anotherSample.computeMean())
#       print("anotherSample covariance=", anotherSample.computeCovariance())

    # Define a point
    zero = [0.0] * dim

    # Show PDF and CDF of zero point
    zeroPDF = distribution.computePDF(zero)
    zeroCDF = distribution.computeCDF(zero)
    print("Zero point= ", zero, " pdf=", zeroPDF, " cdf=", zeroCDF)
    # Get 95% quantile
    quantile = distribution.computeQuantile(0.95)
    print("Quantile=", quantile)
    # print("CDF(quantile)=", distribution.computeCDF(quantile))
    # Extract the marginals
    for i in range(dim):
        margin = distribution.getMarginal(i)
        print("margin=", margin)
        # print("margin PDF=", ot.Point(1, margin.computePDF([0.0])))
        print("margin CDF=", ot.Point(1, margin.computeCDF([0.0])))
        print("margin quantile=", margin.computeQuantile(0.95))
        print("margin realization=", margin.getRealization())
