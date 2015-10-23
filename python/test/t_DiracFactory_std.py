#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Dirac(0.7)
    size = 10000
    sample = distribution.getSample(size)
    factory = DiracFactory()
    covariance = CovarianceMatrix()
    # Distribution estimatedDistribution(factory.build(sample, covariance))
    estimatedDistribution = factory.build(sample)
    print("Distribution          =", distribution)
    print("Estimated distribution=", estimatedDistribution)
    # print "Covariance=", covariance
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedDirac = factory.buildAsDirac(sample)
    print("Dirac          =", distribution)
    print("Estimated dirac=", estimatedDirac)
    estimatedDirac = factory.buildAsDirac()
    print("Default dirac=", estimatedDirac)
    estimatedDirac = factory.buildAsDirac(
        distribution.getParameter())
    print("Dirac from parameters=", estimatedDirac)

except:
    import sys
    print("t_DiracFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
