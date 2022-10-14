#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Rice(0.5, 1.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.RiceFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedRice = factory.buildAsRice(sample)
print("Rice          =", distribution)
print("Estimated rice=", estimatedRice)
estimatedRice = factory.buildAsRice(distribution.getParameter())
print("Rice from parameters=", estimatedRice)
estimatedRice = factory.buildAsRice()
print("Default rice=", estimatedRice)
