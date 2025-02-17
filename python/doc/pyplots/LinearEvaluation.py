import openturns as ot
from openturns.viewer import View

A = ot.Matrix(2, 1)
A[0, 0] = 1.0
A[1, 0] = 2.0
const = [1.5]
center = [2.0, 3.0]
func = ot.LinearEvaluation(center, const, A)

graph = func.draw([0.0, 0.0], [4.0, 4.0], [64] * 2)
graph.setTitle(r'Linear function $(x,y) \mapsto (x-2) + 2(y-3) + 1.5$')
graph.setXTitle(r'$x$')
graph.setYTitle(r'$y$')
View(graph, figure_kw={"figsize": (10, 4)}, add_legend=True)
