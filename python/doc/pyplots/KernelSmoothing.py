import openturns as ot
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)
sample = ot.Gamma(6.0, 1.0).getSample(100)
ks = ot.KernelSmoothing()
bandwidth = [0.9]
fittedDist = ks.build(sample, bandwidth)

graph = fittedDist.drawPDF()
graph.add(ot.Gamma(6.0, 1.0).drawPDF())
graph.setLegends(["KS dist", "Gamma"])
otv.View(graph)
