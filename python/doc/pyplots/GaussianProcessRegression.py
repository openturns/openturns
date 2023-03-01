import openturns as ot
from openturns.experimental import GaussianProcessFitter, GaussianProcessRegression
from openturns.viewer import View

f = ot.SymbolicFunction(["x"], ["x * sin(x)"])
sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0], [7.0], [8.0]]
sampleY = f(sampleX)
basis = ot.Basis(
    [ot.SymbolicFunction(["x"], ["x"]), ot.SymbolicFunction(["x"], ["x^2"])]
)
covarianceModel = ot.SquaredExponential([1.0])
covarianceModel.setActiveParameter([])
fitter_algo = GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
fitter_algo.run()
algo = GaussianProcessRegression(fitter_algo.getResult())
algo.run()
result = algo.getResult()

graph = f.draw(0.0, 10.0)
graph.add(result.getMetaModel().draw(0.0, 10.0))
graph.add(ot.Cloud(sampleX, sampleY))
graph.setColors(["blue", "red", "black"])
graph.setLegends(["model", "meta model", "sample"])
graph.setLegendPosition("upper left")
graph.setTitle("y(x)=x * sin(x)")
View(graph, figure_kw={"figsize": (8, 4)})
