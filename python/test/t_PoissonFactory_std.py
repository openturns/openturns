#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Poisson(2.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.PoissonFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedPoisson = factory.buildAsPoisson(sample)
print("Poisson          =", distribution)
print("Estimated poisson=", estimatedPoisson)
estimatedPoisson = factory.buildAsPoisson()
print("Default poisson=", estimatedPoisson)
estimatedPoisson = factory.buildAsPoisson(distribution.getParameter())
print("Poisson from parameters=", estimatedPoisson)
