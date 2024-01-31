#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from openturns.usecases import coles

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.INFO)

size = 10000
distribution = ot.GeneralizedExtremeValue(2.0, 1.5, -0.15)
sample = distribution.getSample(size)
factory = ot.GeneralizedExtremeValueFactory()

estimated_bic_selection = factory.build(sample)
print("Estimated distribution (BIC selection)=", repr(estimated_bic_selection))
ott.assert_almost_equal(
    estimated_bic_selection.getParameter(), distribution.getParameter(), 5e-2, 5e-2
)

default_distribution = factory.build()
print("Default distribution=", default_distribution)
assert (
    default_distribution.getParameter() == ot.GeneralizedExtremeValue().getParameter()
)

estimated_param = factory.build(distribution.getParameter())
print("Distribution from parameters=", estimated_param)
assert estimated_param.getParameter() == distribution.getParameter(), "wrong parameters"

estimated_bic_selection2 = factory.buildAsGeneralizedExtremeValue(sample)
print("Estimated GeneralizedExtremeValue (BIC selection)=", estimated_bic_selection2)
assert estimated_bic_selection2.__class__.__name__ == "GeneralizedExtremeValue"

default_distribution2 = factory.buildAsGeneralizedExtremeValue()
print("Default GeneralizedExtremeValue=", default_distribution2)
assert (
    default_distribution2.getParameter() == ot.GeneralizedExtremeValue().getParameter()
)
assert default_distribution2.__class__.__name__ == "GeneralizedExtremeValue"

estimated_param2 = factory.buildAsGeneralizedExtremeValue(distribution.getParameter())
print("GeneralizedExtremeValue from parameters=", estimated_param2)
assert (
    estimated_param2.getParameter() == distribution.getParameter()
), "wrong parameters"
assert estimated_param2.__class__.__name__ == "GeneralizedExtremeValue"

estimated_mle = factory.buildMethodOfLikelihoodMaximization(sample)
print("Estimated GeneralizedExtremeValue (MLE)=", estimated_mle)
ott.assert_almost_equal(
    estimated_mle.getParameter(), distribution.getParameter(), 1e-2, 1e-2
)

estimated_prof_mle = factory.buildMethodOfXiProfileLikelihood(sample, 1)
print("Estimated GeneralizedExtremeValue (profile MLE)=", estimated_prof_mle)
ott.assert_almost_equal(
    estimated_mle.getParameter(), distribution.getParameter(), 1e-2, 1e-2
)

# try MLE on multiple parameters
pdist = ot.JointDistribution(
    [
        ot.Normal(0.0, 2.0),
        ot.TruncatedNormal(1.0, 2.0, 1e-6, 10.0),
        ot.Uniform(-0.9, 0.9),
    ]
)
for p in pdist.getSample(10):
    distribution = factory.buildAsGeneralizedExtremeValue(p)
    print("distribution=", repr(distribution))

    sample_p = distribution.getSample(size)

    estimated_mle = factory.buildMethodOfLikelihoodMaximization(sample_p)
    print("Estimated GeneralizedExtremeValue (MLE)=", estimated_mle)
    mu, sigma, xi = estimated_mle.getParameter()
    ott.assert_almost_equal(mu, distribution.getMu(), 5e-2, 2e-1)
    ott.assert_almost_equal(sigma, distribution.getSigma(), 1e-2, 5e-2)
    ott.assert_almost_equal(xi, distribution.getXi(), 1e-2, 5e-2)

# check parameters distribution is gaussian
estimator = factory.buildEstimator(sample)
print(estimator)
assert (
    estimator.getParameterDistribution().getImplementation().__class__.__name__
    == "Normal"
)

estimator_prof_mle = factory.buildMethodOfXiProfileLikelihoodEstimator(sample)
assert (
    estimator_prof_mle.getParameterDistribution().getImplementation().__class__.__name__
    == "Normal"
)

# specific check for profile likelihood
xi = estimator_prof_mle.getParameter()
try:
    ci = estimator_prof_mle.getParameterConfidenceInterval()
    print("profile MLE estimator xi=", xi, ci)
    assert [xi] in ci, "xi should be inside confidence interval"
except Exception as exception:
    print(exception)

graph = estimator_prof_mle.drawProfileLikelihoodFunction()

# specific check for R maxima
sample_rmax = coles.Coles().venice[:, 1:]
print("sample_rmax=", sample_rmax)
estimated_rmax = factory.buildMethodOfLikelihoodMaximization(sample_rmax)
print("Estimated GeneralizedExtremeValue (R maxima)=", estimated_rmax)
# These reference values give a better likelihood than the ones given by Coles
ott.assert_almost_equal(
    estimated_rmax.getParameter(), [116.868, 11.8544, -0.107984], 1e-2, 1e-2
)

estimator_rmax = factory.buildMethodOfLikelihoodMaximizationEstimator(sample_rmax)
assert (
    estimator_rmax.getParameterDistribution().getImplementation().__class__.__name__
    == "Normal"
)

# specific check for covariates
fremantle = coles.Coles().fremantle
timeStamps = fremantle[:, 0]
muIndices = [0]  # linear
sigmaIndices = []  # stationary
xiIndices = []  # stationary
muLink = ot.SymbolicFunction(["x"], ["1.0*x"])
estimator_covariate = factory.buildCovariates(fremantle[:, 1], timeStamps, muIndices, sigmaIndices, xiIndices, muLink)
beta = estimator_covariate.getOptimalParameter()
print("beta*=", beta)
ott.assert_almost_equal(beta, [0.00203333, -2.4751, 0.124301, -0.125008], 1e-2, 1e-2)
beta_dist = estimator_covariate.getParameterDistribution()
print("beta dist=", beta_dist)
assert beta_dist.getImplementation().__class__.__name__ == "Normal", "wrong distribution"
cov_ref = [
    [3.11388e-09, -6.05561e-06, -1.99653e-08, 2.10413e-07],
    [-6.05561e-06, 0.0117791, 3.93182e-05, -0.000414466],
    [-1.99653e-08, 3.93182e-05, 1.51924e-06, -6.06597e-06],
    [2.10413e-07, -0.000414466, -6.06597e-06, 8.53944e-05]]
ott.assert_almost_equal(ot.Matrix(beta_dist.getCovariance()), ot.Matrix(cov_ref), 1e-3, 1e-3)
graph_mu1d = estimator_covariate.drawParameterFunction1D(0)
graph_mu2d = estimator_covariate.drawParameterFunction2D(0)
graph_q_mu1d = estimator_covariate.drawQuantileFunction1D(0.9)
graph_q_mu2d = estimator_covariate.drawQuantileFunction2D(0.9)

# functional
t0 = fremantle[0, 0]  # year of first record
constant = ot.SymbolicFunction(["t"], ["1.0"])
basis = ot.Basis([constant, ot.SymbolicFunction(["t"], ["t"])])
muIndices = [0, 1]  # linear
sigmaIndices = [0]  # stationary
xiIndices = [0]  # stationary
estimator_timevar = factory.buildTimeVarying(fremantle[:, 1], timeStamps, basis, muIndices, sigmaIndices, xiIndices)
beta = estimator_timevar.getOptimalParameter()
print("beta*=", beta)
ott.assert_almost_equal(beta, [1.38216, 0.187033, 0.124317, -0.125086], 1e-2, 1e-2)
beta_dist = estimator_timevar.getParameterDistribution()
print("beta dist=", beta_dist)
assert (beta_dist.getImplementation().__class__.__name__ == "Normal"), "wrong distribution"
cov_ref = [
    [9.49362e-06, -1.34867e-05, 1.43649e-06, -1.51103e-05],
    [-1.34867e-05, 2.65117e-05, -1.85843e-06, 1.93419e-05],
    [1.43649e-06, -1.85843e-06, 1.52237e-06, -6.07642e-06],
    [-1.51103e-05, 1.93419e-05, -6.07642e-06, 8.52864e-05]]
ott.assert_almost_equal(ot.Matrix(beta_dist.getCovariance()), ot.Matrix(cov_ref), 1e-3, 1e-3)
dist0 = estimator_timevar.getDistribution(t0)
print(dist0)
assert dist0.getImplementation().__class__.__name__ == "GeneralizedExtremeValue"
graph_param = estimator_timevar.drawParameterFunction(0)
graph_quantile = estimator_timevar.drawQuantileFunction(0.99)

# specific check for model selection
estimator_mle = factory.buildMethodOfLikelihoodMaximizationEstimator(fremantle[:, 1])
result_deviance = ot.HypothesisTest.LikelihoodRatioTest(
    3, estimator_mle.getLogLikelihood(), 4, estimator_timevar.getLogLikelihood()
)
print(result_deviance)
ott.assert_almost_equal(result_deviance.getStatistic(), 12.674, 1e-2, 1e-2)
assert not result_deviance.getBinaryQualityMeasure(), "H0 (stationary model) accepted"

# specific check for return level
zm = factory.buildReturnLevelEstimator(estimator_mle, 10.0)
print("zm=", zm)
ott.assert_almost_equal(zm, ot.Normal(1.73377, 0.0228792), 1e-2, 1e-2)

# specific check for return level via profile likelihood
estimator_prof_rl = factory.buildReturnLevelXiProfileLikelihoodEstimator(
    fremantle[:, 1], 10.0
)
print(estimator_prof_rl)
zm = estimator_prof_rl.getParameter()
try:
    ci = estimator_prof_rl.getParameterConfidenceInterval()
    print("profile return level estimator zm=", zm, ci)
    assert [zm] in ci, "zm should be inside confidence interval"
except Exception as exception:
    print(exception)
graph = estimator_prof_rl.drawProfileLikelihoodFunction()
