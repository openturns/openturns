#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from math import exp, inf

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
        [81, 0],
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
rwmh.setBurnIn(10000)
conditional = ot.Bernoulli()
observations = data[:, 1]
covariates = data[:, 0]
rwmh.setLikelihood(conditional, observations, linkFunction, covariates)

# try to generate a sample
sample = rwmh.getSample(100000)
mu = sample[rwmh.getBurnIn():].computeMean()
sigma = sample[rwmh.getBurnIn():].computeStandardDeviation()
print("mu=", mu, "sigma=", sigma)


# Put the posterior density in a PythonFunction
def post_den(alpha_beta):
    return [exp(rwmh.computeLogPosterior(alpha_beta))]


posterior_density = ot.PythonFunction(2, 1, post_den)

# approximate the posterior distribution
# NB: to get a good view of the mode of the posterior distribution, use:
# posterior_density.draw([14.0, -0.25], [16.0, -0.22], [100, 100])
mesher = ot.IntervalMesher([100, 100])
lowerBound = [-2.5, -0.53]
upperBound = [34.0, 0.03]
box = ot.Interval(lowerBound, upperBound)
vertices = mesher.build(box).getVertices()
densities = posterior_density(vertices).asPoint()
approximate_posterior = ot.UserDefined(vertices, densities)
approximate_mean = approximate_posterior.getMean()
approximate_std = approximate_posterior.getStandardDeviation()

ott.assert_almost_equal(mu, approximate_mean, 0.2, 0.0)
ott.assert_almost_equal(sigma, approximate_std, 0.2, 0.0)

print("acceptance rate=", rwmh.getAcceptanceRate())
ott.assert_almost_equal(
    rwmh.getAcceptanceRate(), 0.28, 0.1, 0.0
)  # Empirical acceptance rate observed when executing the code

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


# Trick RandomWalkMetropolisHastings into being a simple random walk
# with Uniform(-1, 1) step: every "proposal" is automatically accepted.
logdensity = ot.SymbolicFunction("x", "1")
support = ot.Interval([-inf], [inf])
proposal = ot.Uniform(-1.0, 1.0)
rw = ot.RandomWalkMetropolisHastings(logdensity, support, [0.0], proposal)

# The acceptance rate is 1 in this trivial case,
# so every adaptation step will multiply the adaptation factor
# by the expansion factor.
rw.setAdaptationExpansionFactor(2.0)
rw.setAdaptationPeriod(10)
rw.getSample(100)
ott.assert_almost_equal(rw.getAdaptationFactor(), 2.0**10, 0.0, 0.0)

# Check that the adaptation factor is really taken into account.
# We lengthen the adaptation period to get a longer period witout adaptation.
# We then compare the standard deviation of the step lengths with
# their theoretical standard deviation considering the 1024 adaptation factor.
rw.setAdaptationPeriod(100)
constantAdapationFactorSample = rw.getSample(99)
steps = constantAdapationFactorSample[1:] - constantAdapationFactorSample[:-1]
ref_std = ot.Uniform(-(2.0**10), 2.0**10).getStandardDeviation()
ott.assert_almost_equal(steps.computeStandardDeviation(), ref_std, 0.1, 0.0)

# At the next realization, once again the adaptation factor is multiplied by 2.
rw.getRealization()

# We now change the adaptation range
# to an interval with lower bound larger than 1 (the acceptance rate)
# This way, every adaptation step will multiply the adaptation factor
# by the shrink factor.
rw.setAdaptationRange(ot.Interval(1.1, 1.2))
rw.setAdaptationPeriod(10)
rw.setAdaptationShrinkFactor(0.5)
rw.getSample(100)
ott.assert_almost_equal(rw.getAdaptationFactor(), 2.0, 0.0, 0.0)
