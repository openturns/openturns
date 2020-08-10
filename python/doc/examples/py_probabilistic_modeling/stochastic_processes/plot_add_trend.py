"""
Add a trend to a process
========================
"""

# %%
# In this example we are going to add a trend to a process.
#
# The TrendTransform class enables to create a new process Y from a process X (no hypothesis on X needed):
#
# .. math::Y(\omega, t) = X(\omega, t) + f(t):math:``

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# Create a process
grid = ot.RegularGrid(0.0, 0.1, 10)
amplitude=[5.0]
scale=[0.2]
covModel = ot.ExponentialModel(scale, amplitude)
X = ot.GaussianProcess(covModel, grid)

# %%
# Draw a sample
sample = X.getSample(6)
sample.setName('X')
graph = sample.drawMarginal(0)
view = viewer.View(graph)

# %%
# Define a trend function
f = ot.SymbolicFunction(['t'], ['30*t'])
fTrend = ot.TrendTransform(f, grid)

# %%
# Add it to the process
Y = ot.CompositeProcess(fTrend, X)
Y.setName('Y')

# %%
# Draw a sample
sample = Y.getSample(6)
sample.setName('Y')
graph = sample.drawMarginal(0)
view = viewer.View(graph)
plt.show()
