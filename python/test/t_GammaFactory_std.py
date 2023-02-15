#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

size = 10000

# factory.build from distribution sample
distribution = ot.Gamma(0.2, 1.0, 1.0)
sample = distribution.getSample(size)
factory = ot.GammaFactory()
estimatedDistribution = factory.build(sample)
ott.assert_almost_equal(estimatedDistribution, distribution, 0.13, 0.0)

# factory.build from distribution (with other parameters) sample
distribution = ot.Gamma(2.3, 1.0, 1.0)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
ott.assert_almost_equal(estimatedDistribution, distribution, 0.07, 0.0)

# factory.build from different sample of the same distribution
distribution = ot.Gamma(2.3, 1.0, 1.0)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
ott.assert_almost_equal(estimatedDistribution, distribution, 0.07, 0.0)

# factory.build default
defaultDistribution = ot.Gamma(1.0, 1.0, 0.0)
estimatedDistribution = factory.build()
ott.assert_almost_equal(estimatedDistribution, defaultDistribution, 0.0, 0.0)

# factory.build from parameters
estimatedDistribution = factory.build(distribution.getParameter())
ott.assert_almost_equal(estimatedDistribution, distribution, 0.0, 0.0)

# factory.buildAs from distribution sample
estimatedGamma = factory.buildAsGamma(sample)
ott.assert_almost_equal(estimatedGamma, distribution, 0.07, 0.0)

# factory.buildAs default
estimatedGamma = factory.buildAsGamma()
ott.assert_almost_equal(estimatedGamma, defaultDistribution, 0.0, 0.0)

# factory.buildAs from distribution parameters
estimatedGamma = factory.buildAsGamma(distribution.getParameter())
ott.assert_almost_equal(estimatedGamma, distribution, 0.0, 0.0)
