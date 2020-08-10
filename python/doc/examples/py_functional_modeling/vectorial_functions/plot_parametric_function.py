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
Create a parametric function
============================
"""
# %% 

# %%
# In this example we are going to create a parametric function:
#
# .. math:: d_{L,I}(E, F):  \mathbb{R}^2  \rightarrow \mathbb{R} :math:``
#   
# function from an existing "full" function:
#
#  d(E, F, L, I):  \mathbb{R}^4  \rightarrow \mathbb{R} :math:``

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m


# %%
# Create the function with all parameters d(E, F, L, I)
def d_func(X):
    E, F, L, I = X
    d = -F * L**3 / (3.0 * E * I)
    return [d]
beam = ot.PythonFunction(4, 1, d_func)

# %%
# Evaluate d
x = [50.0, 1.0, 10.0, 5.0]
beam(x)

# %%
# Create the indices of the frozen parameters (L,I) from the full parameter list
indices = [2, 3]

# %%
# Create the values of the frozen parameters (L,I)
referencePoint = [10.0, 5.0]

# %%
# Create the parametric function
beam_LI = ot.ParametricFunction(beam, indices, referencePoint)

# %%
# Evaluate d on (E,F) with fixed parameters (L,I)
beam_LI([50.0, 1.0])
