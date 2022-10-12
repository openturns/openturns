#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Dirichlet([0.2, 0.3, 0.4, 0.5])
size = 10000
sample = distribution.getSample(size)
factory = ot.DirichletFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDirichlet = factory.buildAsDirichlet(sample)
print("Dirichlet          =", distribution)
print("Estimated dirichlet=", estimatedDirichlet)
estimatedDirichlet = factory.buildAsDirichlet()
print("Default dirichlet=", estimatedDirichlet)
