#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)


distribution = Arcsine(1., 2.5)
size = 10000
sample = distribution.getSample(size)
factory = ArcsineFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))

# non-regression for #572
mydist = ArcsineFactory().build(distribution.getSample(10))
myde = MonteCarloExperiment(mydist, 10)
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
