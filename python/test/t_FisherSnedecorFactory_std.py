#!/usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(5)
distribution = ot.FisherSnedecor(4.5, 8.4)
size = 10000
sample = distribution.getSample(size)
factory = ot.FisherSnedecorFactory()
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
typedEstimatedDistribution = factory.buildAsFisherSnedecor(sample)
print('Typed estimated distribution      =', typedEstimatedDistribution)
defaultTypedDistribution = factory.buildAsFisherSnedecor()
print('Default typed distribution        =', defaultTypedDistribution)
typedFromParameterDistribution = factory.buildAsFisherSnedecor(
    distribution.getParameter())
print('Typed distribution from parameters=', typedFromParameterDistribution)
