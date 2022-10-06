#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.InverseNormal(2.5, 1.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.InverseNormalFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedInverseNormal = factory.buildAsInverseNormal(sample)
print("InverseNormal          =", distribution)
print("Estimated inverseNormal=", estimatedInverseNormal)
estimatedInverseNormal = factory.buildAsInverseNormal()
print("Default inverseNormal=", estimatedInverseNormal)
estimatedInverseNormal = factory.buildAsInverseNormal(
    distribution.getParameter())
print("InverseNormal from parameters=", estimatedInverseNormal)

