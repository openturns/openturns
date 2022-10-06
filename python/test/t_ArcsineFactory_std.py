#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


distribution = ot.Arcsine(1., 2.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.ArcsineFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))

# non-regression for #572
mydist = ot.ArcsineFactory().build(distribution.getSample(10))
myde = ot.MonteCarloExperiment(mydist, 10)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedArcsine = factory.buildAsArcsine(sample)
print("Arcsine          =", distribution)
print("Estimated Arcsine=", estimatedArcsine)
estimatedArcsine = factory.buildAsArcsine()
print("Default Arcsine=", estimatedArcsine)
estimatedArcsine = factory.buildAsArcsine(
    distribution.getParameter())
print("Arcsine from parameters=", estimatedArcsine)
