#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(0)

# %%
# Test independentMetropolisHastings on Beta-Binomial conjugate model

# Define Beta-binomial model
a, b, lower, upper = 1.0, 1.0, 0.0, 1.0
n, p = 10, 0.5
prior = ot.Beta(a, b, lower, upper)
model = ot.Binomial(n, p)

# %%
# Simulate data and compute analytical posterior
X = model.getSample(1)
posterior = ot.Beta(a + X[0,0], b + n - X[0,0], lower, upper)

# %%
# Define IMH sampler
initialState = [p]
imh_sampler = ot.IndependentMetropolisHastings(
    prior, initialState, ot.Uniform(-1.0, 1.0), [0]
)
slf = ot.SymbolicFunction(['x'], [str(n),'x'])
imh_sampler.setLikelihood(model, X, slf)

# %%
# Generate posterior distribution sample
N = 10000
Xsample = imh_sampler.getSample(N)

# %%
# Compare empirical to theoretical moments

ott.assert_almost_equal(Xsample.computeMean(), posterior.getMean(), 0.0, 10.0 / math.sqrt(N))
ott.assert_almost_equal(Xsample.computeStandardDeviation(), posterior.getStandardDeviation(), 0.0, 10.0 / math.sqrt(N))

# %%
# Draw the unnormalized probability density
# -----------------------------------------

ot.RandomGenerator.SetSeed(1)
lower_bound = 0.0
print(lower_bound)
upper_bound = 2.0 * math.pi
print(upper_bound)

# %%
# Independent Metropolis-Hastings
# -------------------------------
# Let us use a mixture distribution to approximate the target distribution.
#
# This approximation will serve as the instrumental distribution
# in the independent Metropolis-Hastings algorithm.

exp = ot.Exponential(1.0)
normal = ot.Normal(5.3, 0.4)
instrumentalDistribution = ot.Mixture([exp, normal], [0.9, 0.1])
print(instrumentalDistribution.getWeights())


# %%
# MetropolisHastings classes expect to receive the logarithm to the target density.

log_density = ot.SymbolicFunction(
    "x", "log(2 + sin(x)^2) - (2 + cos(3*x)^3 + sin(2*x)^3) * x"
)

initialState = ot.Point([3.0])  # not important in this case
support = ot.Interval([lower_bound], [upper_bound])
independentMH = ot.IndependentMetropolisHastings(
    log_density, support, initialState, instrumentalDistribution, [0]
)

print(independentMH.getRealization())

# %%
# Get a sample

sampleSize = 1000000
sample = independentMH.getSample(sampleSize)


# %%
# Compute posterior mean

mean_ref = 1.22498
posterior_mean = sample.computeMean()[0]
ott.assert_almost_equal(posterior_mean, mean_ref, 1e-5, 0)

std_ref = 1.61558
posterior_std = sample.computeStandardDeviation()[0]
ott.assert_almost_equal(posterior_std, std_ref, 1e-5, 0)
