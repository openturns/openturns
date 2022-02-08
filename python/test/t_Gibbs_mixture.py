#! /usr/bin/env python

# We sample the from the posterior distribution of the parameters of a mixture model.
# This example is drawn from Example 9.2 from *Monte-Carlo Statistical methods* by Robert and Casella (2004).

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import numpy as np

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(100)

# Sample data with :math:`\mu_0 = 0` and :math:`\mu_1 = 2.7`.
N = 500
p = 0.3

mu0 = 0.0
mu1 = 2.7
nor0 = ot.Normal(mu0, 1.0)
nor1 = ot.Normal(mu1, 1.0)
true_distribution = ot.Mixture([nor0, nor1], [1 - p, p])
observations = np.array(true_distribution.getSample(500))

def nor0post(pt):
    z = np.array(pt)[2:]
    x0 = observations[z==0]
    mu0 = x0.sum() / (0.1 + len(x0))
    sigma0 = 1.0 / (0.1 + len(x0))
    return [mu0, sigma0]

def nor1post(pt):
    z = np.array(pt)[2:]
    x1 = observations[z==1]
    mu1 = x1.sum() / (0.1 + len(x1))
    sigma1 = 1.0 / (0.1 + len(x1))
    return [mu1, sigma1]

def zpost(pt):
    mu0 = pt[0]
    mu1 = pt[1]
    term1 = p * np.exp(- (observations - mu1)**2 / 2)
    term0 = (1.0 - p) * np.exp(- (observations - mu0)**2 / 2)
    res = term1 / (term1 + term0)
    # output must be a 1d list or array in order to create a PythonFunction
    return res.reshape(-1)

nor0posterior = ot.PythonFunction(2 + N, 2, nor0post)
nor1posterior = ot.PythonFunction(2 + N, 2, nor1post)
zposterior = ot.PythonFunction(2 + N, N, zpost)

# We can now construct the Gibbs algorithm
initialState = [0.0] * (N + 2)
sampler0 = ot.RandomVectorMetropolisHastings(ot.RandomVector(ot.Normal()), initialState, [0], nor0posterior)
sampler1 = ot.RandomVectorMetropolisHastings(ot.RandomVector(ot.Normal()), initialState, [1], nor1posterior)
big_bernoulli = ot.ComposedDistribution([ot.Bernoulli()] * N)
sampler2 = ot.RandomVectorMetropolisHastings(ot.RandomVector(big_bernoulli), initialState, range(2, N + 2), zposterior)
gibbs = ot.Gibbs([sampler0, sampler1, sampler2])

# Run the Gibbs algorithm
s = gibbs.getSample(10000)

# Extract the relevant marginals: the first (:math:`mu_0`) and the second (:math:`\mu_1`).
posterior_sample = s[:, 0:2]
mean = posterior_sample.computeMean()
stddev = posterior_sample.computeStandardDeviation()
print(mean, stddev)
ott.assert_almost_equal(mean, [0.824453, -0.0215115])
ott.assert_almost_equal(stddev, [0.00197192, 0.968657])
