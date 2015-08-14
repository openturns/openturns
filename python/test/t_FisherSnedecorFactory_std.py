#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = FisherSnedecor(4.5, 8.4)
    size = 10000
    sample = distribution.getSample(size)
    factory = FisherSnedecorFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParametersCollection())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedFisherSnedecor = factory.buildAsFisherSnedecor(sample)
    print("FisherSnedecor          =", distribution)
    print("Estimated fisherSnedecor=", estimatedFisherSnedecor)
    estimatedFisherSnedecor = factory.buildAsFisherSnedecor()
    print("Default fisherSnedecor=", estimatedFisherSnedecor)
    estimatedFisherSnedecor = factory.buildAsFisherSnedecor(
        distribution.getParametersCollection())
    print("FisherSnedecor from parameters=", estimatedFisherSnedecor)

except:
    import sys
    print("t_FisherSnedecorFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
