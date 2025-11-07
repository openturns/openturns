import openturns as ot
import openturns.viewer as otv

lowerBound = [0.0, 0.0]
upperBound = [1.0, 1.0]
interv = ot.Interval(lowerBound, upperBound)
mesher = ot.IntervalMesher([4] * 2)
inputMesh = mesher.build(interv, True)

lowerBound = [0.0, 0.0]
upperBound = [1.0, 1.0]
interv = ot.Interval(lowerBound, upperBound)
mesher = ot.IntervalMesher([8] * 2)
tempMesh = mesher.build(interv, True)
transform = ot.SymbolicFunction(["x", "y"], ["x^2", "y^2"])
outputMesh = ot.Mesh(transform(tempMesh.getVertices()), tempMesh.getSimplices())

f = ot.SymbolicFunction(["x", "y"], ["sin(pi_*x)*sin(pi_*y)"])
values_inputMesh = f(inputMesh.getVertices())
input_field = ot.Field(inputMesh, values_inputMesh)

dim_field = 1
func_P1 = ot.P1LagrangeInterpolation(inputMesh, tempMesh, dim_field)

values_outputMesh = func_P1(values_inputMesh)
output_field = ot.Field(tempMesh, values_outputMesh)

graph_1 = input_field.drawMarginal(0)
graph_1.setXTitle("$x$")
graph_1.setYTitle("$y$")
graph_1.setTitle(r"Function $x \rightarrow sin(\pi x)sin(\pi y)$ on the input mesh")
graph_1.setLegendPosition('')

graph_2 = output_field.drawMarginal(0)
graph_2.setLegendPosition('')
graph_2.setXTitle("$x$")
graph_2.setYTitle("$y$")
graph_2.setTitle(r"Function $x \rightarrow sin(\pi x)sin(\pi y)$ on the output mesh")

grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, graph_1)
grid.setGraph(0, 1, graph_2)


view = otv.View(grid)
view.show()
