import math

import openturns as ot
import openturns.viewer as otv
from openturns.experimental import SparseGaussianProcessRegression

f = ot.SymbolicFunction(["x"], ["x + x * sin(x)"])
sampleX = [[1.0], [2.0], [3.0], [4.5], [6.0], [7.0], [8.0]]
sampleY = f(sampleX)
covarianceModel = ot.SquaredExponential([1.0])
covarianceModel.setActiveParameter([])
inducingPoints = ot.Sample([[1.5], [3.0], [4.5], [6.0], [7.5]])
algo = SparseGaussianProcessRegression(sampleX, sampleY, covarianceModel, inducingPoints)
algo.run()
result = algo.getResult()

metaModel = result.getMetaModel()
graph = ot.Graph("y(x)=x + x*sin(x), 5 inducing points", "x", "y", True, "upper left")
curveMetamodel = metaModel.draw(0.5, 8.5, 200).getDrawable(0)
curveMetamodel.setLineStyle("solid")
curveMetamodel.setLineWidth(2)
graph.add(curveMetamodel)
curveExact = f.draw(0.5, 8.5, 200).getDrawable(0)
curveExact.setLineStyle("dashed")
curveExact.setLineWidth(1.5)
graph.add(curveExact)
# Conditional confidence interval
newSampleX = ot.Sample([[0.5 + 8.0 * i / 199.0] for i in range(200)])
conditionalVariance = result.getConditionalVariance(newSampleX)
conditionalSigma = ot.Sample([[math.sqrt(max(v, 0.0))] for v in conditionalVariance])
alpha = 0.1
bilateralCI = ot.Normal().computeBilateralConfidenceInterval(1.0 - alpha)
newSampleY = metaModel(newSampleX)
lowerCI = newSampleY + conditionalSigma * bilateralCI.getLowerBound()[0]
upperCI = newSampleY + conditionalSigma * bilateralCI.getUpperBound()[0]
boundsPoly = ot.Polygon.FillBetween(newSampleX, lowerCI, upperCI)
boundsPoly.setColor("green")
graph.add(boundsPoly)
cloud = ot.Cloud(sampleX, sampleY)
cloud.setPointStyle("circle")
cloud.setLegend("sample")
graph.add(cloud)
cloudInd = ot.Cloud(result.getInducingPoints(), metaModel(result.getInducingPoints()))
cloudInd.setColor("orange")
cloudInd.setPointStyle("square")
cloudInd.setLegend("inducing points")
graph.add(cloudInd)
graph.setLegends(["sparse GP metamodel", "exact model", "%d%% C.I." % ((1.0 - alpha) * 100), "sample", "inducing points"])
otv.View(graph, figure_kw={"figsize": (8, 4)})
