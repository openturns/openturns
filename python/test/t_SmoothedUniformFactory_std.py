#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

distribution = ot.SmoothedUniform(-0.5, 1.5, 0.5)
size = 10000
sample = distribution.getSample(size)
factory = otexp.SmoothedUniformFactory()
estimatedDistribution = factory.build(sample)
ott.assert_almost_equal(estimatedDistribution, distribution, 1e-2, 1e-2)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedSmoothedUniform = factory.buildAsSmoothedUniform(sample)
print("SmoothedUniform          =", distribution)
print("Estimated uniform=", estimatedSmoothedUniform)
ott.assert_almost_equal(estimatedSmoothedUniform.getParameter(), distribution.getParameter(), 1e-2, 1e-2)
estimatedSmoothedUniform = factory.buildAsSmoothedUniform()
print("Default uniform=", estimatedSmoothedUniform)
estimatedSmoothedUniform = factory.buildAsSmoothedUniform(distribution.getParameter())
print("SmoothedUniform from parameters=", estimatedSmoothedUniform)
