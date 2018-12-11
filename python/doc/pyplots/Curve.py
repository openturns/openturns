import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
from math import sin, pi

# Generate the data for the curves to be drawn
data = [
    [-3 * pi + 6 * pi * i / 100, sin(-3 * pi + 6 * pi * i / 100)] for i in range(100)]

# Create an empty graph
graph = ot.Graph("Curve example", "x", "sin(x)", True, "")

# Create the curve
curve = ot.Curve(data, "blue", "solid", 2, "")

# Then, draw it
graph.add(curve)

fig = plt.figure(figsize=(6, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=False)
