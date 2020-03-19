#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
from math import sqrt

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)


mu = [0.0] * 4
sigma = [1.0] * 4
a = [-4., -1., 1., 3.]
b = [4., 4., 2., 6.]

for i in range(4):
    ot.PlatformInfo.SetNumericalPrecision(1 if i == 2 else 2)
    distribution = ot.TruncatedNormal(mu[i], sigma[i], a[i], b[i])
    size = 10000
    sample = distribution.getSample(size)
    factory = ot.TruncatedNormalFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedTruncatedNormal = factory.buildAsTruncatedNormal(sample)
    print("TruncatedNormal          =", distribution)
    print("Estimated TruncatedNormal=", estimatedTruncatedNormal)
    estimatedTruncatedNormal = factory.buildAsTruncatedNormal()
    print("Default TruncatedNormal=", estimatedTruncatedNormal)
    estimatedTruncatedNormal = factory.buildAsTruncatedNormal(
        distribution.getParameter())
    print("TruncatedNormal from parameters=", estimatedTruncatedNormal)
    # Various estimators
    estimatedDistribution = factory.build(sample)
    print('Estimated distribution with default estimator =', estimatedDistribution)
    estimatedDistribution = factory.buildMethodOfMoments(sample)
    print('Estimated distribution with moments=', estimatedDistribution)
    estimatedDistribution = factory.buildMethodOfLikelihoodMaximization(sample)
    print('Estimated distribution with likelihoodMax.=', estimatedDistribution)
sample = [[0.0]] * size
estimatedDistribution = factory.build(sample)
print("Estimated distribution with default estimator=", repr(estimatedDistribution))
sample = [[1.0]] * size
estimatedDistribution = factory.build(sample)
print("Estimated distribution with default estimator=", repr(estimatedDistribution))

# Build method of moments
print("Build method of moments")
size = 10000
distribution = ot.TruncatedNormal(2.0, 3.0, -1.0, 4.0)
sample = distribution.getSample(size)
factory = ot.TruncatedNormalFactory()
estimatedTN = factory.buildMethodOfMoments(sample)
exact_mu = sample.computeMean()[0]
exact_sigma2 = sample.computeCovariance()[0,0]
exact_a = sample.getMin()[0]
exact_b = sample.getMax()[0]
computed_mu = estimatedTN.getMean()[0]
computed_sigma2 = estimatedTN.getCovariance()[0,0]
computed_a = estimatedTN.getA()
computed_b = estimatedTN.getB()
ott.assert_almost_equal(exact_mu, computed_mu, 1.e-2, 0.0)
ott.assert_almost_equal(exact_sigma2, computed_sigma2, 1.e-3, 0.0)
ott.assert_almost_equal(exact_a, computed_a, 0.0, 10.0 / size)
ott.assert_almost_equal(exact_b, computed_b, 0.0, 10.0 / size)

# Build method of likelihood maximization
print("Build method of likelihood maximization")
size = 10000
distribution = ot.TruncatedNormal(2.0, 3.0, -1.0, 4.0)
sample = distribution.getSample(size)
factory = ot.TruncatedNormalFactory()
estimatedTN = factory.buildMethodOfLikelihoodMaximization(sample)
exact_mu = distribution.getMu()
exact_sigma = distribution.getSigma()
exact_a = distribution.getA()
exact_b = distribution.getB()
computed_mu = estimatedTN.getMu()
computed_sigma = estimatedTN.getSigma()
computed_a = estimatedTN.getA()
computed_b = estimatedTN.getB()
ott.assert_almost_equal(exact_mu, computed_mu, 0.0, 20.0 / sqrt(size))
ott.assert_almost_equal(exact_sigma, computed_sigma, 0.0, 20.0 / sqrt(size))
ott.assert_almost_equal(exact_a, computed_a, 0.0, 20.0 / size)
ott.assert_almost_equal(exact_b, computed_b, 0.0, 20.0 / size)

# A tricky case
print("A tricky case")
data = [0.6852,0.9349,0.5884,1.727,1.581,0.3193,-0.5701,1.623,2.210,
        -0.3440,-0.1646]
sample = ot.Sample(data, 1)
factory = ot.TruncatedNormalFactory()
estimatedTN = factory.buildMethodOfLikelihoodMaximization(sample)
exact_mu = 0.639863
exact_sigma = 1.89489
exact_a = -0.783954
exact_b = 2.42385
computed_mu = estimatedTN.getMu()
computed_sigma = estimatedTN.getSigma()
computed_a = estimatedTN.getA()
computed_b = estimatedTN.getB()
ott.assert_almost_equal(exact_mu, computed_mu, 1.e-2, 0.0)
ott.assert_almost_equal(exact_sigma, computed_sigma, 1.e-2, 0.0)
ott.assert_almost_equal(exact_a, computed_a, 1.e-2, 0.0)
ott.assert_almost_equal(exact_b, computed_b, 1.e-2, 0.0)

