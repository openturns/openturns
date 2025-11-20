import openturns as ot
import openturns.viewer as otv


size = 100
copula1 = ot.FrankCopula(1.5)
copula2 = ot.GumbelCopula(4.5)
sample1 = copula1.getSample(size)
sample1.setName("data 1")

kendallPlot = ot.VisualTest.DrawKendallPlot(sample1, copula2)

otv.View(kendallPlot, figure_kw={"figsize": (4.5, 4.5)})
