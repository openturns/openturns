import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

sample = ot.Normal(3).getSample(100)
graph = ot.VisualTest.DrawPairs(sample)

View(graph, figure_kwargs={'figsize': (6.0, 6.0)})
