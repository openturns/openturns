"""
Export a field to VTK
=====================
"""

# %%
# The objective here is to create a field and export it as a VTK file.
# A field is the agregation of a mesh :math:`\mathcal{M}` of a domain :math:`\mathcal{D} \in \mathbb{R}^n`
# and a sample of values in :math:`\mathbb{R}^d` associated to each vertex of the mesh.
#
# We note :math:`(\vect{t}_0, \dots, \vect{t}_{N-1})` the vertices of :math:`\mathcal{M}`
# and :math:`(\vect{x}_0, \dots, \vect{x}_{N-1})` the associated values in :math:`\mathbb{R}^d`.
#
# A field is stored in the :class:`~openturns.Field` object that stores the mesh and the values at each vertex of the mesh.
# It can be built from a mesh and values or as a realization of a stochastic process.

# %%
import openturns as ot
import openturns.viewer as otv
from matplotlib import pyplot as plt


# %%
# First, we define a regular 2-d mesh
discretization = [10, 5]
mesher = ot.IntervalMesher(discretization)
lowerBound = [0.0, 0.0]
upperBound = [2.0, 1.0]
interval = ot.Interval(lowerBound, upperBound)
mesh = mesher.build(interval)
graph = mesh.draw()
graph.setTitle("Regular 2-d mesh")
view = otv.View(graph)

# %%
# We now create a field from a mesh and some values
values = ot.Normal([0.0] * 2, [1.0] * 2, ot.CorrelationMatrix(2)).getSample(
    len(mesh.getVertices())
)
for i in range(len(values)):
    x = values[i]
    values[i] = 0.05 * x / x.norm()
field = ot.Field(mesh, values)

# %%
# We can export the `field` to a VTK files. It can be
# read later with an external program such as Paraview.
field.exportToVTKFile("field.vtk")

# %%
# Display figures
plt.show()
