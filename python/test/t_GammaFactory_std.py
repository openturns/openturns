#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

distribution = ot.Gamma(0.2, 1.0, 1.0)
size = 10000
sample = distribution.getSample(size)
factory = ot.GammaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
distribution = ot.Gamma(2.3, 1.0, 1.0)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
distribution = ot.Gamma(2.3, 1.0, 1.0)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
print("Distribution          =", distribution)
print("Estimated distribution=", estimatedDistribution)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedGamma = factory.buildAsGamma(sample)
print("Gamma          =", distribution)
print("Estimated gamma=", estimatedGamma)
estimatedGamma = factory.buildAsGamma()
print("Default gamma=", estimatedGamma)
estimatedGamma = factory.buildAsGamma(
    distribution.getParameter())
print("Gamma from parameters=", estimatedGamma)

