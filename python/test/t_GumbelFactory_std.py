#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

distribution = ot.Gumbel(0.5, 2.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.GumbelFactory()
print('Distribution                      =', repr(distribution))
result = factory.buildEstimator(sample)
estimatedDistribution = result.getDistribution()
print('Estimated distribution            =', repr(estimatedDistribution))
parameterDistribution = result.getParameterDistribution()
print('Parameter distribution            =', parameterDistribution)
defaultDistribution = factory.build()
print('Default distribution              =', defaultDistribution)
fromParameterDistribution = factory.build(distribution.getParameter())
print('Distribution from parameters      =', fromParameterDistribution)
typedEstimatedDistribution = factory.buildAsGumbel(sample)
print('Typed estimated distribution      =', typedEstimatedDistribution)
defaultTypedDistribution = factory.buildAsGumbel()
print('Default typed distribution        =', defaultTypedDistribution)
typedFromParameterDistribution = factory.buildAsGumbel(
    distribution.getParameter())
print('Typed distribution from parameters=', typedFromParameterDistribution)
result = factory.buildEstimator(sample, ot.GumbelAB())
estimatedDistribution = result.getDistribution()
print('Estimated distribution (AB)       =', repr(estimatedDistribution))
parameterDistribution = result.getParameterDistribution()
print('Parameter distribution (AB)       =', parameterDistribution)
