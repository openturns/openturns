"""
Draw 2-d distribution graphs
============================
"""
# %%
# In this example we are going to draw PDF and CDF of a bidimensional distribution.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# assume a 2-d distribution
distribution = ot.Normal([0.0, 2.0], [1.0, 0.6], ot.CorrelationMatrix(2))
print(distribution)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# draw CDF
graph = distribution.drawCDF()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()
