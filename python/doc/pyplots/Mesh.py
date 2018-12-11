import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Define the vertices of the mesh
vertices = [[0.0, 0.0], [1.0, 0.0], [1.0, 1.0],
            [1.5, 1.0], [2.0, 1.5], [0.5, 1.5]]
# Define the simplices of the mesh
simplices = [[0, 1, 2], [1, 2, 3], [2, 3, 4], [2, 4, 5], [0, 2, 5]]
# Create the Mesh
mesh2D = ot.Mesh(vertices, simplices)
# Create a Graph
graph = ot.Graph('Mesh 2D', '', '', True, 'bottomright')
graph.add(mesh2D.draw())

# Then, draw it
fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
View(graph, figure=fig, axes=[axis], add_legend=True)
axis.set_xlim(auto=True)
