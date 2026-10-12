#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
distribution = ot.VonMises(0.5, 2.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.VonMisesFactory(False)
estimatedDistribution = factory.build(sample)
print("distribution (raw)           =", repr(distribution))
print("Estimated distribution (raw) =", repr(estimatedDistribution))
factory = ot.VonMisesFactory(True)
estimatedDistribution = factory.build(sample)
print("distribution (circular)           =", repr(distribution))
print("Estimated distribution (circular) =", repr(estimatedDistribution))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedVonMises = factory.buildAsVonMises(sample)
print("VonMises          =", distribution)
print("Estimated vonMises=", estimatedVonMises)
estimatedVonMises = factory.buildAsVonMises()
print("Default vonMises=", estimatedVonMises)
estimatedVonMises = factory.buildAsVonMises(distribution.getParameter())
print("VonMises from parameters=", estimatedVonMises)

# Highly concentrated samples need bracket expansion past b = 2048
ot.RandomGenerator.SetSeed(0)
concentrated = ot.VonMises(0.0, 5000.0).getSample(2000)
concentratedEstimate = ot.VonMisesFactory().build(concentrated)
assert concentratedEstimate.getKappa() > 2048.0
