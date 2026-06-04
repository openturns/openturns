"""
Composite random vector
=======================
"""

# %%
# In this example we are going to create a random variable :math:`\vect{Y}` which realizations are the images
# of the realizations of another random vector :math:`\vect{X}` by a function.
#
# .. math::
#     \vect{Y} = f(\vect{X})
#

# %%
import openturns as ot


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
