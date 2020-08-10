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
Estimate an integral
====================
"""
# %% 

# %%
# In this example we are going to evaluate an integral of the form.
#
# .. math::I_f = \int_{a}^{b}\, \int_{l_1(x_0)}^{u_1(x_0)}\, \int_{l_2(x_0, x_1)}^{u_2(x_0,x_1)}\, \int_{l_{n-1}(x_0, \dots, x_{n-2})}^{u_{n-1}(x_0, \dots, x_{n-2})} \, f(x_0, \dots, x_{n-1})\mathrm{d}{x_{n-1}}\dots\mathrm{d}{x_0}:math:``
#   
# with the iterated quadrature algorithm.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# define the integrand and the bounds
a = -m.pi
b = m.pi
f = ot.SymbolicFunction(['x', 'y'], ['1+cos(x)*sin(y)'])
l = [ot.SymbolicFunction(['x'], [' 2+cos(x)'])]
u = [ot.SymbolicFunction(['x'], ['-2-cos(x)'])]

# %%
# Draw the graph of the integrand and the bounds
g = ot.Graph('Integration nodes', 'x', 'y', True, 'topright')
g.add(f.draw([a,a],[b,b]))
curve = l[0].draw(a, b).getDrawable(0)
curve.setLineWidth(2)
curve.setColor('red')
g.add(curve)
curve = u[0].draw(a, b).getDrawable(0)
curve.setLineWidth(2)
curve.setColor('red')
g.add(curve)
g

# %%
# compute the integral value
I2 = ot.IteratedQuadrature().integrate(f, a, b, l, u)
print(I2)
