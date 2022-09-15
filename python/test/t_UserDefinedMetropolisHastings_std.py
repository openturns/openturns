#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()

ot.RandomGenerator.SetSeed(1)

# Sample from a target distribution defined through its log-PDF
# (defined up to some additive constant) and its support:

log_density = ot.SymbolicFunction('x', 'log(2 + sin(x)^2)')
support = ot.Interval([0.0], [2.0 * m.pi])

# Apply a Metropolis adjusted Langevin algorithm (MALA).

initialState = [2.5]
proposal = ot.Normal()
h = 0.1
std_deviation = m.sqrt(h)


# The mean of the proposal normal distribution is the current state,
# but moved according to the derivative of the target log-density.

def python_link(x):
    derivative_log_density = log_density.getGradient().gradient(x)[0, 0]
    mean = x[0] + h / 2 * derivative_log_density
    return [mean, std_deviation]
link = ot.PythonFunction(1, 2, python_link)
mala = otexp.UserDefinedMetropolisHastings(log_density, support, initialState, proposal, link)

# %%
# Get a sample

sampleSize = 100000
sample = mala.getSample(sampleSize)


# %%
# Compute sample mean and median

mean_ref = m.pi
sample_mean = sample.computeMean()[0]
ott.assert_almost_equal(sample_mean, mean_ref, 1e-2, 0.0)

median_ref = m.pi
sample_median = sample.computeQuantile(0.5)[0]
ott.assert_almost_equal(sample_median, median_ref, 1e-1, 0.0)
