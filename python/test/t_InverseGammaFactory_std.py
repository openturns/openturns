#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Build an inverse gamma distribution
distribution = ot.InverseGamma(5.0, 3.0)
print("distribution=", distribution)

# Generate a sample
size = 10000
sample = distribution.getSample(size)

# Estimate the distribution using the factory
factory = otexp.InverseGammaFactory()
estimatedDistribution = factory.build(sample)
print("Estimated distribution=", estimatedDistribution)

# Build from parameters
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)

# Build default
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)

# Build as native
estimatedInverseGamma = factory.buildAsInverseGamma(sample)
print("Estimated InverseGamma=", estimatedInverseGamma)

# Verify moments of estimated distribution
estimatedMean = estimatedInverseGamma.getMean()[0]
estimatedCov = estimatedInverseGamma.getCovariance()[0, 0]
sampleMean = sample.computeMean()[0]
sampleCov = sample.computeCovariance()[0, 0]
print("Sample mean=", sampleMean, " estimated mean=", estimatedMean)
print("Sample variance=", sampleCov, " estimated variance=", estimatedCov)
ott.assert_almost_equal(sampleMean, estimatedMean, 1e-6, 0.0)
ott.assert_almost_equal(sampleCov, estimatedCov, 1e-6, 0.0)
