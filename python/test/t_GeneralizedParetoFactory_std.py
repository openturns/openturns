#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)
PlatformInfo.SetNumericalPrecision(4)
try:
    xi = [-0.75, 0.0, 0.75]
    size = 10000
    covariance = CovarianceMatrix(2)
    factory = GeneralizedParetoFactory()
    for i in range(3):
        distribution = GeneralizedPareto(2.5, xi[i], 0.5)
        sample = distribution.getSample(size)
        # Distribution estimatedDistribution(factory.build(sample, covariance))
        estimatedDistribution = factory.build(sample)
        print("Distribution          =", distribution)
        print("Estimated distribution=", estimatedDistribution)
        estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto(sample)
        print("GeneralizedPareto          =", distribution)
        print("Estimated generalizedPareto=", estimatedGeneralizedPareto)
        # print "Covariance=", covariance
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto()
    print("Default generalizedPareto=", estimatedGeneralizedPareto)
    estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto(
        distribution.getParameter())
    print("GeneralizedPareto from parameters=", estimatedGeneralizedPareto)

except:
    import sys
    print("t_GenralizedParetoFactory_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
