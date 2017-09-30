#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

distribution = ot.Trapezoidal(1.0, 2.3, 4.5, 5.0)
size = 10000
sample = distribution.getSample(size)
factory = ot.TrapezoidalFactory()
estimatedDistribution = factory.build(sample)
print("distribution=", repr(distribution))
oldPrecision = ot.PlatformInfo.GetNumericalPrecision()
ot.PlatformInfo.SetNumericalPrecision(4)
print("Estimated distribution=", repr(estimatedDistribution))
ot.PlatformInfo.SetNumericalPrecision(oldPrecision)
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedTrapezoidal = factory.buildAsTrapezoidal(sample)
print("Trapezoidal          =", distribution)
oldPrecision = ot.PlatformInfo.GetNumericalPrecision()
ot.PlatformInfo.SetNumericalPrecision(4)
print("Estimated trapezoidal=", estimatedTrapezoidal)
ot.PlatformInfo.SetNumericalPrecision(oldPrecision)
estimatedTrapezoidal = factory.buildAsTrapezoidal()
print("Default trapezoidal=", estimatedTrapezoidal)
estimatedTrapezoidal = factory.buildAsTrapezoidal(distribution.getParameter())
print("Trapezoidal from parameters=", estimatedTrapezoidal)
sample = [[0.0]] * size
estimatedDistribution = factory.build(sample)
print("Estimated distribution=", repr(estimatedDistribution))
sample = [[1.0]] * size
estimatedDistribution = factory.build(sample)
print("Estimated distribution=", repr(estimatedDistribution))
