"""
Create a gaussian process from spectral density
===============================================
"""
# %% 

# %%
# In this example we are going to build a gaussian process from its spectral density.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# define a spectral model
amplitude = [1.0, 2.0]
scale = [4.0, 5.0]
spatialCorrelation = ot.CorrelationMatrix(2)
spatialCorrelation[0,1] = 0.8
mySpectralModel = ot.CauchyModel(scale, amplitude, spatialCorrelation)

# %%
# define a mesh
myTimeGrid =  ot.RegularGrid(0.0, 0.1, 20)

# %%
# create the process
process = ot.SpectralGaussianProcess(mySpectralModel, myTimeGrid)
print(process)

# %%
# draw a sample
sample = process.getSample(6)
graph = sample.drawMarginal(0)
view = viewer.View(graph)
plt.show()
