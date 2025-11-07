import openturns as ot
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)

size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)

henryPlot = ot.VisualTest.DrawHenryLine(sample)

otv.View(henryPlot, figure_kw={"figsize": (4.5, 4.5)})
