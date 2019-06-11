#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
ot.Log.Show(ot.Log.ALL)
distribution = ot.Frechet(2.5, 2.0, -1.5)
size = 10000
sample = distribution.getSample(size)
factory = ot.FrechetFactory()
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
typedEstimatedDistribution = factory.buildAsFrechet(sample)
print('Typed estimated distribution      =', typedEstimatedDistribution)
defaultTypedDistribution = factory.buildAsFrechet()
print('Default typed distribution        =', defaultTypedDistribution)
typedFromParameterDistribution = factory.buildAsFrechet(
    distribution.getParameter())
print('Typed distribution from parameters=', typedFromParameterDistribution)
