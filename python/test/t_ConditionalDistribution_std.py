#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Multivariate case
    coll2 = DistributionCollection(0)
    coll2.add(Dirac(1))
    coll2.add(Dirac(2))
    coll2.add(Bernoulli(0.7))
    coll2.add(Uniform(3.0, 4.0))
    d2 = ComposedDistribution(coll2)
    coll1 = DistributionCollection(0)
    coll1.add(Uniform())
    coll1.add(Uniform())
    d1 = ComposedDistribution(coll1)
    distribution = ConditionalDistribution(d1, d2)
    dim = distribution.getDimension()
    print("distribution=", distribution)
    print("Parameters ", distribution.getParametersCollection())
    print("Mean ", distribution.getMean())
    print("Covariance ", distribution.getCovariance().clean(1e-10))
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
    size = 10
    oneSample = distribution.getSample(size)
    print("oneSample=", oneSample)

    # Test for sampling
    size = 10000
    anotherSample = distribution.getSample(size)
    print("anotherSample mean=", anotherSample.computeMean())
    print("anotherSample covariance=", anotherSample.computeCovariance())

    # Define a point
    zero = NumericalPoint(dim, 0.0)

    # Show PDF and CDF of zero point
    zeroPDF = distribution.computePDF(zero)
    zeroCDF = distribution.computeCDF(zero)
    print("Zero point= ", zero, " pdf=", zeroPDF, " cdf=", zeroCDF)

    # Get 95% quantile
    quantile = distribution.computeQuantile(0.95)
    print("Quantile=", quantile)
    print("CDF(quantile)= %.12g" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = NumericalPoint(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print("Survival(inverseSurvival)=%.6f" % distribution.computeSurvivalFunction(inverseSurvival))
    # Confidence regions
    #threshold = NumericalPoint()
    #print("Minimum volume interval=", distribution.computeMinimumVolumeInterval(0.95, threshold))
    #print("threshold=", threshold)
    beta = NumericalPoint()
    #levelSet = distribution.computeMinimumVolumeLevelSet(0.95, beta)
    #print("Minimum volume level set=", levelSet)
    #print("beta=", beta)
    #print("Bilateral confidence interval=", distribution.computeBilateralConfidenceInterval(0.95, beta))
    #print("beta=", beta)
    print("Unilateral confidence interval (lower tail)=", distribution.computeUnilateralConfidenceInterval(0.95, False, beta))
    print("beta=", beta)
    print("Unilateral confidence interval (upper tail)=", distribution.computeUnilateralConfidenceInterval(0.95, True, beta))
    print("beta=", beta)
    
    # "
    # 1D tests
    # Create a collection of distribution
    conditionedDistribution = Normal()
    conditioningDistributionCollection = DistributionCollection(0)
    # First conditioning distribution: continuous/continuous
    atoms = DistributionCollection(0)
    atoms.add(Uniform(0.0, 1.0))
    atoms.add(Uniform(1.0, 2.0))
    conditioningDistributionCollection.add(ComposedDistribution(atoms))
    # Second conditioning distribution: discrete/continuous
    atoms = DistributionCollection(0)
    atoms.add(Binomial(3, 0.5))
    atoms.add(Uniform(1.0, 2.0))
    conditioningDistributionCollection.add(ComposedDistribution(atoms))
    # Third conditioning distribution: dirac/continuous
    atoms = DistributionCollection(0)
    atoms.add(Dirac(0.5))
    atoms.add(Uniform(1.0, 2.0))
    conditioningDistributionCollection.add(ComposedDistribution(atoms))
    for i in range(conditioningDistributionCollection.getSize()):
        print("conditioning distribution=",
              conditioningDistributionCollection[i])
        distribution = ConditionalDistribution(
            conditionedDistribution, conditioningDistributionCollection[i])
        dim = distribution.getDimension()
        print("Distribution ", distribution)
        print("Parameters ", distribution.getParametersCollection())
        print("Mean ", distribution.getMean())
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
        size = 10
        oneSample = distribution.getSample(size)
        print("oneSample=", oneSample)

        # Test for sampling
        size = 10000
        anotherSample = distribution.getSample(size)
        print("anotherSample mean=", anotherSample.computeMean())
        print("anotherSample covariance=", anotherSample.computeCovariance())

        # Define a point
        zero = NumericalPoint(dim, 0.0)

        # Show PDF and CDF of zero point
        zeroPDF = distribution.computePDF(zero)
        zeroCDF = distribution.computeCDF(zero)
        print("Zero point= ", zero, " pdf=%.6f" %
              zeroPDF, " cdf=%.6f" % zeroCDF)

        # Get 95% quantile
        quantile = distribution.computeQuantile(0.95)
        print("Quantile=", quantile)
        print("CDF(quantile)= %.12g" % distribution.computeCDF(quantile))
        # Get 95% survival function
        inverseSurvival = NumericalPoint(distribution.computeInverseSurvivalFunction(0.95))
        print("InverseSurvival=", repr(inverseSurvival))
        print("Survival(inverseSurvival)=%.6f" % distribution.computeSurvivalFunction(inverseSurvival))

except:
    import sys
    print("t_ConditionalDistribution_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
