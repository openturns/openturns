#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Exponential(2.5, -1.3)
size = 10000
sample = distribution.getSample(size)
factory = ot.ExponentialFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedExponential = factory.buildAsExponential(sample)
print("Exponential          =", distribution)
print("Estimated exponential=", estimatedExponential)
estimatedExponential = factory.buildAsExponential()
print("Default exponential=", estimatedExponential)
estimatedExponential = factory.buildAsExponential(distribution.getParameter())
print("Exponential from parameters=", estimatedExponential)
