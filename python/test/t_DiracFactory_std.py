#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Dirac(0.7)
size = 10000
sample = distribution.getSample(size)
factory = ot.DiracFactory()
estimatedDistribution = factory.build(sample)
print("Distribution          =", distribution)
print("Estimated distribution=", estimatedDistribution)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedDirac = factory.buildAsDirac(sample)
print("Dirac          =", distribution)
print("Estimated dirac=", estimatedDirac)
estimatedDirac = factory.buildAsDirac()
print("Default dirac=", estimatedDirac)
estimatedDirac = factory.buildAsDirac(
    distribution.getParameter())
print("Dirac from parameters=", estimatedDirac)
