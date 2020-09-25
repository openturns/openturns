"""
Create a maximum distribution
=============================
"""
# %%
# In this example we are going to build the distribution of the maximum of independent distributions.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# create a collection of distribution
distribution1 = ot.Normal()
distribution2 = ot.Uniform(-1.0, 2.0)
distColl = [distribution1, distribution2]

# %%
# create the distribution
distribution = ot.MaximumDistribution(distColl)
print(distribution)

# %%
# draw PDF
graph = distribution.drawPDF()
view = viewer.View(graph)
if not viewer._noshow:
    plt.show()
