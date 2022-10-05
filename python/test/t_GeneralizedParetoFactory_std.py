#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)
ot.PlatformInfo.SetNumericalPrecision(4)

xi = [-0.75, 0.0, 0.75]
size = 10000
covariance = ot.CovarianceMatrix(2)
factory = ot.GeneralizedParetoFactory()
for i in range(3):
    distribution = ot.GeneralizedPareto(2.5, xi[i], 0.5)
    sample = distribution.getSample(size)
    # Distribution estimatedDistribution(factory.build(sample, covariance))
    estimatedDistribution = factory.build(sample)
    print("Distribution          =", distribution)
    print("Estimated distribution=", estimatedDistribution)
    estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto(sample)
    print("GeneralizedPareto          =", distribution)
    print("Estimated generalizedPareto=", estimatedGeneralizedPareto)
    try:
        estimatedDistribution = factory.buildMethodOfMoments(sample)
        print("GeneralizedPareto from moments=", estimatedDistribution)
    except Exception:
        pass

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

