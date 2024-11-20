import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

f = ot.SymbolicFunction(['x'], ['sin(pi_*sin(pi_*x))'])
graph = ot.Graph(r'Function $x  \mapsto \sin(\pi \sin(\pi x))$', r'$x$', r'$y$', True) 
graph.add(f.draw(-0.5, 3., 1024))

fig = plt.figure(figsize=(10, 4))
func_axis = fig.add_subplot(111)
view = View(graph, figure=fig, axes=[func_axis], add_legend=False)
