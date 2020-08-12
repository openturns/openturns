"""
Create a composite distribution
===============================
"""
# %%
# In this example we are going to create a distribution defined as the push-forward distribution of a scalar distribution by a transformation.
#
#
# If we note :math:`\mathcal{L}_0` a scalar distribution, :math:`f: \mathbb{R} \rightarrow \mathbb{R}` a mapping, then it is possible to create the push-forward distribution :math:`\mathcal{L}` defined by
#
# .. math::
#    \mathcal{L} = f(\mathcal{L}_0)
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt

# %%
# create an 1-d distribution
antecedent = ot.Normal()

# %%
# Create an 1-d transformation
f = ot.SymbolicFunction(['x'], ['sin(x)+cos(x)'])

# %%
# Create the composite distribution
distribution = ot.CompositeDistribution(f, antecedent)
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# Using the simplified construction
distribution = antecedent.exp()
graph = distribution.drawPDF()
view = viewer.View(graph)

# %%
# Using chained operators
distribution = antecedent.abs().sqrt()
graph = distribution.drawPDF()
view = viewer.View(graph)
plt.show()
