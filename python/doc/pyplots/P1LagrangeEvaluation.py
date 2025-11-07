import openturns as ot
import openturns.viewer as otv

lowerBound = [0.0, 0.0]
upperBound = [1.0, 1.0]
interv = ot.Interval(lowerBound, upperBound)
mesher = ot.IntervalMesher([6] * 2)
mesh = mesher.build(interv, True)

f = ot.SymbolicFunction(["x", "y"], ["sin(pi_*x)*sin(pi_*y)"])
values = f(mesh.getVertices())

field = ot.Field(mesh, values)
func_P1 = ot.P1LagrangeEvaluation(field)

graph = mesh.draw()
graph.setColors(["lightblue"])
draw_f = f.draw(lowerBound, upperBound, [1024] * 2).getDrawable(0)
draw_f.setLineStyle("dashed")
lev = draw_f.getLevels()
draw_fP1 = (
    func_P1.draw(lowerBound, upperBound, [1024] * 2).getDrawable(0).getImplementation()
)
draw_fP1.setLevels(lev)
draw_fP1.setColorBarPosition("")
graph.add(draw_f)
graph.add(draw_fP1)
graph.setTitle(
    r"P1 Lagrange interpolation (solid) of $(x,y) \mapsto \sin(\pi x) \sin(\pi y)$ (dashed) from a mesh (blue solid)")
graph.setXTitle("$x$")
graph.setYTitle("$y$")

view = otv.View(graph, figure_kw={"figsize": (10, 4)}, add_legend=False)
