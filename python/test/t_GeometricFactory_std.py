#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


distribution = ot.Geometric(0.7)
size = 10000
sample = distribution.getSample(size)
factory = ot.GeometricFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedGeometric = factory.buildAsGeometric(sample)
print("Geometric          =", distribution)
print("Estimated geometric=", estimatedGeometric)
estimatedGeometric = factory.buildAsGeometric()
print("Default geometric=", estimatedGeometric)
estimatedGeometric = factory.buildAsGeometric(
    distribution.getParameter())
print("Geometric from parameters=", estimatedGeometric)

