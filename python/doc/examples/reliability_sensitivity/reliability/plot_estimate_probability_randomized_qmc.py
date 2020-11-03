"""
Estimate a probability using randomized QMC
===========================================
"""
# %%
# In this example we are going to estimate a failure probability on the :ref:`cantilever beam <use-case-cantilever-beam>`.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We load the data class containing the probabilistic modeling of the beam.
from openturns.usecases import cantilever_beam as cantilever_beam
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
event = ot.ThresholdEvent(G, ot.Greater(), 30.0)

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
