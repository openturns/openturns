import openturns as ot
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)
distribution_2d = ot.MarshallOlkinCopula(0.3, 0.7)
title = f"Marshall-Olkin copula CDF and sample: alpha = {distribution_2d.getAlpha()}, beta = {distribution_2d.getBeta()}"

graph = ot.Graph(title, "x1", "x2", True)
graph.add(ot.Cloud(distribution_2d.getSample(1000)))
graph.add(distribution_2d.drawCDF([0.0] * 2, [1.0] * 2, [71] * 2))
v = otv.View(graph)
v.ShowAll()
