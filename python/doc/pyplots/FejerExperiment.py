import openturns as ot
import openturns.experimental as otexp
from openturns.viewer import View


f = ot.SymbolicFunction(["x"], ["sin(x)"])
a = -2.5
b = 4.5
# Graph
g = f.draw(a, b, 512)

# Fejer type 1
algo = ot.FejerAlgorithm([20], ot.FejerAlgorithm.FEJERTYPE1)
bounds = ot.Interval(a, b)
experiment = otexp.FejerExperiment(algo, bounds)
nodes, weights = experiment.generateWithWeights()
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), 1))
lower.setColor("green")
lower.setPointStyle("circle")
g.add(lower)

# Fejer type 2
algo = ot.FejerAlgorithm([20], ot.FejerAlgorithm.FEJERTYPE2)
experiment = otexp.FejerExperiment(algo, bounds)
nodes, weights = experiment.generateWithWeights()
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), [-1.0 / 8]))
lower.setColor("red")
lower.setPointStyle("square")
g.add(lower)

# Clenshaw-Curtis
algo = ot.FejerAlgorithm([20], ot.FejerAlgorithm.CLENSHAWCURTIS)
experiment = otexp.FejerExperiment(algo, bounds)
nodes, weights = experiment.generateWithWeights()
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), [1.0 / 8]))
lower.setColor("magenta")
lower.setPointStyle("plus")
g.add(lower)

g.setTitle(r"Fejer experiment example")

g.setLegends(["f", "Fejer_1", "Fejer_2", "Clenshaw-Curtis"])
g.setLegendPosition("upper right")

View(g, figure_kw={"figsize": (8, 4)}, add_legend=True)
