import openturns as ot
from openturns.viewer import View

f1 = ot.SymbolicFunction(['x'], ['sin(x)'])
f2 = ot.SymbolicFunction(['x'], ['x'])
f3 = ot.SymbolicFunction(['x'], ['cos(x)'])

f = ot.LinearCombinationFunction([f1, f2, f3], [1.0, 2.0, 3.0])
graph = f.draw(0.0, 10.0)
graph.setTitle('y=1*f1+2*f2+3*f3')
View(graph, figure_kwargs={'figsize': (8, 4)}, add_legend=True)
