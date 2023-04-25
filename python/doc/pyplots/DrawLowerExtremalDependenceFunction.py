import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)
data = ot.FrankCopula().getSample(1000)
graph = ot.VisualTest.DrawLowerExtremalDependenceFunction(data)

View(graph, figure_kw={"figsize": (4.5, 4.5)})
