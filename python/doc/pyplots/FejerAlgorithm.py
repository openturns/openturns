import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


f = ot.SymbolicFunction(["x"], ["sin(x)"])
a = -2.5
b = 4.5
# Graph
g = f.draw(a, b, 512)

# Fejer type 1
algo = ot.FejerAlgorithm([20], ot.FejerAlgorithm.FEJERTYPE1)
value, nodes = algo.integrateWithNodes(f, ot.Interval(a, b))
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), 1))
lower.setColor("green")
lower.setPointStyle("circle")
g.add(lower)

# Fejer type 2
algo = ot.FejerAlgorithm([20], ot.FejerAlgorithm.FEJERTYPE2)
value, nodes = algo.integrateWithNodes(f, ot.Interval(a, b))
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), [-1.0 / 8]))
lower.setColor("red")
lower.setPointStyle("square")
g.add(lower)

# Clenshaw-Curtis
algo = ot.FejerAlgorithm([20], ot.FejerAlgorithm.CLENSHAWCURTIS)
value, nodes = algo.integrateWithNodes(f, ot.Interval(a, b))
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), [1.0 / 8]))
lower.setColor("magenta")
lower.setPointStyle("plus")
g.add(lower)

g.setTitle(r"Fejer algorithms example: $\int_{-5/2}^{9/2}\sin(t)\,dt=$" + str(value[0]))

g.setLegends(["f", "Fejer_1", "Fejer_2", "Clenshaw-Curtis"])
g.setLegendPosition("upper right")

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=True)
