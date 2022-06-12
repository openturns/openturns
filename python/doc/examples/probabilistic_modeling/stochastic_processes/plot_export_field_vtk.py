"""
Export a field to VTK
=====================
"""
# %%
# The objective here is to create a field and export it as a VTK file. A field is the agregation of a mesh :math:`\mathcal{M}` of a domain :math:`\mathcal{D} \in \mathbb{R}^n` and a sample of values in :math:`\mathbb{R}^d` associated to each vertex of the mesh.
#
# We note :math:`(\underline{t}_0, \dots, \underline{t}_{N-1})` the vertices of :math:`\mathcal{M}` and :math:`(\underline{x}_0, \dots, \underline{x}_{N-1})` the associated values in :math:`\mathbb{R}^d`.
#
# A field is stored in the *Field* object that stores the mesh and the values at each vertex of the mesh.
# It can be built from a mesh and values or as a realization of a stochastic process.

# %%
import openturns as ot
import openturns.viewer as viewer
from matplotlib import pylab as plt
import math as m
ot.Log.Show(ot.Log.NONE)

# %%
# First, we define a regular 2-d mesh
discretization = [10, 5]
mesher = ot.IntervalMesher(discretization)
lowerBound = [0.0, 0.0]
upperBound = [2.0, 1.0]
interval = ot.Interval(lowerBound, upperBound)
mesh = mesher.build(interval)
graph = mesh.draw()
graph.setTitle('Regular 2-d mesh')
view = viewer.View(graph)

# %%
# We now create a field from a mesh and some values
values = ot.Normal([0.0]*2, [1.0]*2, ot.CorrelationMatrix(2)
                   ).getSample(len(mesh.getVertices()))
for i in range(len(values)):
    x = values[i]
    values[i] = 0.05 * x / x.norm()
field = ot.Field(mesh, values)

# %%
# We can export the `field` to a VTK files. It can be
# read later with an external program such as Paraview.
field.exportToVTKFile('field.vtk')

# %%
# Display figures
plt.show()
