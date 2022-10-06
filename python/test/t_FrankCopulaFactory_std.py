#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.FrankCopula(1.5)
size = 1000
sample = distribution.getSample(size)
factory = ot.FrankCopulaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedFrankCopula = factory.buildAsFrankCopula(sample)
print("FrankCopula          =", distribution)
print("Estimated frankCopula=", estimatedFrankCopula)
estimatedFrankCopula = factory.buildAsFrankCopula()
print("Default frankCopula=", estimatedFrankCopula)
estimatedFrankCopula = factory.buildAsFrankCopula(
    distribution.getParameter())
print("FrankCopula from parameters=", estimatedFrankCopula)
