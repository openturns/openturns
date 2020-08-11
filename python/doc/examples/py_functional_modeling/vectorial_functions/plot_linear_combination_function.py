"""
Create a linear combination of functions
========================================
"""
# %%
# In this example we are going to build a linear combination of several functions.
#
# .. math::
#    f = \sum \alpha_i f_i
#   
# Here
#
# .. math::
#    f_1(x) = [x_1^2+x_2,x_1+x_2+x_3]
#    f_2(x) = [x_1 + 2 * x_2 + x_3,x_1+x_2+x_3]
#   
# with coefficients
#
# .. math::
#    a_1 = 2, a_2 = 4
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# assume a list of functions to combine
functions = list()
functions.append(ot.SymbolicFunction(['x1', 'x2', 'x3'],
                                     ['x1^2 + x2', 'x1 + x2 + x3']))
functions.append(ot.SymbolicFunction(['x1', 'x2', 'x3'],
                                     ['x1 + 2 * x2 + x3', 'x1 + x2 - x3']))

# %%
# create the combination function
coefficients = [2.0, 4.0]
function = ot.LinearCombinationFunction(functions, coefficients)

# %%
# evaluate the function
x = [1.0, 2.0, 3.0]
y = function(x)
print('x=', x, 'y=', y)
