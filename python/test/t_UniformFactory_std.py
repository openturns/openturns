#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


distribution = ot.Uniform(1.0, 2.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.UniformFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(
    distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedUniform = factory.buildAsUniform(sample)
print("Uniform          =", distribution)
print("Estimated uniform=", estimatedUniform)
estimatedUniform = factory.buildAsUniform()
print("Default uniform=", estimatedUniform)
estimatedUniform = factory.buildAsUniform(
    distribution.getParameter())
print("Uniform from parameters=", estimatedUniform)
