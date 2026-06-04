"""
Use a randomized QMC algorithm
==============================
"""

# %%
# In this example we are going to estimate a failure probability on the :ref:`stressed beam <use-case-stressed-beam>`.

# %%
from openturns.usecases import stressed_beam
import openturns as ot


# %%
# We load the data class containing the probabilistic modeling of the beam.
sm = stressed_beam.AxialStressedBeam()

# %%
# We load the joint probability distribution of the input parameters :
distribution = sm.distribution

# %%
# We load the model as well,
model = sm.model

# %%
# We create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Less(), 0.0)

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
algo.setMaximumOuterSampling(int(1e4))
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print("Pf=", probability)
