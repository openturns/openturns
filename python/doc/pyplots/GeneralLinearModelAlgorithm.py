import openturns as ot
import openturns.viewer as otv

f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
inputSample = ot.Sample([[1.0], [3.0], [5.0], [6.0], [7.0], [8.0]])
outputSample = f(inputSample)

f1 = ot.SymbolicFunction(["x"], ["sin(x)"])
f2 = ot.SymbolicFunction(["x"], ["x"])
f3 = ot.SymbolicFunction(["x"], ["cos(x)"])
basis = ot.Basis([f1, f2, f3])
covarianceModel = ot.SquaredExponential([1.0])
covarianceModel.setActiveParameter([])
algo = ot.GeneralLinearModelAlgorithm(inputSample, outputSample, covarianceModel, basis)
algo.run()
result = algo.getResult()
metamodel = result.getMetaModel()

graph = f.draw(0.0, 10.0)
graph.add(metamodel.draw(0.0, 10.0))
graph.add(ot.Cloud(inputSample, outputSample))
graph.setLegends(["model", "meta model", "sample"])
graph.setLegendPosition("upper left")
graph.setTitle(r"$x + x \; \sin(x)$")
otv.View(graph)
