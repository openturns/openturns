import openturns as ot
import openturns.experimental as otexp
from openturns.viewer import View

# Graph
g = ot.Graph("Fejer quadratures", "nodes", "", True)
g.setIntegerYTick(True)
discretization = [15]

# Fejer type 1
experiment = otexp.FejerExperiment(discretization, otexp.FejerExperiment.FEJERTYPE1)
nodes, weights = experiment.generateWithWeights()
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), [0.0]))
lower.setColor("green")
lower.setPointStyle("circle")
g.add(lower)

# Fejer type 2
experiment = otexp.FejerExperiment(discretization, otexp.FejerExperiment.FEJERTYPE2)
nodes, weights = experiment.generateWithWeights()
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), [1.0]))
lower.setColor("red")
lower.setPointStyle("square")
g.add(lower)

# Clenshaw-Curtis
experiment = otexp.FejerExperiment(discretization, otexp.FejerExperiment.CLENSHAWCURTIS)
nodes, weights = experiment.generateWithWeights()
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), [2.0]))
lower.setColor("magenta")
lower.setPointStyle("plus")
g.add(lower)

g.setTitle(r"Fejer experiment example")

g.setLegends(["Fejer_1", "Fejer_2", "Clenshaw-Curtis"])
g.setLegendPosition("upper right")

View(g, figure_kw={"figsize": (8, 4)}, add_legend=True)
