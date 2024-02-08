#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from openturns.usecases import coles

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.INFO)

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
pref_mle = [7.44573, 0.184112, 30.0]
ott.assert_almost_equal(inf_dist.getParameter(), pref_mle, 1e-2, 1e-2)
print("parameter dist=", estimator_mle.getParameterDistribution())
print(estimator_mle.getParameterDistribution().getCovariance())
cov_ref = [[0.920412, -0.0655531, 0],
           [-0.0655531, 0.0102358, 0],
           [0, 0, 0]]
ott.assert_almost_equal(ot.Matrix(estimator_mle.getParameterDistribution().getCovariance()), ot.Matrix(cov_ref), 2e-3, 1e-5)
ott.assert_almost_equal(estimator_mle.getLogLikelihood(), -485.094)

# specific check for covariates
covariates = ot.Sample([[i + 1] for i in range(sample.getSize())])
sigmaIndices = [0]  # linear
xiIndices = []  # stationary
sigmaLink = ot.SymbolicFunction(["x"], ["exp(x)"])
estimator_covariate = factory.buildCovariates(sample, u, covariates, sigmaIndices, xiIndices, sigmaLink)
beta = estimator_covariate.getOptimalParameter()
print("beta*=", beta)
ott.assert_almost_equal(beta, [1.9582e-05, 1.80441, 0.197766], 1e-2, 1e-2)
beta_dist = estimator_covariate.getParameterDistribution()
print("beta dist=", beta_dist)
graph_sigma1d = estimator_covariate.drawParameterFunction1D(0)
graph_sigma2d = estimator_covariate.drawParameterFunction2D(0)
graph_q_sigma1d = estimator_covariate.drawQuantileFunction1D(0.9)
graph_q_sigma2d = estimator_covariate.drawQuantileFunction2D(0.9)

# functional
timeStamps = ot.Sample([[i + 1] for i in range(sample.getSize())])
constant = ot.SymbolicFunction(["t"], ["1.0"])
basis = ot.Basis([ot.SymbolicFunction(["t"], ["t"]), constant])
sigmaIndices = [0, 1]  # linear
xiIndices = [1]  # stationary
sigmaLink = ot.SymbolicFunction(["x"], ["exp(x)"])
estimator_timevar = factory.buildTimeVarying(sample, u, timeStamps, basis, sigmaIndices, xiIndices, sigmaLink)
beta = estimator_timevar.getOptimalParameter()
print("beta*=", beta)
ott.assert_almost_equal(beta, [0.343272, 1.80443, 0.197766], 1e-2, 1e-2)
beta_dist = estimator_timevar.getParameterDistribution()
print("beta dist=", beta_dist)
t0 = timeStamps[0, 0]
dist0 = estimator_timevar.getDistribution(t0)
print(dist0)
assert dist0.getImplementation().__class__.__name__ == "GeneralizedPareto"
graph_param = estimator_timevar.drawParameterFunction(0)
graph_quantile = estimator_timevar.drawQuantileFunction(0.99)

# specific check for return level, see coles2001 p86
xm = factory.buildReturnLevelEstimator(estimator_mle, 100.0 * 365.0, sample)
print("xm=", xm)
ott.assert_almost_equal(xm.getMean(), [106.284], 1e-2, 1e-2)
ott.assert_almost_equal(xm.getCovariance()[0, 0], 433.145, 1e-2, 1e-2)

# specific check for return level via profile likelihood
estimator_prof_rl = factory.buildReturnLevelProfileLikelihoodEstimator(sample, u, 100.0 * 365.0)
print(estimator_prof_rl)
zm = estimator_prof_rl.getParameter()
try:
    ci = estimator_prof_rl.getParameterConfidenceInterval()
    print("profile return level estimator zm=", zm, ci)
    assert [zm] in ci, "zm should be inside confidence interval"
except Exception as exception:
    print(exception)
ott.assert_almost_equal(ci.getLowerBound(), [80.8575], 1e-2, 1e-2)
ott.assert_almost_equal(ci.getUpperBound(), [184.988], 1e-2, 1e-2)
graph = estimator_prof_rl.drawProfileLikelihoodFunction()

# profile MLE (xi)
estimator_prof_mle = factory.buildMethodOfXiProfileLikelihoodEstimator(sample, u)
inf_dist = estimator_prof_mle.getDistribution()
print("Estimated GPD (profile MLE)=", inf_dist)
ott.assert_almost_equal(inf_dist.getParameter(), pref_mle, 1e-2, 1e-2)

# specific check for profile likelihood
xi = estimator_prof_mle.getParameter()
ci = estimator_prof_mle.getParameterConfidenceInterval()
print("profile MLE estimator xi=", xi, ci)
assert [xi] in ci, "xi should be inside confidence interval"
graph = estimator_prof_mle.drawProfileLikelihoodFunction()

# parameter stability
u_range = ot.Interval(0.5, 50.0)
graph = factory.drawParameterThresholdStability(sample, u_range)
