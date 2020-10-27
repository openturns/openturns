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

# More involved test: the sample distribution does not fit the factory

# The distributions used :
myFrechet = ot.Frechet(1.0, 1.0, 0.0)
myGumbel = ot.Gumbel(1.0,3.0)
# We build our mixture sample of size 2*1000=2000.
mixtureSample = ot.Sample()
sampleFrechet = myFrechet.getSample(1000)
sampleGumbel = myGumbel.getSample(1000)
mixtureSample.add(sampleFrechet)
mixtureSample.add(sampleGumbel)
# Build on the mixture sample
typedEstimatedFromMixtureSample = factory.buildAsFrechet(mixtureSample)
print('Estimated dist from mixture sample=', typedEstimatedFromMixtureSample)