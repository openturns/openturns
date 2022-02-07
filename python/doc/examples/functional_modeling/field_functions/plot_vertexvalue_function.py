"""
Vertex value function
=====================
"""
# %%
#
# A vertex value function
# :math:`f_{vertexvalue}: \mathcal{D} \times \mathbb{R}^d \rightarrow \mathcal{D} \times \mathbb{R}^q` is a
# particular field function that lets invariant the mesh of a field
# and defined by a function :math:`h : \mathbb{R}^n \times \mathbb{R}^d  \rightarrow \mathbb{R}^q` such that:
#
# .. math::
#    \begin{aligned} f_{vertexvalue}(\underline{t}, \underline{x})=(\underline{t}, h(\underline{t},\underline{x}))\end{aligned}
#
# Let's note that the input dimension of :math:`f_{vertexvalue}` still design the
# dimension of :math:`\underline{x}` : :math:`d`. Its output dimension is equal to :math:`q`.
#
# The creation of the *VertexValueFunction* object requires the
# function :math:`h` and the integer :math:`n`: the dimension of the
# vertices of the mesh :math:`\mathcal{M}`.
#
# This example illustrates the creation of a field from the function
# :math:`h:\mathbb{R}\times\mathbb{R}^2` such as:
#
# .. math::
#    \begin{aligned}
#       h(\underline{t}, \underline{x})=(t+x_1^2+x_2^2)
#    \end{aligned}
#

# %%
from __future__ import print_function
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# Create a mesh
N = 100
mesh = ot.RegularGrid(0.0, 1.0, N)

# %%
# Create the function that acts the values of the mesh
h = ot.SymbolicFunction(['t', 'x1', 'x2'],  ['t+x1^2+x2^2'])

# %%
# Create the field function
f = ot.VertexValueFunction(h, mesh)

# %%
# Evaluate f
inF = ot.Normal(2).getSample(N)
outF = f(inF)

# print input/output at first 10 mesh nodes
txy = mesh.getVertices()
txy.stack(inF)
txy.stack(outF)
txy[:10]
