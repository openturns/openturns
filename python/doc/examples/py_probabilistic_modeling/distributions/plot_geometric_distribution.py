"""
Create a geometric distribution
===============================
"""
# %%
# In this example we are going to create a geometric distribution with parameter :math:`p = 0.7`.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

distribution = ot.Geometric(0.7)
print(distribution)

# %%
sample = distribution.getSample(10)
print(sample)

# %%
graph = distribution.drawCDF()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()
