import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create the Pie
pie = ot.Pie([.4, .3, .2, .1], ['a0', 'a1', 'a2', 'a3'])
# Create an empty graph
graph = ot.Graph("Pie example")
graph.add(pie)

# Then, draw it
fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
View(graph, figure=fig, axes=[axis], add_legend=False)
axis.set_xlim(auto=True)
