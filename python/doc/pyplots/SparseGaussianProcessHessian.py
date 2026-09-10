import openturns as ot
import openturns.viewer as otv
from openturns.experimental import SparseGaussianProcessRegression

f = ot.SymbolicFunction(["x"], ["x * sin(x)"])
sampleX = [[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]]
sampleY = f(sampleX)
covarianceModel = ot.SquaredExponential([1.0])
covarianceModel.setActiveParameter([])
inducingPoints = ot.Sample([[1.0], [2.5], [4.0], [5.5], [7.0], [8.5]])
algo = SparseGaussianProcessRegression(sampleX, sampleY, covarianceModel, inducingPoints)
algo.run()
result = algo.getResult()
metaModel = result.getMetaModel()

# Draw the second order derivative of the metamodel mean
n_points = 101
xMin, xMax = 0.5, 8.5
step = (xMax - xMin) / (n_points - 1)
grid = ot.RegularGrid(xMin, step, n_points)
vertices = grid.getVertices()
hessianValues = ot.Sample(n_points, 1)
for i in range(n_points):
    hessianValues[i, 0] = metaModel.hessian(vertices[i])[0, 0, 0]

modelValues = ot.Sample(len(sampleX), 1)
for i in range(len(sampleX)):
    modelValues[i, 0] = f.hessian(sampleX[i])[0, 0, 0]

graph = ot.Graph("Sparse GP metamodel hessian", "x", r"$\partial^2 \mu / \partial x^2$")
graph.add(ot.Curve(vertices, hessianValues))
graph.add(ot.Cloud(sampleX, modelValues))
graph.setLegends(["sparse GP metamodel hessian", "model"])
graph.setLegendPosition("upper right")
otv.View(graph, figure_kw={"figsize": (8, 4)})
