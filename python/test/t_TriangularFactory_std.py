#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

distribution = ot.Triangular(1.0, 2.5, 4.0)
size = 10000
sample = distribution.getSample(size)
factory = ot.TriangularFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedTriangular = factory.buildAsTriangular(sample)
print("Triangular          =", distribution)
print("Estimated triangular=", estimatedTriangular)
estimatedTriangular = factory.buildAsTriangular()
print("Default triangular=", estimatedTriangular)
estimatedTriangular = factory.buildAsTriangular(
    distribution.getParameter())
print("Triangular from parameters=", estimatedTriangular)
