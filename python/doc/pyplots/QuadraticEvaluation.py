import openturns as ot
from openturns.viewer import View

center = [0.0]
constant = [3.0]
linear = ot.Matrix([[2.0]])
quadratic = ot.SymmetricTensor([[[5.0]]])
f = ot.QuadraticEvaluation(center, constant, linear, quadratic)

graph = f.draw(-5.0, 5.0)
graph.setTitle(r"Quadratic function $x \mapsto 5x^2+2x+3$")
graph.setXTitle(r'$x$')
graph.setYTitle(r'$y$')
View(graph, figure_kw={"figsize": (8, 4)}, add_legend=True)
