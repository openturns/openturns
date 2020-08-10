"""
Estimate a probability using randomized QMC-v2
==============================================
"""
# %%
# In this example we are going to estimate a failure probability on the cantilever beam use case (see :ref:`use-case-cantilever-beam`).

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
# Define the low discrepancy sequence.

# %%
sequence = ot.SobolSequence()

# %%
# Create a simulation algorithm.

# %%
experiment = ot.LowDiscrepancyExperiment(sequence, 1)
experiment.setRandomize(True)
algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
algo.setMaximumCoefficientOfVariation(0.05)
algo.setMaximumOuterSampling(int(1e5))
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)
