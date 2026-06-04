import openturns as ot
from matplotlib import pyplot as plt
import openturns.viewer as otv

# Create the Pie
pie = ot.Pie([0.4, 0.3, 0.2, 0.1], ["a0", "a1", "a2", "a3"])
# Create an empty graph
graph = ot.Graph("Pie example")
graph.add(pie)

# Then, draw it
fig = plt.figure(figsize=(4, 4))
axis = fig.add_subplot(111)
otv.View(graph, figure=fig, axes=[axis], add_legend=False)
axis.set_xlim(auto=True)
