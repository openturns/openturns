import openturns as ot
import openturns.viewer as otv

g = ot.SymbolicFunction(["x"], ["sin(x)"])
f = ot.SymbolicFunction(["y"], ["abs(y)"])
composed = ot.ComposedFunction(f, g)

graph = composed.draw(0.0, 10.0)
graph.setTitle("y=abs(sin(x))")
otv.View(graph, figure_kw={"figsize": (8, 4)}, add_legend=True).ShowAll()
