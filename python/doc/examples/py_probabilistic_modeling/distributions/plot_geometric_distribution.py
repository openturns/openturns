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
Create a geometric distribution
===============================
"""
# %% 

# %%
# In this example we are going to create a geometric distribution with parameter :math:``p = 0.7:math:``

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
plt.show()
