import openturns as ot
from openturns.viewer import View

f = ot.SymbolicFunction(['x'], ['sin(pi_*sin(pi_*x))'])
graph = ot.Graph(r'Function $x  \mapsto \sin(\pi \sin(\pi x))$', r'$x$', r'$y$', True)
graph.add(f.draw(-0.5, 3., 1024))

view = View(graph, figure_kw={"figsize": (10, 4)}, add_legend = False)
