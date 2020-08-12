"""
Create a gaussian process from a cov. model
===========================================
"""
# %%
# In this example we are going to build a gaussian process from its covariance model.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# define a covariance model
defaultDimension = 1
# Amplitude values
amplitude = [1.0]*defaultDimension
# Scale values
scale = [1.0]*defaultDimension
# Covariance model
myModel = ot.AbsoluteExponential(scale, amplitude)

# %%
# define a mesh
tmin = 0.0
step = 0.1
n = 11
myTimeGrid = ot.RegularGrid(tmin, step, n)

# %%
# create the process
process = ot.GaussianProcess(myModel, myTimeGrid)
print(process)

# %%
# draw a sample
sample = process.getSample(6)
graph = sample.drawMarginal(0)
view = viewer.View(graph)
plt.show()
