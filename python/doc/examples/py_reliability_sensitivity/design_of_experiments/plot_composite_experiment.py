"""
Create a composite design of experiments
========================================
"""
# %% 

# %%
# In this example we create a deterministic design experiment with the `Composite` class.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Define position, scale
center = [0.5, 1.5]
levels = [4, 8, 16]

# %%
# Create the design
experiment = ot.Composite(center, levels)
sample = experiment.generate()

# %%
# Plot the design
graph = ot.Graph("Composite design", "x1", "x2", True, "")
cloud = ot.Cloud(sample, "blue", "fsquare", "")
graph.add(cloud)
view = viewer.View(graph)
plt.show()
