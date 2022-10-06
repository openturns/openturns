#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Rayleigh(2.5, -1.0)
size = 10000
sample = distribution.getSample(size)
factory = ot.RayleighFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedRayleigh = factory.buildAsRayleigh(sample)
print("Rayleigh          =", distribution)
print("Estimated rayleigh=", estimatedRayleigh)
estimatedRayleigh = factory.buildAsRayleigh()
print("Default rayleigh=", estimatedRayleigh)
estimatedRayleigh = factory.buildAsRayleigh(
    distribution.getParameter())
print("Rayleigh from parameters=", estimatedRayleigh)
