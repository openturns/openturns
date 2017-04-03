#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Dirichlet(Point([0.2, 0.3, 0.4, 0.5]))
    size = 10000
    sample = distribution.getSample(size)
    factory = DirichletFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDirichlet = factory.buildAsDirichlet(sample)
    print("Dirichlet          =", distribution)
    print("Estimated dirichlet=", estimatedDirichlet)
    estimatedDirichlet = factory.buildAsDirichlet()
    print("Default dirichlet=", estimatedDirichlet)

except:
    import sys
    print("t_DirichletFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
