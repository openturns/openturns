import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View

mesh = ot.RegularGrid(0.0, 1.0, 4)
values = [[0.5], [1.5], [1.0], [-0.5]]
field = ot.Field(mesh, values)
func = ot.P1LagrangeEvaluation(field)
func.setDescription(['$x$', '$y$'])

graph = func.draw(-1.0, 4.0, 1024)
cloud = ot.Cloud(mesh.getVertices(), values)
cloud.setPointStyle("square")
graph.add(cloud)
graph.setColors(["blue", "red"])
graph.setTitle('P1 Lagrange interpolation')

fig = plt.figure(figsize=(10, 4))
func_axis = fig.add_subplot(111)
view = View(graph, figure=fig, axes=[func_axis], add_legend=False)
