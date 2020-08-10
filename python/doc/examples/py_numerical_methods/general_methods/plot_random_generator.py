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
Random generator parametrization
================================
"""
# %% 

# %%
# The seed of the pseudo random generator is initialized to 0 when the module is imported.
#
# It means the same script will yield the same results across several replications.
#
# It is also possible to initialize the random generator differently:
#
# - by setting a seed
# - by setting the complete generator state

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import os
import time

# %%
# Example 0: using a fixed seed
ot.RandomGenerator.SetSeed(77)

# %%
# Example 1: using the python process id
ot.RandomGenerator.SetSeed(os.getpid())

# %%
# Example 2: using the time in milliseconds
ot.RandomGenerator.SetSeed(int(1000*time.time()))

# %%
# Example 3: using a previously saved generator state

# use the random generator
ot.Normal().getSample(100)
# save the generator state
particularState = ot.RandomGenerator.GetState()
ot.Normal().getRealization()

# %%
# load the generator state
ot.RandomGenerator.SetState(particularState)
ot.Normal().getRealization()
