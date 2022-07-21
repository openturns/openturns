import openturns as ot
from openturns.viewer import View

f1 = ot.SymbolicFunction(['x'], ['sin(x)'])
f2 = ot.SymbolicFunction(['x'], ['x'])

f = f1 * f2
graph = f.draw(0.0, 10.0)
graph.setTitle('y=f1*f2')
View(graph, figure_kw={'figsize': (8, 4)}, add_legend=True)
