#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Chi(0.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.ChiFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
distribution = ot.Chi(1.0)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
distribution = ot.Chi(2.5)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedChi = factory.buildAsChi(sample)
print("Chi          =", distribution)
print("Estimated chi=", estimatedChi)
estimatedChi = factory.buildAsChi()
print("Default chi=", estimatedChi)
estimatedChi = factory.buildAsChi(distribution.getParameter())
print("Chi from parameters=", estimatedChi)

# overflow in boost::math::gamma_q_inv
true_dist = ot.Beta(0.9, 2., 3e7, 5e7)
ot.RandomGenerator.SetSeed(0)
data = true_dist.getSample(229)
ot.ChiFactory().build(data)
