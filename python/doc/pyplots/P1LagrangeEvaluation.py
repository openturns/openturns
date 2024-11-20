import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View

lowerBound = [0.0, 0.0]
upperBound = [1.0, 1.0]
interv = ot.Interval(lowerBound, upperBound)
mesher = ot.IntervalMesher([6]*2)
mesh = mesher.build(interv, True)

f = ot.SymbolicFunction(['x', 'y'], ['sin(pi_*x)*sin(pi_*y)'])
values = f(mesh.getVertices())

field = ot.Field(mesh, values)
func_P1 = ot.P1LagrangeEvaluation(field)

graph = mesh.draw()
graph.setColors(['lightblue'])
draw_f = f.draw(lowerBound, upperBound, [1024]*2).getDrawable(0)
draw_f.setLineStyle('dashed')
lev = draw_f.getLevels()
draw_fP1 = func_P1.draw(lowerBound, upperBound, [1024]*2).getDrawable(0)
draw_fP1.setLevels(lev)
graph.add(draw_f)
graph.add(draw_fP1)
graph.setTitle(r"P1 Lagrange interpolation of $(x,y) \mapsto \sin(\pi x) \sin(\pi y)$")
graph.setXTitle('$x$')
graph.setYTitle('$y$')

fig = plt.figure(figsize=(10, 4))
func_axis = fig.add_subplot(111)
View(graph, figure=fig, axes=[func_axis], add_legend=False)
