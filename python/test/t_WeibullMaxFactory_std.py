#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.WeibullMax(1.0, 2.5, -1.0)
size = 10000
sample = distribution.getSample(size)
factory = ot.WeibullMaxFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedWeibull = factory.buildAsWeibullMax(sample)
print("Estimated weibull=", estimatedWeibull)
estimatedWeibull = factory.buildAsWeibullMax()
print("Default weibull=", estimatedWeibull)
estimatedWeibull = factory.buildAsWeibullMax(distribution.getParameter())
print("WeibullMax from parameters=", estimatedWeibull)
