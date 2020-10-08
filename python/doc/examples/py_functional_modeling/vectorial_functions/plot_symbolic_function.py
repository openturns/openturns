"""
Create a symbolic function
==========================
"""
# %%
# In this example we are going to create a function from mathematical formulas:
#
# .. math:: 
#    f(x_1, x_2) = -(6 + x_0^2 - x_1)
#   
# Analytical expressions of the gradient and hessian are automatically computed except if the function is not differentiable everywhere. In that case a finite difference method is used.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# create a symbolic function
function = ot.SymbolicFunction(['x0', 'x1'],
                               ['-(6 + x0^2 - x1)'])
print(function)

# %%
# evaluate function
x = [2.0, 3.0]
print('x=', x, 'f(x)=', function(x))

# %%
# show gradient
print(function.getGradient())

# %%
# use gradient
print('x=', x, 'df(x)=', function.gradient(x))

# %%
# draw isocontours of f around [2,3]
graph = function.draw(0, 1, 0, [2.0, 3.0], [1.5, 2.5], [2.5, 3.5])
view = viewer.View(graph)
plt.show()
