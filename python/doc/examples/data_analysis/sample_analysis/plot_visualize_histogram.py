"""
Visualize an histogram from data
================================
"""

# %%
# In this example we are going to a plot an histogram from an unidimensional sample.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

# %%
# Create data to visualize
size = 500
normal = ot.Normal(1)
sample = normal.getSample(size)

# %%
# Draw histogram
graph = ot.HistogramFactory().build(sample).drawPDF()
view = viewer.View(graph)
plt.show()
