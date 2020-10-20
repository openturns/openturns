"""
Fit a normal distribution
=========================
"""
# %% 

# %%
# In this example we are going to estimate the parameters of a gaussian distribution from a sample.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Create data
sample = ot.Normal().getSample(50)

# %%
# Estimate a normal distribution
distribution = ot.NormalFactory().build(sample)
print(distribution)

# %%
# Draw fitted distribution
graph = distribution.drawPDF()
view = viewer.View(graph)
plt.show()
