"""
Estimate a probability with Latin Hypercube Sampling
====================================================
"""

# %%
# In this example we show how to use the `LHS` algorithm to estimate the probability of an event.
# For illustration purpose we shall consider the axial stressed beam :ref:`use-case-stressed-beam`. 

# %% 
from __future__ import print_function
import openturns as ot
import numpy as np
from use_cases import use_case_stressed_beam_exemple

# Load the use case axial stressed beam
uc_beam = use_case_stressed_beam_exemple.axial_stressed_beam()

# %%
# Create a LHS algorithm.

# %%
algo = ot.LHS(uc_beam.event)
algo.setMaximumCoefficientOfVariation(0.05)
algo.setMaximumOuterSampling(int(1e5))
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)


