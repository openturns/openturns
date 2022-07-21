#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Multinomial(5, [0.25] * 3)
size = 10000
sample = distribution.getSample(size)
factory = ot.MultinomialFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedMultinomial = factory.buildAsMultinomial(sample)
print("Multinomial          =", distribution)
print("Estimated multinomial=", estimatedMultinomial)
estimatedMultinomial = factory.buildAsMultinomial()
print("Default multinomial=", estimatedMultinomial)
