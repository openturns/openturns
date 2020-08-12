"""
Probabilistic design of experiments
===================================
"""
# %%
# In this example we are going to expose the available probabilistic design of experiments generated according to a specified distribution and a specified number of points.

# %%
from __future__ import print_function
import openturns as ot
import math as m
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Create the target distribution
distribution = ot.Normal(2)
N = 300

# %%
# 1. Monte Carlo
experiment = ot.MonteCarloExperiment(distribution, N)
sample = experiment.generate()
graph = ot.Cloud(sample)
view = viewer.View(graph)

# %%
# 2. LHS
experiment = ot.LHSExperiment(distribution, N)
sample = experiment.generate()
graph = ot.Cloud(sample)
view = viewer.View(graph)
plt.show()
