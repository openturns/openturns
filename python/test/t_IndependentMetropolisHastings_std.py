#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
from math import pi

ot.TESTPREAMBLE()

# %%
# Draw the unnormalized probability density
# -----------------------------------------

ot.RandomGenerator.SetSeed(1)
lower_bound = 0.0
print(lower_bound)
upper_bound = 2.0 * pi
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



