#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

distribution = ot.Binomial(15, 0.7)
size = 10000
sample = distribution.getSample(size)
factory = ot.BinomialFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
print("Estimated distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", repr(estimatedDistribution))
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", repr(estimatedDistribution))
estimatedBinomial = factory.buildAsBinomial(sample)
print("Binomial          =", distribution)
print("Estimated binomial=", estimatedBinomial)
estimatedBinomial = factory.buildAsBinomial()
print("Default binomial=", estimatedBinomial)
estimatedBinomial = factory.buildAsBinomial(distribution.getParameter())
print("Binomial from parameters=", estimatedBinomial)

# gh #3379: all-zero sample must keep known n
factoryKnown = ot.BinomialFactory()
factoryKnown.setKnownParameter([0], [4.0])
estimatedKnown = factoryKnown.build(ot.Sample([[0.0], [0.0]]))
ott.assert_almost_equal(estimatedKnown.getParameter(), [4.0, 0.0])

# all-zero sample with known p only: n=0 has likelihood 1
factoryKnownP = ot.BinomialFactory()
factoryKnownP.setKnownParameter([1], [0.3])
estimatedKnownP = factoryKnownP.build(ot.Sample([[0.0], [0.0]]))
ott.assert_almost_equal(estimatedKnownP.getParameter(), [0.0, 0.3])

# non-integral known n is rejected
factoryBadN = ot.BinomialFactory()
factoryBadN.setKnownParameter([0], [4.2])
with ott.assert_raises(TypeError):
    factoryBadN.build(ot.Sample([[0.0], [0.0]]))

# out-of-range known n is rejected
factoryHugeN = ot.BinomialFactory()
factoryHugeN.setKnownParameter([0], [1.0e30])
with ott.assert_raises(TypeError):
    factoryHugeN.build(ot.Sample([[0.0], [0.0]]))

# non-finite known p is rejected
factoryBadP = ot.BinomialFactory()
factoryBadP.setKnownParameter([1], [float("nan")])
with ott.assert_raises(TypeError):
    factoryBadP.build(ot.Sample([[0.0], [0.0]]))

# out-of-range known p is rejected
factoryBadP2 = ot.BinomialFactory()
factoryBadP2.setKnownParameter([1], [1.5])
with ott.assert_raises(TypeError):
    factoryBadP2.build(ot.Sample([[0.0], [0.0]]))
