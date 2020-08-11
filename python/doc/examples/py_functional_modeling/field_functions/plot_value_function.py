"""
Value function
==============
"""
# %%
# A value function
# :math:`f_{value}: \mathcal{D} \times \mathbb{R}^d \rightarrow \mathcal{D} \times \mathbb{R}^q` is a
# particular field function that lets invariant the mesh of a field
# and defined by a function :math:`g : \mathbb{R}^d  \rightarrow \mathbb{R}^q` such that:
#
# .. math::
#    \begin{aligned} f_{value}(\underline{t}, \underline{x})=(\underline{t}, g(\underline{x}))\end{aligned}
#   
# Let’s note that the input dimension of :math:`f_{value}` still designs the
# dimension of :math:`\underline{x}` : :math:`d`. Its output dimension is equal to :math:`q`.
#
# The creation of the *ValueFunction* object requires the
# function :math:`g` and the integer :math:`n`: the dimension of the
# vertices of the mesh :math:`\mathcal{M}`. This data is required for tests on the
# compatibility of dimension when a composite process is created using the
# spatial function.
#
#
# The use case illustrates the creation of a spatial (field) function
# from the function :math:`g: \mathbb{R}^2  \rightarrow \mathbb{R}^2` such as :
# 
# .. math::
#    \begin{aligned}
#      g(\underline{x})=(x_1^2, x_1+x_2)
#    \end{aligned}
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m

# %%
# Create a mesh
N = 100
mesh = ot.RegularGrid(0.0, 1.0, N)

# %%
# Create the function that acts the values of the mesh
g = ot.SymbolicFunction(['x1', 'x2'],  ['x1^2', 'x1+x2'])

# %%
# Create the field function
f = ot.ValueFunction(g, mesh)

# %%
# Evaluate f
inF = ot.Normal(2).getSample(N)
outF = f(inF)

# print input/output at first mesh nodes
xy = inF
xy.stack(outF)
xy[:5]
