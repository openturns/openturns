#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

factory = otexp.SquaredNormalFactory()

# Negative tests: rejection paths for buildMethodOfMoments
with ott.assert_raises(Exception):
    badSample = ot.Sample([[x] for x in [1.0, 2.0, -1.0, 4.0]])
    factory.build(badSample)

with ott.assert_raises(Exception):
    # Sample with m1^2 - m2/2 < 0 (extreme outlier inflates variance relative to squared mean)
    badSample = ot.Sample([[1e-6], [1e-6], [1e-6], [100.0]])
    factory.build(badSample)

# Build a squared normal distribution
distribution = ot.SquaredNormal(1.0, 2.0)
print("distribution=", distribution)

# Generate a sample
size = 10000
sample = distribution.getSample(size)

# Estimate the distribution using the factory
estimatedDistribution = factory.build(sample)
print("Estimated distribution=", estimatedDistribution)

# Build from parameters
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)

# Build default
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)

# Build as native
estimatedSquaredNormal = factory.buildAsSquaredNormal(sample)
print("Estimated SquaredNormal=", estimatedSquaredNormal)

# Verify moments of estimated distribution
estimatedMean = estimatedSquaredNormal.getMean()[0]
estimatedCov = estimatedSquaredNormal.getCovariance()[0, 0]
sampleMean = sample.computeMean()[0]
sampleCov = sample.computeCovariance()[0, 0]
print("Sample mean=", sampleMean, " estimated mean=", estimatedMean)
print("Sample variance=", sampleCov, " estimated variance=", estimatedCov)
ott.assert_almost_equal(sampleMean, estimatedMean, 0.5, 0.0)
ott.assert_almost_equal(sampleCov, estimatedCov, 1.0, 0.0)
