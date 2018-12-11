import openturns as ot
from openturns.viewer import View

f1 = ot.SymbolicFunction(['x'], ['sin(x)'])
f2 = ot.SymbolicFunction(['x'], ['x'])

f = ot.ProductFunction(f1.getImplementation(), f2.getImplementation())
graph = f.draw(0.0, 10.0)
graph.setTitle('y=f1*f2')
View(graph, figure_kwargs={'figsize': (8, 4)}, add_legend=True)
