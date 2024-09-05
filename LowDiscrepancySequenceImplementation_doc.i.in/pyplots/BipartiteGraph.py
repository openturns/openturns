import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

bipartite = ot.BipartiteGraph([[0, 2], [1, 4], [3, 4, 5]])

graph = bipartite.draw()
graph.setTitle("Bipartite graph")

fig = plt.figure(figsize=(6, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)
