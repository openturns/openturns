#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    distribution = Bernoulli(0.7)
    size = 10000
    sample = distribution.getSample(size)
    factory = BernoulliFactory()
    estimatedDistribution = factory.build(sample)
    print("distribution=", repr(distribution))
    print("Estimated distribution=", repr(estimatedDistribution))
    estimatedDistribution = factory.build()
    print("Default distribution=", estimatedDistribution)
    estimatedDistribution = factory.build(
        distribution.getParameter())
    print("Distribution from parameters=", estimatedDistribution)
    estimatedBernoulli = factory.buildAsBernoulli(sample)
    print("Bernoulli          =", distribution)
    print("Estimated Bernoulli=", estimatedBernoulli)
    estimatedBernoulli = factory.buildAsBernoulli()
    print("Default Bernoulli=", estimatedBernoulli)
    estimatedBernoulli = factory.buildAsBernoulli(
        distribution.getParameter())
    print("Bernoulli from parameters=", estimatedBernoulli)
    result = factory.buildEstimator(sample)
    estimatedDistribution = result.getDistribution()
    print('Estimated distribution            =', repr(estimatedDistribution))
    parameterDistribution = result.getParameterDistribution()
    print('Parameter distribution            =', parameterDistribution)
except:
    import sys
    print("t_BernoulliFactory_std.py", sys.exc_info()[0], sys.exc_info()[1])
