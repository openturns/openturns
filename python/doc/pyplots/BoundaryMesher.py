import openturns as ot
from openturns.viewer import View
import openturns.experimental as otexp
import matplotlib.pyplot as plt

# Define the vertices of the mesh
vertices = [[0.0, 0.0], [1.0, 0.0], [1.0, 1.0], [1.5, 1.0], [3.0, 1.5], [0.5, 1.5]]
# Define the simplices of the mesh
simplices = [[0, 1, 2], [1, 2, 3], [2, 3, 4], [2, 4, 5], [0, 2, 5]]
# Create the Mesh
mesh2D = ot.Mesh(vertices, simplices)
# Build the Mesh boundary
mesh2DBoundary = otexp.BoundaryMesher().build(mesh2D)
# Build a thick outside Mesh boundary
mesh2DBoundaryOutside = otexp.BoundaryMesher().build(mesh2D, 0.05)
# Build a thick inside Mesh boundary
mesh2DBoundaryInside = otexp.BoundaryMesher().build(mesh2D, -0.05)
# Create a Graph
graph = ot.Graph("", "", "", True, "bottomright")
graph.add(mesh2D.draw())
# Then, draw it
fig = plt.figure(figsize=(16, 4))
axis = fig.add_subplot(141)
View(graph, figure=fig, axes=[axis], add_legend=True)
axis.set_xlim(auto=True)

# Create a Graph
graph = ot.Graph("", "", "", True, "bottomright")
graph.add(mesh2DBoundary.draw())
# Then, draw it
axis = fig.add_subplot(142)
View(graph, figure=fig, axes=[axis], add_legend=True)
axis.set_xlim(auto=True)

# Create a Graph
graph = ot.Graph("", "", "", True, "bottomright")
graph.add(mesh2DBoundaryInside.draw())
# Then, draw it
axis = fig.add_subplot(143)
View(graph, figure=fig, axes=[axis], add_legend=True)
axis.set_xlim(auto=True)

# Create a Graph
graph = ot.Graph(
    "A 2D mesh with its boundary, its inside thick boundary and its outside thick boundary",
    "",
    "",
    True,
    "bottomright",
)
graph.add(mesh2DBoundaryOutside.draw())
# Then, draw it
axis = fig.add_subplot(144)
View(graph, figure=fig, axes=[axis], add_legend=True)
axis.set_xlim(auto=True)
