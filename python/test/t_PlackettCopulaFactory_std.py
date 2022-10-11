#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.PlackettCopula(1.5)
size = 1000
sample = distribution.getSample(size)
factory = ot.PlackettCopulaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedPlackettCopula = factory.buildAsPlackettCopula(sample)
print("PlackettCopula          =", distribution)
print("Estimated plackettCopula=", estimatedPlackettCopula)
estimatedPlackettCopula = factory.buildAsPlackettCopula()
print("Default plackettCopula=", estimatedPlackettCopula)
estimatedPlackettCopula = factory.buildAsPlackettCopula(distribution.getParameter())
print("PlackettCopula from parameters=", estimatedPlackettCopula)
