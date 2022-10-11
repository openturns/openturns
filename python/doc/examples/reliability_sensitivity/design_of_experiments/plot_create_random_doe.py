"""
Create a random design of experiments
=====================================
"""
# %%
# Abstract
# --------
#
# Random designs of experiments can be generated from probability distributions.

# %%
import openturns as ot
import openturns.viewer as viewer
import math as m
from matplotlib import pylab as plt

ot.Log.Show(ot.Log.NONE)


# %%
# We create the underlying distribution: a standard 2-dimensional normal distribution.
distribution = ot.Normal(2)
size = 50

# %%
# The Monte Carlo design of experiments
# -------------------------------------
#
# We build the experiment  with the :class:`~openturns.MonteCarloExperiment` class :
experiment = ot.MonteCarloExperiment(distribution, size)
sample = experiment.generate()

# %%
# We draw the design of experiments as a :class:`~openturns.Cloud`
graph = ot.Graph("Monte Carlo design", r"$x_1$", r"$x_2$", True, "")
cloud = ot.Cloud(sample, "blue", "fsquare", "")
graph.add(cloud)
view = viewer.View(graph)

# %%
# Latin Hypercube Sampling
# ------------------------
#
# We build the LHS design of experiments with the :class:`~openturns.LHSExperiment` class :
experiment = ot.LHSExperiment(distribution, size)
sample = experiment.generate()

# %%
# We draw the LHS design of experiments as a cloud :
graph = ot.Graph("LHS design", r"$x_1$", r"$x_2$", True, "")
cloud = ot.Cloud(sample, "blue", "fsquare", "")
graph.add(cloud)
view = viewer.View(graph)

# %%
# Display figures
plt.show()
