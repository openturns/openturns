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
Create an aggregated function
=============================
"""
# %% 

# %%
# In this example we are going to build a function that stacks all the outputs from several functions
#
# .. math::f = (f_1, \dots, f_n):math:``
#   
#
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# assume a list of functions to aggregate
functions = list()
functions.append(ot.SymbolicFunction(['x1', 'x2', 'x3'],
                                     ['x1^2 + x2', 'x1 + x2 + x3']))
functions.append(ot.SymbolicFunction(['x1', 'x2', 'x3'],
                                     ['x1 + 2 * x2 + x3', 'x1 + x2 - x3']))

# %%
# create the aggregated function
function = ot.AggregatedFunction(functions)

# %%
# evaluate the function
x = [1.0, 2.0, 3.0]
y = function(x)
print('x=', x, 'y=', y)
