#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Laplace(-1.3, 2.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.LaplaceFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedLaplace = factory.buildAsLaplace(sample)
print("Laplace          =", distribution)
print("Estimated laplace=", estimatedLaplace)
estimatedLaplace = factory.buildAsLaplace()
print("Default laplace=", estimatedLaplace)
estimatedLaplace = factory.buildAsLaplace(distribution.getParameter())
print("Laplace from parameters=", estimatedLaplace)
