#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

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
sample = [[0.0]] * size
estimatedDistribution = factory.build(sample)
print("Estimated distribution=", repr(estimatedDistribution))
sample = [[1.0]] * size
estimatedDistribution = factory.build(sample)
print("Estimated distribution=", repr(estimatedDistribution))
