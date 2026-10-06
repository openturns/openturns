import openturns as ot
import openturns.viewer as otv
from openturns.experimental import SparseGaussianProcessFitter

f = ot.SymbolicFunction(["x"], ["x * sin(x)"])
sampleX = [[1.0], [2.0], [3.0], [4.5], [6.0], [7.0], [8.0]]
sampleY = f(sampleX)
covarianceModel = ot.SquaredExponential([1.0])
covarianceModel.setActiveParameter([])
n_inducing = 4
algo = SparseGaussianProcessFitter(sampleX, sampleY, covarianceModel, n_inducing)
algo.run()
result = algo.getResult()

metaModel = result.getMetaModel()
graph = metaModel.draw(0.5, 8.5)
graph.add(ot.Cloud(sampleX, sampleY))
inducingPoints = result.getInducingPoints()
cloud = ot.Cloud(inducingPoints, metaModel(inducingPoints))
cloud.setColor("orange")
cloud.setPointStyle("square")
cloud.setLegend("inducing points")
graph.add(cloud)
graph.setLegends(["metamodel", "sample", "inducing points"])
graph.setLegendPosition("upper left")
otv.View(graph, figure_kw={"figsize": (8, 4)})
