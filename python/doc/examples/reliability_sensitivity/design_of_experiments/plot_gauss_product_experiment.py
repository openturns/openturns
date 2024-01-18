"""
Create a Gauss product design
=============================
"""
# %%
# In this example we are going to create a deterministic weighted design experiment using Gauss product.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)

# %%
# Define the underlying distribution, degrees
distribution = ot.JointDistribution(
    [ot.Exponential(), ot.Triangular(-1.0, -0.5, 1.0)]
)
marginalSizes = [15, 8]

# %%
# Create the design
experiment = ot.GaussProductExperiment(distribution, marginalSizes)
sample = experiment.generate()

# %%
# Plot the design
graph = ot.Graph("GP design", "x1", "x2", True, "")
cloud = ot.Cloud(sample, "blue", "fsquare", "")
graph.add(cloud)
view = viewer.View(graph)
plt.show()
