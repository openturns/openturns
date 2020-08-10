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
#
"""
Random vector manipulation
==========================
"""
# %% 

# %%
# The RandomVector object represents the concept of random variable.
#
# In this example we are going to exhibit some of its main methods.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# Create a random vector
dist3d = ot.Normal(3)
X = ot.RandomVector(dist3d)

# %%
# Get the dimension
X.getDimension()

# %%
# Get the mean
X.getMean()

# %%
# Get the covariance
X.getCovariance()

# %%
# Draw a sample
X.getSample(5)

# %%
# Extract a single component
X1 = X.getMarginal(1)
X1.getSample(5)

# %%
# Extract several components
X02 = X.getMarginal([0, 2])
X02.getSample(5)
