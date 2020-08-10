"""
Generate low discrepancy sequences
==================================
"""
# %% 

# %%
# In this examples we are going to expose the available low discrepancy sequences in order to approximate some integrals.
#
# The following low-discrepancy sequences are available:
#
# - Sobol
# - Faure
# - Halton
# - reverse Halton
# - Haselgrove
#
# To illustrate these sequences we generate their first 1024 points and compare with the sequence obtained from the pseudo random generator (Merse Twister) as the latter has a higher discrepancy.

# %%
from __future__ import print_function
import openturns as ot
import math as m
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# 1. Sobol sequence
dimension = 2
size = 1024
sequence = ot.SobolSequence(2)
sample = sequence.generate(size)
graph = ot.Graph("Sobol", "", "", True, "")
cloud = ot.Cloud(sample)
graph.add(cloud)
view = viewer.View(graph)

# %%
# 2. Halton sequence
dimension = 2
sequence = ot.HaltonSequence(2)
sample = sequence.generate(size)
graph = ot.Graph("Halton", "", "", True, "")
cloud = ot.Cloud(sample)
graph.add(cloud)
view = viewer.View(graph)

# %%
# 3. Reverse Halton sequence
sequence = ot.ReverseHaltonSequence(dimension)
sample = sequence.generate(size)
print('discrepancy=', ot.LowDiscrepancySequenceImplementation.ComputeStarDiscrepancy(sample))
graph = ot.Graph("Reverse Halton", "", "", True, "")
cloud = ot.Cloud(sample)
graph.add(cloud)
view = viewer.View(graph)

# %%
# 4. Haselgrove sequence
sequence = ot.HaselgroveSequence(dimension)
sample = sequence.generate(size)
graph = ot.Graph("Haselgrove", "", "", True, "")
cloud = ot.Cloud(sample)
graph.add(cloud)
view = viewer.View(graph)

# %%
# Compare with uniform random sequence
distribution = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)]*2)
sample = distribution.getSample(size)
print('discrepancy=', ot.LowDiscrepancySequenceImplementation.ComputeStarDiscrepancy(sample))
graph = ot.Graph("Mersenne Twister", "", "", True, "")
cloud = ot.Cloud(sample)
graph.add(cloud)
view = viewer.View(graph)
plt.show()
