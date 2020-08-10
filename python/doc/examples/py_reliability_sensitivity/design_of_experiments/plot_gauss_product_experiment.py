"""
Create a Gauss product design
=============================
"""
# %% 

# %%
# In this example we are going to create a deterministic weighted design experiment using Gauss product.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Define the underlying distribution, degrees
distribution = ot.ComposedDistribution([ot.Exponential(), ot.Triangular(-1.0, -0.5, 1.0)])
marginalDegrees = [15, 8]

# %%
# Create the design
experiment = ot.GaussProductExperiment(distribution, marginalDegrees)
sample = experiment.generate()

# %%
# Plot the design
graph = ot.Graph("GP design", "x1", "x2", True, "")
cloud = ot.Cloud(sample, "blue", "fsquare", "")
graph.add(cloud)
view = viewer.View(graph)
plt.show()
