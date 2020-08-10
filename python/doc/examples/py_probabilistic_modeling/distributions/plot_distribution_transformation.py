"""
Transform a distribution
========================
"""
# %% 

# %%
# In this example we are going to use distribution algebra and distribution transformation via functions.

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# We define some (classical) distribution :

# %%
distribution1 = ot.Uniform(0.0, 1.0)
distribution2 = ot.Uniform(0.0, 2.0)
distribution3 = ot.WeibullMin(1.5, 2.0)

# %%
# Sum & difference of distributions
# ---------------------------------
#
# It is easy to compute the sum of distributions. For example:

# %%
distribution = distribution1 + distribution2
print(distribution)
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# We might also use substraction even with scalar values: 

# %%
distribution = 3.0 - distribution3
print(distribution)
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# Product & inverse
# -----------------
#
# We might also compute the product of two (or more) distributions. For example:

# %%
distribution = distribution1 * distribution2
print(distribution)
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# We could also inverse a distribution : 

# %%
distribution = 1 / distribution1
print(distribution)
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# Or compute a ratio distrobution:

# %%
ratio = distribution2 / distribution1
print(ratio)
graph = ratio.drawPDF()
view = viewer.View(graph)

# %%
# Transformation using functions
# ------------------------------
#
# The library provides methods to get the full distributions of `f(x)` where `f` can be equal to :
#
#  - `sin`, 
#  - `asin`,
#  - `cos`, 
#  - `acos`, 
#  - `tan`,
#  - `atan`,
#  - `sinh`, 
#  - `asinh`,
#  - `cosh`, 
#  - `acosh`, 
#  - `tanh`,
#  - `atanh`,
#  - `sqr` (for square), 
#  - `inverse`, 
#  - `sqrt`,
#  - `exp`,
#  - `log`/`ln`,
#  - `abs`,
#  - `cbrt`.
#  
# If one wants a specific method, user might rely on `CompositeDistribution`. 
#
# For example for the usual `log` transformation:

# %%
graph =distribution1.log().drawPDF()
view = viewer.View(graph)

# %%
# And for the `log2` function:

# %%
f = ot.SymbolicFunction(['x'], ['log2(x)'])
f.setDescription(["X","ln(X)"])
graph = ot.CompositeDistribution(f, distribution1).drawPDF()
view = viewer.View(graph)
plt.show()
