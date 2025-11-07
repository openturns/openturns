import openturns as ot
import openturns.viewer as otv

center = [0.0]
constant = [3.0]
linear = ot.Matrix([[2.0]])
f = ot.LinearFunction(center, constant, linear)

graph = f.draw(0.0, 10.0)
graph.setTitle(r"Linear function $x \mapsto 2x+3$")
graph.setXTitle(r"$x$")
graph.setYTitle(r"$y$")
otv.View(graph, figure_kw={"figsize": (8, 4)}, add_legend=True)
