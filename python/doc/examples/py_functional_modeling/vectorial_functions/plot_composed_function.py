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
Create a composed function
==========================
"""
# %% 

# %%
# In this example we are going to create a composed function :math:``f\circ g:math:``
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# assume f, g functions
g = ot.SymbolicFunction(['x1', 'x2'],
                        ['x1 + x2','3 * x1 * x2'])
f = ot.SymbolicFunction(['x1', 'x2'], ['2 * x1 - x2'])

# %%
# create the composed function
function = ot.ComposedFunction(f, g)

# %%
# evaluate the function
x = [3.0, 4.0]
y = function(x)
print('x=', x, 'y=', y)
