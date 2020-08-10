# ---
# jupyter:
#   jupytext:
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.5.1
#   kernelspec:
#     display_name: Python 3
#     language: python
#     name: python3
# ---

# %%
"""
Visualize empirical CDF
=======================
"""
# %% 

# %%
# In this example we are going to draw the empirical CDF of an unidimensional sample.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# Then create a sample from a gaussian distribution.

# %%
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)

# %%
# We draw the empirical CDF based on the `UserDefined` distribution. By default, the `drawCDF` method requires no input argument.

# %%
distribution = ot.UserDefined(sample)
graph = distribution.drawCDF()
view = viewer.View(graph)

# %%
# If required, we can specify the interval that we want to draw. In the following example, these bounds are computed from the minimum and the maximum of the sample.

# %%
xmin = sample.getMin()[0] - 2.0
xmax = sample.getMax()[0] + 2.0
graph = ot.UserDefined(sample).drawCDF(xmin, xmax)
view = viewer.View(graph)
plt.show()
