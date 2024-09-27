#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Polya(15, 0.7)
size = 10000
sample = distribution.getSample(size)
factory = ot.PolyaFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedNegativeBinomial = factory.buildAsNegativeBinomial(sample)
print("Polya          =", distribution)
print("Estimated negativeBinomial=", estimatedNegativeBinomial)
estimatedNegativeBinomial = factory.buildAsNegativeBinomial()
print("Default negativeBinomial=", estimatedNegativeBinomial)
estimatedNegativeBinomial = factory.buildAsNegativeBinomial(distribution.getParameter())
print("Polya from parameters=", estimatedNegativeBinomial)
