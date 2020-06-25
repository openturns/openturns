import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

sample = ot.Normal(3).getSample(100)
distribution = ot.ComposedDistribution([ot.HistogramFactory().build(sample.getMarginal(i)) for i in range(3)])
graph = ot.VisualTest.DrawPairsMarginals(sample, distribution)

View(graph, figure_kwargs={'figsize': (6.0, 6.0)})
