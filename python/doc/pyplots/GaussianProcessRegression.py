import openturns as ot
import openturns.experimental as otexp
from openturns.viewer import View

f = ot.SymbolicFunction(["x"], ["x * sin(x)"])
sampleX = [[1.0], [2.0], [3.0], [4.0], [5.0], [6.0], [7.0], [8.0]]
sampleY = f(sampleX)
basis = ot.Basis(
    [ot.SymbolicFunction(["x"], ["1"])]
)
covarianceModel = ot.SquaredExponential([1.0])
covarianceModel.setActiveParameter([])
fitter_algo = otexp.GaussianProcessFitter(sampleX, sampleY, covarianceModel, basis)
fitter_algo.run()
algo = otexp.GaussianProcessRegression(fitter_algo.getResult())
algo.run()
result = algo.getResult()
fhat = result.getMetaModel()

graph = f.draw(0.0, 10.0)
graph.add(fhat.draw(0.0, 10.0))
graph.add(ot.Cloud(sampleX, sampleY))
graph.setColors(["blue", "red", "black"])

# Conditional variance
gccc = otexp.GaussianProcessConditionalCovariance(result)
# Vriance per marginal & extract variance component
newSampleX = ot.Sample.BuildFromPoint([0.0, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5])
newSampleX.add(sampleX)
newSampleX = newSampleX.sort()
newSampleY = fhat(newSampleX)
conditionalVariance = gccc.getConditionalMarginalVariance(newSampleX) + [1e-15]
conditionalSigma = ot.SymbolicFunction("x", "sqrt(x)")(conditionalVariance)
alpha = 0.1
bilateralCI = ot.Normal().computeBilateralConfidenceInterval(1.0 - alpha)
lowerCI = newSampleY + conditionalSigma * bilateralCI.getLowerBound()
upperCI = newSampleY + conditionalSigma * bilateralCI.getUpperBound()
boundsPoly = ot.Polygon.FillBetween(newSampleX, lowerCI, upperCI)
boundsPoly.setColor("green")
boundsPoly_legend = "%d%% C.I." % ((1.0 - alpha) * 100)
graph.add(boundsPoly)
graph.setLegends(["model", "meta model", "sample", boundsPoly_legend])
graph.setLegendPosition("upper left")
graph.setTitle("y(x)=x * sin(x)")
View(graph, figure_kw={"figsize": (8, 4)})
