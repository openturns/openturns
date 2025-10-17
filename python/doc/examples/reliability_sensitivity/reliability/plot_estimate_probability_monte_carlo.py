"""
Estimate a probability with Monte Carlo
=======================================
"""

# %%
# In this example we estimate a probability by means of a simulation algorithm, the Monte-Carlo algorithm.
# To do this, we need the classes `MonteCarloExperiment` and `ProbabilitySimulationAlgorithm`.
# We consider the :ref:`axial stressed beam <use-case-stressed-beam>` example.


# %%
from openturns.usecases import stressed_beam
import openturns as ot


# %%
# We load the model from the usecases module :
sm = stressed_beam.AxialStressedBeam()

# %%
# We get the joint distribution of the parameters.
distribution = sm.distribution

# %%
# The model is also stored in the data class :
model = sm.model

# %%
# We create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Less(), 0.0)

# %%
# Create a Monte Carlo algorithm.

# %%
experiment = ot.MonteCarloExperiment()
algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
algo.setMaximumCoefficientOfVariation(0.05)
algo.setMaximumOuterSampling(int(1e5))
algo.setKeepSample(True)
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print("Pf=", probability)

# %%
# Print the size of used sample and compare it with the budget size (1e5)
print(f"Sample used to get the probability = {len(algo.getOutputSample())}")
