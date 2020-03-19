#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
from math import sqrt

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
# Various estimators
estimatedDistribution = factory.build(sample)
print('Estimated distribution with default estimator =', estimatedDistribution)
estimatedDistribution = factory.buildMethodOfMoments(sample)
print('Estimated distribution with moments=', estimatedDistribution)
estimatedDistribution = factory.buildMethodOfLikelihoodMaximization(sample)
print('Estimated distribution with likelihoodMax.=', estimatedDistribution)

# Build method of moments
print("Build method of moments")
size = 10000
distribution = ot.FisherSnedecor(4.5, 8.4)
sample = distribution.getSample(size)
factory = ot.FisherSnedecorFactory()
estimatedTN = factory.buildMethodOfMoments(sample)
exact_mu = sample.computeMean()[0]
exact_sigma2 = sample.computeCovariance()[0,0]
computed_mu = estimatedTN.getMean()[0]
computed_sigma2 = estimatedTN.getCovariance()[0,0]
ott.assert_almost_equal(exact_mu, computed_mu, 1.e-15, 0.0)
ott.assert_almost_equal(exact_sigma2, computed_sigma2, 1.e-15, 0.0)

# Build method of likelihood maximization
print("Build method of likelihood maximization")
size = 10000
distribution = ot.FisherSnedecor(4.5, 8.4)
sample = distribution.getSample(size)
factory = ot.FisherSnedecorFactory()
estimatedTN = factory.buildMethodOfLikelihoodMaximization(sample)
exact_d1 = distribution.getD1()
exact_d2= distribution.getD2()
computed_d1 = estimatedTN.getD1()
computed_d2 = estimatedTN.getD2()
ott.assert_almost_equal(exact_d1, computed_d1, 0.0, 100.0 / sqrt(size))
ott.assert_almost_equal(exact_d2, computed_d2, 0.0, 100.0 / sqrt(size))
