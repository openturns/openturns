#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from openturns.usecases import coles

ot.TESTPREAMBLE()

size = 10000
factory = ot.GeneralizedParetoFactory()
for xi in [-0.75, 0.0, 0.75]:
    distribution = ot.GeneralizedPareto(2.5, xi, 0.5)
    sample = distribution.getSample(size)
    estimatedDistribution = factory.build(sample)
    print("Distribution          =", distribution)
    print("Estimated distribution=", estimatedDistribution)
    ott.assert_almost_equal(
        estimatedDistribution.getParameter(), distribution.getParameter(), 1e-1, 1e-1
    )
    estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto(sample)
    print("GeneralizedPareto          =", distribution)
    print("Estimated generalizedPareto=", estimatedGeneralizedPareto)
    assert estimatedGeneralizedPareto.__class__.__name__ == "GeneralizedPareto", "wrong name"

    # method of moments
    if xi <= 0.0:
        estimatedDistribution = factory.buildMethodOfMoments(sample)
        print("GeneralizedPareto from moments=", estimatedDistribution)
        ott.assert_almost_equal(estimatedDistribution.getParameter(), distribution.getParameter(), 1e-1, 1e-1)

    # exponential regression
    estimatedDistribution = factory.buildMethodOfExponentialRegression(sample)
    print("GeneralizedPareto from exponential regression=", estimatedDistribution)
    ott.assert_almost_equal(estimatedDistribution.getParameter(), distribution.getParameter(), 1e-1, 1e-1)

    # pwm
    if xi >= -0.5:
        estimatedDistribution = factory.buildMethodOfProbabilityWeightedMoments(sample)
        print("GeneralizedPareto from pwm=", estimatedDistribution)
        ott.assert_almost_equal(estimatedDistribution.getParameter(), distribution.getParameter(), 1e-1, 1e-1)

estimatedDistribution = factory.build()
ott.assert_almost_equal(estimatedDistribution.getParameter(), [1.0, 0.0, 0.0])
print("Default distribution=", estimatedDistribution)
estimatedDistribution = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimatedDistribution)
estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto()
assert estimatedGeneralizedPareto.__class__.__name__ == "GeneralizedPareto", "wrong name"
print("Default generalizedPareto=", estimatedGeneralizedPareto)
estimatedGeneralizedPareto = factory.buildAsGeneralizedPareto(
    distribution.getParameter()
)
assert estimatedGeneralizedPareto.__class__.__name__ == "GeneralizedPareto", "wrong name"
print("GeneralizedPareto from parameters=", estimatedGeneralizedPareto)
ott.assert_almost_equal(
    estimatedGeneralizedPareto.getParameter(), distribution.getParameter()
)

if not ot.PlatformInfo.HasFeature("bison"):
    exit(0)

# mean residual life
sample = coles.Coles().rain
graph = factory.drawMeanResidualLife(sample)

# MLE
u = 30.0
estimator_mle = factory.buildMethodOfLikelihoodMaximizationEstimator(sample, u)
print("MLE estimator=", estimator_mle)
inf_dist = estimator_mle.getDistribution()
print("GPD from MLE=", inf_dist)
ott.assert_almost_equal(inf_dist.getParameter(), [7.44573, 0.184112, 30.0], 1e-2, 1e-2)

# profile MLE (xi)
estimator_prof_mle = factory.buildMethodOfXiProfileLikelihoodEstimator(sample, u)
inf_dist = estimator_prof_mle.getDistribution()
print("Estimated GPD (profile MLE)=", inf_dist)
ott.assert_almost_equal(inf_dist.getParameter(), [7.44573, 0.184112, 30.0], 1e-2, 1e-2)

# specific check for profile likelihood
xi = estimator_prof_mle.getParameter()
ci = estimator_prof_mle.getParameterConfidenceInterval()
print("profile MLE estimator xi=", xi, ci)
assert [xi] in ci, "xi should be inside confidence interval"
graph = estimator_prof_mle.drawProfileLikelihoodFunction()
