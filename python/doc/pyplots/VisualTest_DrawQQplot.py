import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)
size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)

QQPlot = ot.VisualTest.DrawQQplot(sample, normal)

View(QQPlot, figure_kwargs={'figsize': (4.5, 4.5)})
