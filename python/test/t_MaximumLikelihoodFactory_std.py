#! /usr/bin/env python

import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(3)
distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
print(f"distribution={distribution}")
sample = distribution.getSample(1000)
factory = ot.MaximumLikelihoodFactory(ot.Beta())
infDist = factory.build(sample)
print(f"estimated distribution={infDist}")

# set (a,b) out of (r, t, a, b)
factory.setKnownParameter([-1.0, 1.0], [2, 3])
infDist = factory.build(sample)
print(f"estimated distribution with bounds={infDist}")

factory = ot.MaximumLikelihoodFactory(ot.Exponential())
factory.setKnownParameter([0.1], [1])
print(factory.build())
print(factory.build([3, 0]))

# test BuildEstimator
distribution = ot.Gumbel(0.5, -0.5)
sample = distribution.getSample(1000)
for isRegular in [True, False]:
    fResult = ot.MaximumLikelihoodFactory.BuildEstimator(
        ot.GumbelFactory(), sample, isRegular
    )
    paramDist = fResult.getParameterDistribution()
    print(f"estimator(isRegular={isRegular}): {str(paramDist)[:100]}")
