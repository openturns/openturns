#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.ClaytonCopula(1.5)
size = 1000
sample = distribution.getSample(size)
factory = ot.ClaytonCopulaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedClaytonCopula = factory.buildAsClaytonCopula(sample)
print("ClaytonCopula          =", distribution)
print("Estimated claytonCopula=", estimatedClaytonCopula)
estimatedClaytonCopula = factory.buildAsClaytonCopula()
print("Default claytonCopula=", estimatedClaytonCopula)
estimatedClaytonCopula = factory.buildAsClaytonCopula(distribution.getParameter())
print("ClaytonCopula from parameters=", estimatedClaytonCopula)
