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

estimated_prof_mle = factory.buildMethodOfProfileLikelihoodMaximization(sample)
print("Estimated GeneralizedExtremeValue (profile MLE)=", estimated_prof_mle)
ott.assert_almost_equal(
    estimated_mle.getParameter(), distribution.getParameter(), 1e-2, 1e-2
)

# try MLE on multiple parameters
pdist = ot.ComposedDistribution(
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

estimator_prof_mle = factory.buildMethodOfProfileLikelihoodMaximizationEstimator(sample)
assert (
    estimator_prof_mle.getParameterDistribution().getImplementation().__class__.__name__
    == "Normal"
)

# specific check for profile likelihood
xi = estimator_prof_mle.getParameter()
try:
    ci = estimator_prof_mle.getParameterConfidenceInterval()
    print("profile MLE estimator xsi=", xi, ci)
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

# specific check for time-varying
fremantle = coles.Coles().fremantle
t0 = fremantle[0, 0]  # year of first record
mesh = fremantle[:, 0]
constant = ot.SymbolicFunction(["t"], ["1.0"])
basis_mu = ot.Basis([constant, ot.SymbolicFunction(["t"], ["t"])])  # linear trend
basis_sigma = ot.Basis([constant])  # stationary
basis_xi = ot.Basis([constant])  # stationary
basis_coll = [basis_mu, basis_sigma, basis_xi]
estimator_timevar = factory.buildTimeVarying(fremantle[:, 1], mesh, basis_coll)
beta = estimator_timevar.getOptimalParameter()
print("beta*=", beta)
ott.assert_almost_equal(beta, [1.38216, 0.187033, 0.124317, -0.125086], 1e-2, 1e-2)
print("beta dist=", estimator_timevar.getParameterDistribution())
assert (
    estimator_timevar.getParameterDistribution().getImplementation().__class__.__name__
    == "Normal"
)
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
print("zm=", zm.getMean())
ott.assert_almost_equal(zm.getMean(), [1.73377], 1e-2, 1e-2)

# specific check for return level via profile likelihood
estimator_prof_rl = factory.buildReturnLevelProfileLikelihoodEstimator(
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
