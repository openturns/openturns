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
Create a copula
===============
"""
# %% 

# %%
# In this example we are going to create a Normal copula from a correlation matrix.
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# create the distribution
R = ot.CorrelationMatrix(3)
R[0, 1] = 0.25
R[1, 2] = 0.25
copula = ot.NormalCopula(R)
print(copula)
