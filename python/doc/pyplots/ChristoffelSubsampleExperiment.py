import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

ot.RandomGenerator.SetSeed(0)
factory = ot.OrthogonalProductPolynomialFactory([ot.Uniform(-1.0, 1.0)])
basis = ot.OrthogonalBasis(factory)
experiment = otexp.ChristoffelSubsampleExperiment(basis, 30)
sample, weights = experiment.generateWithWeights()
nodes = ot.Cloud(sample, ot.Sample(sample.getSize(), [0.0]))
nodes.setColor("blue")
nodes.setPointStyle("circle")
graph = ot.Graph("Christoffel subsample", "x", "")
graph.add(nodes)
graph.setLegends(["subsample"])
graph.setLegendPosition("upper right")
otv.View(graph, figure_kw={"figsize": (8, 2)}, add_legend=True)
