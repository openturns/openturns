"""
Create a Monte Carlo design of experiments
==========================================
"""
# %%

# %%
# In this example we are going to create a MonteCarlo probabilistic design experiment.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Define underlying distribution, size
distribution = ot.Normal(2)
size = 50

# %%
# Create the design
experiment = ot.MonteCarloExperiment(distribution, size)
sample = experiment.generate()

# %%
# Plot the design
graph = ot.Graph("MC design", "x1", "x2", True, "")
cloud = ot.Cloud(sample, "blue", "fsquare", "")
graph.add(cloud)
view = viewer.View(graph)
plt.show()
