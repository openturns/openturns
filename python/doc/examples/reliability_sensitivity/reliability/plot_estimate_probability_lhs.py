"""
Estimate a probability with Latin Hypercube Sampling
====================================================
"""
# %%
# In this example we show how to use the `LHS` algorithm to estimate the probability of an event. We consider the :ref:`axial stressed beam <use-case-stressed-beam>` example.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# We load the model from the usecases module :
from openturns.usecases import stressed_beam as stressed_beam
sm = stressed_beam.AxialStressedBeam()

# %%
# We get the input parameter distribution :
distribution = sm.distribution

# %%
# and get the model :
model = sm.model

# %%
# We create the event whose probability we want to estimate.

# %%
vect = ot.RandomVector(distribution)
G = ot.CompositeRandomVector(model, vect)
event = ot.ThresholdEvent(G, ot.Less(), 0.0)

# %%
# Create a LHS algorithm.

# %%
algo = ot.LHS(event)
algo.setMaximumCoefficientOfVariation(0.05)
algo.setMaximumOuterSampling(int(1e5))
algo.run()

# %%
# Retrieve results.

# %%
result = algo.getResult()
probability = result.getProbabilityEstimate()
print('Pf=', probability)

