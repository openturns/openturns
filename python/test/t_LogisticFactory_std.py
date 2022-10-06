#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Logistic(2.0, 2.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.LogisticFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedLogistic = factory.buildAsLogistic(sample)
print("Logistic          =", distribution)
print("Estimated logistic=", estimatedLogistic)
estimatedLogistic = factory.buildAsLogistic()
print("Default logistic=", estimatedLogistic)
estimatedLogistic = factory.buildAsLogistic(
    distribution.getParameter())
print("Logistic from parameters=", estimatedLogistic)

