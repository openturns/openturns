import openturns as ot
from openturns.viewer import View

dim = 1
f = ot.SymbolicFunction(['x'], ['x*sin(x)'])
uniform = ot.Uniform(0.0, 10.0)
distribution = ot.ComposedDistribution([uniform]*dim)
factoryCollection = [ot.OrthogonalUniVariateFunctionFamily(ot.OrthogonalUniVariatePolynomialFunctionFactory(ot.StandardDistributionPolynomialFactory(uniform)))] * dim
functionFactory = ot.OrthogonalProductFunctionFactory(factoryCollection)
size = 10
sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0], [7.0], [8.0]]
sampleY = f(sampleX)
nk = [5] * dim
maxRank = 1
algo = ot.TensorApproximationAlgorithm(sampleX, sampleY, distribution, functionFactory, nk, maxRank)
algo.run()
result = algo.getResult()
metamodel = result.getMetaModel()

graph = f.draw(0.0, 10.0)
graph.add(metamodel.draw(0.0, 10.0))
graph.add(ot.Cloud(sampleX, sampleY))
graph.setColors(['blue', 'red', 'black'])
graph.setLegends(['model', 'meta model', 'sample'])
graph.setLegendPosition('topleft')
graph.setTitle('y(x)=x*sin(x)')
graph.setYTitle('y')
View(graph, figure_kwargs={'figsize': (8, 4)})
