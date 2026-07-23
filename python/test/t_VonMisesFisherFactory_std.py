#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

size = 10000

# factory.build from distribution sample
distribution = otexp.VonMisesFisher(ot.Point([1.0, 0.0]), 0.5)
sample = distribution.getSample(size)
factory = otexp.VonMisesFisherFactory()
estimatedDistribution = factory.build(sample)
ott.assert_almost_equal(estimatedDistribution, distribution, 0.3, 0.05)

# factory.build from another distribution sample
distribution = otexp.VonMisesFisher(ot.Point([0.0, 1.0]), 2.0)
sample = distribution.getSample(size)
estimatedDistribution = factory.build(sample)
ott.assert_almost_equal(estimatedDistribution, distribution, 0.3, 0.05)

# factory.build default
defaultDistribution = otexp.VonMisesFisher()
estimatedDistribution = factory.build()
ott.assert_almost_equal(estimatedDistribution, defaultDistribution, 0.0, 0.0)

# factory.build from parameters
estimatedDistribution = factory.build(distribution.getParameter())
ott.assert_almost_equal(estimatedDistribution, distribution, 0.0, 0.0)

# factory.buildAs from distribution sample
estimatedVonMisesFisher = factory.buildAsVonMisesFisher(sample)
ott.assert_almost_equal(estimatedVonMisesFisher, distribution, 0.3, 0.05)

# factory.buildAs default
estimatedVonMisesFisher = factory.buildAsVonMisesFisher()
ott.assert_almost_equal(estimatedVonMisesFisher, defaultDistribution, 0.0, 0.0)

# factory.buildAs from distribution parameters
estimatedVonMisesFisher = factory.buildAsVonMisesFisher(distribution.getParameter())
ott.assert_almost_equal(estimatedVonMisesFisher, distribution, 0.0, 0.0)

# __str__
strRepr = factory.__repr__()
assert "VonMisesFisherFactory" in strRepr
strStr = factory.__str__()
assert "VonMisesFisherFactory" in strStr

# Build from too few points should fail
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[0.0, 0.0]]))

# Build from wrong dimension should fail
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[0.0], [1.0]]))

# Build from invalid parameters should fail
with ott.assert_raises(TypeError):
    factory.build([1.0, 0.0, -1.0])
with ott.assert_raises(TypeError):
    factory.build([0.0, 0.0, 1.0])
with ott.assert_raises(TypeError):
    factory.build([1.0])
