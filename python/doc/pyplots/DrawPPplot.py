import openturns as ot
import openturns.viewer as otv

size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)

graph = ot.VisualTest.DrawPPplot(sample, normal)

otv.View(graph, figure_kw={"figsize": (4.5, 4.5)})
