import openturns as ot
from openturns.viewer import View

ot.RandomGenerator.SetSeed(0)

size = 100
copula1 = ot.FrankCopula(1.5)
copula2 = ot.GumbelCopula(4.5)
sample1 = copula1.getSample(size)
sample1.setName('data 1')

kendallPlot = ot.VisualTest_DrawKendallPlot(sample1, copula2)

View(kendallPlot, figure_kwargs={'figsize': (4.5, 4.5)})
