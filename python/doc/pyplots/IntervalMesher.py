import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create the mesher
mesher = ot.IntervalMesher([10] * 2)

# Create a square
square = ot.Interval([0.0] * 2, [1.0] * 2)

# Mesh the square the usual way
mesh1 = mesher.build(square)

# Mesh the square using diamond mesh
mesh2 = mesher.build(square, True)

# Draw the first mesh
graph1 = mesh1.draw()
graph1.setTitle('diamond=False')
graph1.setXTitle('$x_0$')
graph1.setYTitle('$x_1$')

# Draw the second mesh
graph2 = mesh2.draw()
graph2.setTitle('diamond=True')
graph2.setXTitle('$x_0$')
graph2.setYTitle('$x_1$')

fig = plt.figure(figsize=(10, 4))
graph1_axis = fig.add_subplot(121)
graph2_axis = fig.add_subplot(122)
graph1_axis.set_xlim(auto=True)
graph2_axis.set_xlim(auto=True)

View(graph1, figure=fig, axes=[graph1_axis], add_legend=True)
View(graph2, figure=fig, axes=[graph2_axis], add_legend=True)
fig.suptitle("Mesh of an interval: diamond=False/True")
