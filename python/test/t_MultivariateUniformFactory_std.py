#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Build from a sample (1D)
distribution = otexp.MultivariateUniform([0.5], [2.5])
size = 10000
sample = distribution.getSample(size)
factory = otexp.MultivariateUniformFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
# Estimated bounds should be close to true bounds
assert estimatedDistribution.getDimension() == 1
# build default
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
assert estimatedDistribution.getDimension() == 1
# build from parameters
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
ott.assert_almost_equal(estimatedDistribution.getParameter(), distribution.getParameter())
# buildAsMultivariateUniform
estimatedUniform = factory.buildAsMultivariateUniform(sample)
print("MultivariateUniform          =", distribution)
print("Estimated MultivariateUniform=", estimatedUniform)
assert estimatedUniform.getDimension() == 1
ott.assert_almost_equal(estimatedUniform.getA()[0], 0.5, 1e-2)
ott.assert_almost_equal(estimatedUniform.getB()[0], 2.5, 1e-2)
# buildAsMultivariateUniform default
estimatedUniform = factory.buildAsMultivariateUniform()
print("Default MultivariateUniform=", estimatedUniform)
assert estimatedUniform.getDimension() == 1
ott.assert_almost_equal(estimatedUniform.getA(), [-1.0])
ott.assert_almost_equal(estimatedUniform.getB(), [1.0])
# buildAsMultivariateUniform from parameters
estimatedUniform = factory.buildAsMultivariateUniform(distribution.getParameter())
print("MultivariateUniform from parameters=", estimatedUniform)
ott.assert_almost_equal(estimatedUniform.getA(), distribution.getA())
ott.assert_almost_equal(estimatedUniform.getB(), distribution.getB())

# Build from a sample (2D)
distribution2d = otexp.MultivariateUniform([0.0, 1.0], [2.0, 5.0])
sample2d = distribution2d.getSample(size)
estimated2d = factory.buildAsMultivariateUniform(sample2d)
print("2D distribution=", distribution2d)
print("2D estimated=", estimated2d)
assert estimated2d.getDimension() == 2
ott.assert_almost_equal(estimated2d.getA()[0], 0.0, 1e-2, 1e-2)
ott.assert_almost_equal(estimated2d.getA()[1], 1.0, 1e-2, 1e-2)
ott.assert_almost_equal(estimated2d.getB()[0], 2.0, 1e-2, 1e-2)
ott.assert_almost_equal(estimated2d.getB()[1], 5.0, 1e-2, 1e-2)

# Build estimator
result = factory.buildEstimator(sample)
print("Estimator result=", result.getDistribution())
assert result.getDistribution().getDimension() == 1
