import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

size = 100
normal = ot.Normal(1)
sample = normal.getSample(size)

sampleHist = ot.VisualTest_DrawHistogram(sample)

View(sampleHist, figure_kwargs={'figsize': (6, 4)})
