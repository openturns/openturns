#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.GumbelCopula(1.5)
size = 1000
sample = distribution.getSample(size)
factory = ot.GumbelCopulaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedGumbelCopula = factory.buildAsGumbelCopula(sample)
print("GumbelCopula          =", distribution)
print("Estimated gumbelCopula=", estimatedGumbelCopula)
estimatedGumbelCopula = factory.buildAsGumbelCopula()
print("Default gumbelCopula=", estimatedGumbelCopula)
estimatedGumbelCopula = factory.buildAsGumbelCopula(distribution.getParameter())
print("GumbelCopula from parameters=", estimatedGumbelCopula)
