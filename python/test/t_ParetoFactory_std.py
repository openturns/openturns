#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()
ot.PlatformInfo.SetNumericalPrecision(3)

distribution = ot.Pareto(2.0, 3.5, -1.0)
size = 10000
sample = distribution.getSample(size)
factory = ot.ParetoFactory()
print("distribution=", repr(distribution))
print("Estimated distribution (Moments)=", factory.buildMethodOfMoments(sample))
print(
    "Estimated distribution (MLE)=", factory.buildMethodOfLikelihoodMaximization(sample)
)
print("Estimated distribution (LSQ)=", factory.buildMethodOfLeastSquares(sample))
estimatedDistribution = factory.build()
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedPareto = factory.buildAsPareto(sample)
print("Estimated pareto=", estimatedPareto)
estimatedPareto = factory.buildAsPareto()
print("Default pareto=", estimatedPareto)
estimatedPareto = factory.buildAsPareto(distribution.getParameter())
print("Pareto from parameters=", estimatedPareto)
