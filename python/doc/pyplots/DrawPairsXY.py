import openturns as ot
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

sampleX = ot.Normal(2).getSample(100)

f = ot.SymbolicFunction(["x1", "x2"], ["x1 + x2", "2 * x1 - x2"])

sampleY = f(sampleX)

graph = ot.VisualTest.DrawPairsXY(sampleX, sampleY)

otv.View(graph, figure_kw={"figsize": (6.0, 6.0)})
