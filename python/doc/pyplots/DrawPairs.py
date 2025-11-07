import openturns as ot
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

sample = ot.Normal(3).getSample(100)
graph = ot.VisualTest.DrawPairs(sample)

otv.View(graph, figure_kw={"figsize": (6.0, 6.0)})
