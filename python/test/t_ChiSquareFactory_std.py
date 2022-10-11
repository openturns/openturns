#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.ChiSquare(0.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.ChiSquareFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
distribution = ot.ChiSquare(1.0)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
distribution = ot.ChiSquare(2.5)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedChiSquare = factory.buildAsChiSquare(sample)
print("ChiSquare          =", distribution)
print("Estimated chiSquare=", estimatedChiSquare)
estimatedChiSquare = factory.buildAsChiSquare()
print("Default chiSquare=", estimatedChiSquare)
estimatedChiSquare = factory.buildAsChiSquare(distribution.getParameter())
print("ChiSquare from parameters=", estimatedChiSquare)
