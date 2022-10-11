"""
Composite random vector
=======================
"""
# %%
# In this example we are going to create a random variable :math:`\underline{Y}` which realizations are the images of the realizations of another random vector :math:`\underline{X}` by a function.
#
# .. math::
#     \underline{Y} = f(\underline{X})
#

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

ot.Log.Show(ot.Log.NONE)

# %%
# Create a random vector based on a distribution
dist2d = ot.Normal(2)
X = ot.RandomVector(dist2d)

# %%
# Create the function
f = ot.SymbolicFunction(["x1", "x2"], ["x1 + x2", "x1*x2"])
f

# %%
# Create the composite random vector
Y = ot.CompositeRandomVector(f, X)

# %%
Y.getSample(5)
