#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    dim = 3
    mean = Point(dim)
    sigma = Point(dim)
    R = CorrelationMatrix(dim)
    for i in range(dim):
        mean[i] = i + 0.5
        sigma[i] = 2 * i + 1.0
        for j in range(i):
            R[i, j] = 0.5 * (1.0 + i) / dim
    distribution = Normal(mean, sigma, R)
    size = 10000
    sample = distribution.getSample(size)
    factory = NormalFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedNormal = factory.buildAsNormal(sample)
    print("Normal          =", distribution)
    print("Estimated normal=", estimatedNormal)
    estimatedNormal = factory.buildAsNormal()
    print("Default normal=", estimatedNormal)
    estimatedNormal = factory.buildAsNormal(
        distribution.getParameter())
    print("Normal from parameters=", estimatedNormal)
    # 1D estimation
    result = factory.buildEstimator(sample[:, 0])
    estimatedDistribution = result.getDistribution()
    print('Estimated distribution            =', repr(estimatedDistribution))
    parameterDistribution = result.getParameterDistribution()
    print('Parameter distribution            =', parameterDistribution)
except:
    import sys
    print("t_NormalFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
