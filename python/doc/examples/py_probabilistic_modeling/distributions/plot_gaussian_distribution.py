"""
Create a gaussian distribution
==============================
"""
# %%
# In this example we are going to create a gaussian distribution with parameters
#
# .. math::
#     \mu = 2.2, \sigma = 0.6
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
ot.Log.Show(ot.Log.NONE)

distribution = ot.Normal(2.2, 0.6)
print(distribution)

# %%
sample = distribution.getSample(10)
print(sample)

# %%
graph = distribution.drawPDF()
view = viewer.View(graph)
plt.show()
