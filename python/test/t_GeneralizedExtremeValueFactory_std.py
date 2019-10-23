#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

ot.PlatformInfo.SetNumericalPrecision(5)
size = 10000


for distribution in [ot.GeneralizedExtremeValue(2.0, 1.5, -0.15),
                     ot.GeneralizedExtremeValue(2.0, 1.5,  0.0)]:
                     # ot.GeneralizedExtremeValue(2.0, 1.5,  0.15)]:

    sample = distribution.getSample(size)
    factory = ot.GeneralizedExtremeValueFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))

    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedGeneralizedExtremeValue = factory.buildAsGeneralizedExtremeValue(
        sample)
    print("GeneralizedExtremeValue          =", distribution)
    print("Estimated GeneralizedExtremeValue=",
          estimatedGeneralizedExtremeValue)
    estimatedGeneralizedExtremeValue = factory.buildAsGeneralizedExtremeValue()
    print("Default GeneralizedExtremeValue=", estimatedGeneralizedExtremeValue)
    estimatedGeneralizedExtremeValue = factory.buildAsGeneralizedExtremeValue(
        distribution.getParameter())
    print("GeneralizedExtremeValue from parameters=",
          estimatedGeneralizedExtremeValue)
