#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


distribution = ot.AliMikhailHaqCopula(0.5)
size = 1000
sample = distribution.getSample(size)
factory = ot.AliMikhailHaqCopulaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedAliMikhailHaqCopula = factory.buildAsAliMikhailHaqCopula(sample)
print("Distribution          =", distribution)
print("Estimated AliMikhailHaqCopula=", estimatedAliMikhailHaqCopula)
estimatedAliMikhailHaqCopula = factory.buildAsAliMikhailHaqCopula()
print("Default AliMikhailHaqCopula=", estimatedAliMikhailHaqCopula)
estimatedAliMikhailHaqCopula = factory.buildAsAliMikhailHaqCopula(
    distribution.getParameter())
print("AliMikhailHaqCopula from parameters=", estimatedAliMikhailHaqCopula)
