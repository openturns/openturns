import openturns as ot
from openturns.viewer import View

sample = ot.Gamma(6.0, 1.0).getSample(100)
ks = ot.KernelSmoothing()
bandwidth = [0.9]
fittedDist = ks.build(sample, bandwidth)

graph = fittedDist.drawPDF()
graph.add(ot.Gamma(6.0, 1.0).drawPDF())
graph.setLegends(["KS dist", "Gamma"])
View(graph)
