"""
Probability estimation: directional simulation-v2
=================================================
"""
# %%
# In this example we estimate a failure probability with the directional simulation algorithm provided by the `DirectionalSampling` class. We shall use the cantilever beam as a test case (see :ref:`use-case-cantilever-beam`).

# %%
# Introduction
# ------------
#
# The directional simulation algorithm  operates in the standard space based on:
#
# 1. a *root strategy* to evaluate the nearest failure point along each direction and take the contribution of each direction to the failure event probability into account. The available strategies are:
#    - `RiskyAndFast`
#    - `MediumSafe`
#    - `SafeAndSlow`
#  
# 2. a *sampling strategy* to choose directions in the standard space. The available strategies are:
#    - `RandomDirection`
#    - `OrthogonalDirection`
#
#

# %%
from __future__ import print_function
import openturns as ot
from use_cases import use_case_cantilever_beam

cb = use_case_cantilever_beam.cantilever_beam()

# %%
model = cb.model
distribution = cb.distribution

# %%
# Create the event whose probability we want to estimate.

# %%
Y = cb.Y
event = ot.ThresholdEvent(Y, ot.Greater(), 30.0)

# %%
# Root finding algorithm.

# %%
solver = ot.Brent()
rootStrategy = ot.MediumSafe(solver)

# %%
# Direction sampling algorithm.

# %%
samplingStrategy = ot.OrthogonalDirection()

# %%
# Create a simulation algorithm.

# %%
algo = ot.DirectionalSampling(event, rootStrategy, samplingStrategy)
algo.setMaximumCoefficientOfVariation(0.1)
algo.setMaximumOuterSampling(40000)
algo.setConvergenceStrategy(ot.Full())
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)
