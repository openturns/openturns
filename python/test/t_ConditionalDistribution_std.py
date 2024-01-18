#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Multivariate case
coll2 = ot.DistributionCollection(0)
coll2.add(ot.Dirac(1))
coll2.add(ot.Dirac(2))
coll2.add(ot.Bernoulli(0.7))
coll2.add(ot.Uniform(3.0, 4.0))
d2 = ot.JointDistribution(coll2)
coll1 = ot.DistributionCollection(0)
coll1.add(ot.Uniform())
coll1.add(ot.Uniform())
d1 = ot.JointDistribution(coll1)
# Test the different DOE
ot.ResourceMap.SetAsUnsignedInteger(
    "ConditionalDistribution-MarginalIntegrationNodesNumber", 256
)
ot.ResourceMap.SetAsUnsignedInteger(
    "ConditionalDistribution-MaximumIntegrationNodesNumber", 10000
)
for method in ["GaussProduct", "QMC", "MC"]:
    print("#" * 50)
    print("method=", method)
    ot.ResourceMap.SetAsString(
        "ConditionalDistribution-ContinuousDiscretizationMethod", method
    )
    distribution = ot.ConditionalDistribution(d1, d2)
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
    zero = ot.Point(dim, 0.0)

    # Show PDF and CDF of zero point
    zeroPDF = distribution.computePDF(zero)
    zeroCDF = distribution.computeCDF(zero)
    print("Zero point= ", zero, " pdf=", zeroPDF, " cdf=", zeroCDF)

    # Get 95% quantile
    quantile = distribution.computeQuantile(0.95)
    print("Quantile=", quantile)
    print("CDF(quantile)= %.5g" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print(
        "Survival(inverseSurvival)=%.6f"
        % distribution.computeSurvivalFunction(inverseSurvival)
    )

    # Confidence regions
    # interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(0.95)
    # print("Minimum volume interval=", interval)
    # print("threshold=", Point(1, threshold))
    # levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(0.95)
    # print("Minimum volume level set=", levelSet)
    # print("beta=", Point(1, beta))
    # interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(0.95)
    # print("Bilateral confidence interval=", interval)
    # print("beta=", Point(1, beta))
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, False
    )
    print("Unilateral confidence interval (lower tail)=", interval)
    print("beta=", ot.Point(1, beta))
    (
        interval,
        beta,
    ) = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
        0.95, True
    )
    print("Unilateral confidence interval (upper tail)=", interval)
    print("beta=", ot.Point(1, beta))

# "
# 1D tests
# Create a collection of distribution
conditionedDistribution = ot.Normal()
conditioningDistributionCollection = ot.DistributionCollection(0)
# First conditioning distribution: continuous/continuous
atoms = ot.DistributionCollection(0)
atoms.add(ot.Uniform(0.0, 1.0))
atoms.add(ot.Uniform(1.0, 2.0))
conditioningDistributionCollection.add(ot.JointDistribution(atoms))
# Second conditioning distribution: discrete/continuous
atoms = ot.DistributionCollection(0)
atoms.add(ot.Binomial(3, 0.5))
atoms.add(ot.Uniform(1.0, 2.0))
conditioningDistributionCollection.add(ot.JointDistribution(atoms))
# Third conditioning distribution: dirac/continuous
atoms = ot.DistributionCollection(0)
atoms.add(ot.Dirac(0.5))
atoms.add(ot.Uniform(1.0, 2.0))
conditioningDistributionCollection.add(ot.JointDistribution(atoms))
for i in range(conditioningDistributionCollection.getSize()):
    print("conditioning distribution=", conditioningDistributionCollection[i])
    distribution = ot.ConditionalDistribution(
        conditionedDistribution, conditioningDistributionCollection[i]
    )
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
    zero = ot.Point(dim, 0.0)

    # Show PDF and CDF of zero point
    zeroPDF = distribution.computePDF(zero)
    zeroCDF = distribution.computeCDF(zero)
    print("Zero point= ", zero, " pdf=%.6f" % zeroPDF, " cdf=%.6f" % zeroCDF)

    # Get 95% quantile
    quantile = distribution.computeQuantile(0.95)
    print("Quantile=", quantile)
    print("CDF(quantile)= %.12g" % distribution.computeCDF(quantile))
    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    print("InverseSurvival=", repr(inverseSurvival))
    print(
        "Survival(inverseSurvival)=%.6f"
        % distribution.computeSurvivalFunction(inverseSurvival)
    )
