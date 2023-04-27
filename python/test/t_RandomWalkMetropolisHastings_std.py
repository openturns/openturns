#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(0)

mu = 5000.0
prior = ot.Normal(mu, 1.0)
initialState = [0.0]
instrumental = ot.Normal(0.0, 1.0)
sampler = ot.RandomWalkMetropolisHastings(prior, initialState, instrumental)
sampler.setBurnIn(1000)
s1 = sampler.getSample(2000)
ott.assert_almost_equal(s1[sampler.getBurnIn():].computeMean()[0], mu, 1e-2, 0.0)

data = ot.Sample(
    [
        [53, 1],
        [57, 1],
        [58, 1],
        [63, 1],
        [66, 0],
        [67, 0],
        [67, 0],
        [67, 0],
        [68, 0],
        [69, 0],
        [70, 0],
        [70, 0],
        [70, 1],
        [70, 1],
        [72, 0],
        [73, 0],
        [75, 0],
        [75, 1],
        [76, 0],
        [76, 0],
        [78, 0],
        [79, 0],
        [81, 0]
    ]
)

data.setDescription(["Temp. (°F)", "Failure"])
print(data)


fun = ot.SymbolicFunction(
    ["alpha", "beta", "x"], ["exp(alpha + beta * x) / (1 + exp(alpha + beta * x))"]
)
linkFunction = ot.ParametricFunction(fun, [2], [0.0])
instrumental = ot.Normal([0.0] * 2, [0.5, 0.05], ot.IdentityMatrix(2))

target = ot.ComposedDistribution([ot.Uniform(-100.0, 100.0)] * 2)
rwmh = ot.RandomWalkMetropolisHastings(target, [0.0] * 2, instrumental)
conditional = ot.Bernoulli()
observations = data[:, 1]
covariates = data[:, 0]
rwmh.setLikelihood(conditional, observations, linkFunction, covariates)

# try to generate a sample
sample = rwmh.getSample(10000)
mu = sample[rwmh.getBurnIn():].computeMean()
sigma = sample.computeStandardDeviation()
print("mu=", mu, "sigma=", sigma)
ott.assert_almost_equal(mu, [10.3854, -0.164881])
ott.assert_almost_equal(sigma, [3.51975, 0.0517796])

print("acceptance rate=", rwmh.getAcceptanceRate())
ott.assert_almost_equal(rwmh.getAcceptanceRate(), 0.3345)

# from 1532
fullModel = ot.SymbolicFunction(["x", "theta"], ["theta", "1.0"])
model = ot.ParametricFunction(fullModel, [0], [1.0])
prior = ot.Normal(0.0, 1.0)
prior.setDescription(["theta"])
instrumental = ot.Normal(0.0, 1.0)
thetaTrue = [2.0]
# We choose the most favorable initial state: the true parameter value.
initialState = thetaTrue
conditional = ot.Normal()  # the log-likelihood is Gaussian
# 503 observations: OK
obsSize = 503
ot.RandomGenerator.SetSeed(0)
y_obs = ot.Normal(thetaTrue[0], 1.0).getSample(obsSize)
x_obs = y_obs
RWMHsampler = ot.RandomWalkMetropolisHastings(prior, initialState, instrumental)
RWMHsampler.setLikelihood(conditional, y_obs, model, x_obs)
print(
    "Log-likelihood of thetaTrue = {!r}".format(
        RWMHsampler.computeLogLikelihood(thetaTrue)
    )
)
real_503 = RWMHsampler.getRealization()
print("With 503 observations, getRealization() produces {!r}".format(real_503[0]))
ott.assert_almost_equal(real_503[0], 2.0)
# 504 observations: not OK
obsSize = 504
ot.RandomGenerator.SetSeed(0)
y_obs = ot.Normal(thetaTrue[0], 1.0).getSample(obsSize)
x_obs = y_obs
RWMHsampler = ot.RandomWalkMetropolisHastings(prior, initialState, instrumental)
RWMHsampler.setLikelihood(conditional, y_obs, model, x_obs)
print(
    "Log-likelihood of thetaTrue = {!r}".format(
        RWMHsampler.computeLogLikelihood(thetaTrue)
    )
)
# produces an error with current master branch
real_504 = RWMHsampler.getRealization()
print("With 504 observations, getRealization() produces {!r}".format(real_504[0]))
ott.assert_almost_equal(real_504[0], 2.0)


# this example throws in ot 1.19 as it tries to evaluate the likelihood outside the support of the prior
# see MetropolisHastingsImplementation::computeLogPosterior
obs = ot.TruncatedNormal(0.5, 0.5, 0.0, 10.0).getSample(50)
likelihood = ot.GeneralizedPareto()
prior = ot.ComposedDistribution([ot.LogUniform(-1.40, 4.0), ot.Normal(), ot.Normal()])
proposals = [
    ot.Uniform(
        -prior.getMarginal(k).getStandardDeviation()[0],
        +prior.getMarginal(k).getStandardDeviation()[0],
    )
    for k in range(prior.getDimension())
]
initialState = prior.getMean()
mh_coll = [
    ot.RandomWalkMetropolisHastings(prior, initialState, proposals[i], [i])
    for i in range(2)
]
for mh in mh_coll:
    mh.setLikelihood(likelihood, obs)
sampler = ot.Gibbs(mh_coll)
parameters_sample = sampler.getSample(2000)
