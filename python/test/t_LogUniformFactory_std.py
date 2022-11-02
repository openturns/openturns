#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.LogUniform(1.0, 2.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.LogUniformFactory()
estimatedDistribution = factory.build(sample)
print("Distribution          =", distribution)
print("Estimated distribution=", estimatedDistribution)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedLogUniform = factory.buildAsLogUniform(sample)
print("LogUniform          =", distribution)
print("Estimated logUniform=", estimatedLogUniform)
estimatedLogUniform = factory.buildAsLogUniform()
print("Default logUniform=", estimatedLogUniform)
estimatedLogUniform = factory.buildAsLogUniform(distribution.getParameter())
print("LogUniform from parameters=", estimatedLogUniform)
