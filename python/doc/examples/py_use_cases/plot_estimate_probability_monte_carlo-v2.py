"""
Estimate a probability with Monte Carlo
=======================================
"""
# %%
# In this example we estimate a probability by means of a simulation algorithm, the Monte-Carlo algorithm. To do this, we need the classes `MonteCarloExperiment` and `ProbabilitySimulationAlgorithm`.
# For illustration purpose we shall consider the axial stressed beam :ref:`use-case-stressed-beam`. 

# %%
from __future__ import print_function
import openturns as ot
from use_cases import use_case_stressed_beam_exemple
import openturns.viewer as viewer
from matplotlib import pylab as plt

# Load the use case axial stressed beam
uc_beam = use_case_stressed_beam_exemple.axial_stressed_beam()

# %%
experiment = ot.MonteCarloExperiment()
algo = ot.ProbabilitySimulationAlgorithm(uc_beam.event, experiment)
algo.setMaximumCoefficientOfVariation(0.05)
algo.setMaximumOuterSampling(int(1e5))
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)
