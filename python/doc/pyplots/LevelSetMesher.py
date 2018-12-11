import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create the mesher
mesher = ot.LevelSetMesher([50] * 2)

# Create a level set
function = ot.SymbolicFunction(['x0', 'x1'], ['10*(x0^3+x1)^2+x0^2'])
level = 0.5
set = ot.LevelSet(function, level)

# Mesh the level set
mesh = mesher.build(set, ot.Interval([-1.0] * 2, [1.0] * 2))

# Draw the first mesh
graph = mesh.draw()
graph.setXTitle('$x_0$')
graph.setYTitle('$x_1$')
graph.setTitle('Mesh of a level set')

fig = plt.figure(figsize=(10, 4))
graph_axis = fig.add_subplot(111)
graph_axis.set_xlim(auto=True)

View(graph, figure=fig, axes=[graph_axis], add_legend=True)
