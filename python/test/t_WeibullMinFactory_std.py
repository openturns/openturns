#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

distribution = ot.WeibullMin(1.0, 2.5, -1.0)
size = 10000
sample = distribution.getSample(size)
factory = ot.WeibullMinFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedWeibull = factory.buildAsWeibullMin(sample)
print("Estimated weibull=", estimatedWeibull)
estimatedWeibull = factory.buildAsWeibullMin()
print("Default weibull=", estimatedWeibull)
estimatedWeibull = factory.buildAsWeibullMin(distribution.getParameter())
print("WeibullMin from parameters=", estimatedWeibull)
