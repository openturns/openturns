import openturns as ot
from openturns.viewer import View

center = [0.0]
constant = [3.0]
linear = ot.Matrix([[2.0]])
f = ot.LinearFunction(center, constant, linear)

graph = f.draw(0.0, 10.0)
graph.setTitle('$y=2x+3$')
View(graph, figure_kwargs={'figsize': (8, 4)}, add_legend=True)
