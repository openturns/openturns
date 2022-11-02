#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.FarlieGumbelMorgensternCopula(0.7)
size = 1000
sample = distribution.getSample(size)
factory = ot.FarlieGumbelMorgensternCopulaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedFarlieGumbelMorgensternCopula = factory.buildAsFarlieGumbelMorgensternCopula(
    sample
)
print("FarlieGumbelMorgensternCopula          =", distribution)
print(
    "Estimated farlieGumbelMorgensternCopula=", estimatedFarlieGumbelMorgensternCopula
)
estimatedFarlieGumbelMorgensternCopula = factory.buildAsFarlieGumbelMorgensternCopula()
print("Default farlieGumbelMorgensternCopula=", estimatedFarlieGumbelMorgensternCopula)
estimatedFarlieGumbelMorgensternCopula = factory.buildAsFarlieGumbelMorgensternCopula(
    distribution.getParameter()
)
print(
    "FarlieGumbelMorgensternCopula from parameters=",
    estimatedFarlieGumbelMorgensternCopula,
)
