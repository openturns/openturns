#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.RandomGenerator.SetSeed(0)

ot.TESTPREAMBLE()

# %%
# Test RandomVectorMetropolisHastings on Beta-Binomial conjugate model

# Define Beta-binomial model
a, b, lower, upper = 1.0, 1.0, 0., 1.
n, p = 10, 0.5
prior = ot.RandomVector( ot.Beta(a, b, lower, upper) )
model = ot.Binomial(n, p)

# %%
# Simulate data and compute analytical posterior
X = model.getSample(1)
posterior = ot.Beta(a + X[0,0], b + n - X[0,0], lower, upper)

# %%
# Define RVMH sampler
# Here, the random vector is defined with repsect to the prior law
# this effectively implements the IMH algorithm using the prior
# as an instrumental distribution
initialState = [p]
rvmh_sampler = ot.RandomVectorMetropolisHastings(prior, initialState, [0])
slf = ot.SymbolicFunction(['x'], [str(n),'x'])
rvmh_sampler.setLikelihood(model, X, slf)

# %%
# Generate posterior distribution sample
N = 10000
Xsample = rvmh_sampler.getSample(N)

# %%
# Compare empirical to theoretical moments

ott.assert_almost_equal(Xsample.computeMean(), posterior.getMean(), 0.0, 10.0 / math.sqrt(N))
ott.assert_almost_equal(Xsample.computeStandardDeviation(), posterior.getStandardDeviation(), 0.0, 10.0 / math.sqrt(N))

# %%
randomVector = ot.RandomVector(ot.Normal())
initialState = [0.0]
sampler = ot.RandomVectorMetropolisHastings(randomVector, initialState)
x = sampler.getSample(10000)
mean = x.computeMean()
stddev = x.computeStandardDeviation()
print(mean, stddev)
# ott.assert_almost_equal(mean, [0.824453, -0.0215115])
# ott.assert_almost_equal(stddev, [0.00197192, 0.968657])

# with link function
slf = ot.SymbolicFunction(["x"], ["0.0", "0.1"])
sampler = ot.RandomVectorMetropolisHastings(randomVector, initialState, [0], slf)
x = sampler.getSample(10000)
mean = x.computeMean()
stddev = x.computeStandardDeviation()
print(mean, stddev)
ott.assert_almost_equal(mean, [0.0], 0.0, 0.002)
ott.assert_almost_equal(stddev, [0.1], 0.1, 0.0)
