import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

sampleX = ot.Normal(2).getSample(100)

f = ot.SymbolicFunction(['x1','x2'],['x1 + x2','2 * x1 - x2'])

sampleY = f(sampleX)

graph = ot.VisualTest.DrawPairsXY(sampleX, sampleY)

View(graph, figure_kw={"figsize": (6.0, 6.0)})
