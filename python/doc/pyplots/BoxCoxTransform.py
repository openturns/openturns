import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

# Create a Box Cox transformation
lambdas = [0.0, 0.1, 1.0, 1.5]

graph = ot.Graph("Box Cox transformations", 'x', 'y', True)
for i in range(len(lambdas)):
    boxCoxT = ot.BoxCoxTransform(lambdas[i])
    graph.add(boxCoxT.draw(0.1, 2.1))

graph.setColors(['red', 'blue', 'black', 'green'])
graph.setLegends(['lambda = ' + str(lam) for lam in lambdas])
graph.setLegendPosition("bottomright")

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(graph, figure=fig, axes=[axis], add_legend=True)
