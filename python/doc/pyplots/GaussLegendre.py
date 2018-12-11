import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


f = ot.SymbolicFunction(['x'], ['sin(x)'])
a = -2.5
b = 4.5
algo = ot.GaussLegendre([20])
value, nodes = algo.integrateWithNodes(f, ot.Interval(a, b))

g = f.draw(a, b, 512)
lower = ot.Cloud(nodes, ot.Sample(nodes.getSize(), 1))
lower.setColor("magenta")
lower.setPointStyle('circle')
g.add(lower)
g.setTitle(
    r"GaussLegendre example: $\int_{-5/2}^{9/2}\sin(t)\,dt=$" + str(value[0]))

fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=False)
