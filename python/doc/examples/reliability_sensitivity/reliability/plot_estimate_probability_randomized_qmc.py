"""
Use a randomized QMC algorithm
==============================
"""
# %%
# In this example we are going to estimate a failure probability on the :ref:`cantilever beam <use-case-cantilever-beam>`.

# %%
from openturns.usecases import cantilever_beam
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# We load the data class containing the probabilistic modeling of the beam.
cb = cantilever_beam.CantileverBeam()

# %%
# We load the joint probability distribution of the input parameters :
distribution = cb.distribution

# %%
# We load the model as well,
model = cb.model

# %%
# We create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Greater(), 0.3)

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
print("Pf=", probability)
