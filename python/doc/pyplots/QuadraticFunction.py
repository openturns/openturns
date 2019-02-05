import openturns as ot
from openturns.viewer import View

center = [0.0]
constant = [3.0]
linear = ot.Matrix([[2.0]])
quadratic = ot.SymmetricTensor([[[5.0]]])
f = ot.QuadraticFunction(center, constant, linear, quadratic)

graph = f.draw(0.0, 10.0)
graph.setTitle('$y=5x^2+2x+3$')
View(graph, figure_kwargs={'figsize': (8, 4)}, add_legend=True)
