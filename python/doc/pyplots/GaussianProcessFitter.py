import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

f = ot.SymbolicFunction(["x"], ["x * sin(x)"])
sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0], [7.0], [8.0]]
sampleY = f(sampleX)
basis = ot.Basis([ot.SymbolicFunction(["x"], ["1"])])
covarianceModel = ot.SquaredExponential([1.0])
covarianceModel.setActiveParameter([])
fitter_algo = otexp.GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
fitter_algo.run()
fitter_result = fitter_algo.getResult()

graph = f.draw(0.0, 10.0)
graph.add(fitter_result.getMetaModel().draw(0.0, 10.0))
graph.add(ot.Cloud(sampleX, sampleY))

algo = otexp.GaussianProcessRegression(fitter_result)
algo.run()
result = algo.getResult()
fhat = result.getMetaModel()

graph.add(fhat.draw(0.0, 10.0))
graph.setLegends(["model", "GP Fitter", "sample", "GP Regression"])
graph.setLegendPosition("upper left")
graph.setTitle(r"$y(x)=x * sin(x)$")
graph.setYTitle(r"$f$")
graph.setXTitle(r"$x$")
otv.View(graph, figure_kw={"figsize": (8, 4)})
