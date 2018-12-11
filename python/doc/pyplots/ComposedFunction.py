import openturns as ot
from openturns.viewer import View

g = ot.SymbolicFunction(['x'], ['sin(x)'])
f = ot.SymbolicFunction(['y'], ['abs(y)'])
composed = ot.ComposedFunction(f, g)

graph = composed.draw(0.0, 10.0)
graph.setTitle('y=abs(sin(x))')
View(graph, figure_kwargs={'figsize': (8, 4)}, add_legend=True).ShowAll()
