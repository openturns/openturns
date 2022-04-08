"""
Create a Python function
========================
"""
# %%
# In this example we are going to create a Function object (ie usable in the OpenTURNS context) from a pure Python function.
#
# The pure Python function to wrap must accept a sequence of floats and return a sequence of float.
#
# .. math::
#    f(x) = [x_0+x_1+x_2, (x_1-1)*e^{x_0} * x_2]
#

# %%
import numpy as np
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m
ot.Log.Show(ot.Log.NONE)


# %%
# define a pure Python function from R^3 to R^2
def regularFunc(X):
    x0, x1, x2 = X
    y0 = x0 + x1 + x2
    y1 = (x1 - 1.0) * m.exp(x0) * x2
    return [y0, y1]


# %%
# create a Function object from a regular Python function
function = ot.PythonFunction(3, 2, regularFunc)

# %%
# evaluate the function on a Point
x = [1.0, 2.0, 3.0]
print('x=', x, 'f(x)=', function(x))

# %%
# evaluate the function on a Sample
xs = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]
print('xs=', xs, '\nf(xs)=', function(xs))

# %%
# now we can use the Function object services such as the gradient
function.gradient(x)

# %%
# Performance issues
# ------------------
#
# When this function is used internally to evaluate a Sample, it loops over its points.  This requires many memory allocations; moreover this loop is done in Python, it may thus be slow if Sample is large.
# We can define a function to operate on a Sample, and return a Sample.
#
# For maximum performance, argument is in fact not a Sample, but a wrapper object which contains a pointer to data.  When using Numpy arrays without copies and loops, performance is similar to C code, but Python definition is somewhat convoluted; please refer to Numpy documentation to learn how to efficiently define such functions.

# %%
# define the same function on a Sample


def regularFuncSample(X):
    # Create a numpy array with the contents of X without copy
    xarray = np.array(X, copy=False)
    # Get columns as vectors, there is also no copy
    x0, x1, x2 = xarray.T
    # Allocate a numpy array to store result
    y = np.zeros((len(X), 2))
    y[:, 0] = x0 + x1 + x2
    y[:, 1] = (x1 - 1.0) * np.exp(x0) * x2
    return y


# %%
# create a Function object from a regular Python function
functionSample = ot.PythonFunction(3, 2, func_sample=regularFuncSample)

# %%
# evaluate the function on a Sample
print('xs=', xs, '\nf(xs)=', functionSample(xs))

# %%
# evaluate the function on a Point
print('x=', x, 'f(x)=', functionSample(x))

# %%
# The most efficient solution is to provide evaluations both on Point and Sample.  This requires two Python function definitions, but if your code takes a lot of time, you should consider this option.

# %%
functionFast = ot.PythonFunction(
    3, 2, func=regularFunc, func_sample=regularFuncSample)
