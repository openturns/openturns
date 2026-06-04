import openturns as ot
import openturns.viewer as otv

data = ot.FrankCopula().getSample(1000)
graph = ot.VisualTest.DrawLowerTailDependenceFunction(data)

otv.View(graph, figure_kw={"figsize": (4.5, 4.5)})
