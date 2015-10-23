#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = AliMikhailHaqCopula(0.5)
    size = 1000
    sample = distribution.getSample(size)
    factory = AliMikhailHaqCopulaFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedAliMikhailHaqCopula = factory.buildAsAliMikhailHaqCopula(sample)
    print("Distribution          =", distribution)
    print("Estimated AliMikhailHaqCopula=", estimatedAliMikhailHaqCopula)
    estimatedAliMikhailHaqCopula = factory.buildAsAliMikhailHaqCopula()
    print("Default AliMikhailHaqCopula=", estimatedAliMikhailHaqCopula)
    estimatedAliMikhailHaqCopula = factory.buildAsAliMikhailHaqCopula(
        distribution.getParameter())
    print("AliMikhailHaqCopula from parameters=", estimatedAliMikhailHaqCopula)

except:
    import sys
    print("t_AliMikhailHaqFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
