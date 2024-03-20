import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)

graph = ot.VisualTest.DrawPPplot(sample, normal)

View(graph, figure_kw={"figsize": (4.5, 4.5)})
