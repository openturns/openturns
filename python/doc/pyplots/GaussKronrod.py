import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View


f = ot.SymbolicFunction(['x'], ['abs(sin(x))'])
a = -2.5
b = 4.5
algo = ot.GaussKronrod(
    100000, 1e-13, ot.GaussKronrodRule(ot.GaussKronrodRule.G11K23))

value = algo.integrate(f, ot.Interval(a, b))[0]

ai = ot.Point()
bi = ot.Point()
fi = ot.Sample()
ei = ot.Point()
error = ot.Point()
value2 = algo.integrate(f, a, b, error, ai, bi, fi, ei)[0]

ai.add(b)
g = f.draw(a, b, 512)
lower = ot.Cloud(ai, ot.Point(ai.getDimension()))
lower.setColor("magenta")
lower.setPointStyle('circle')
g.add(lower)
g.setTitle(
    r"GaussKronrod example: $\int_{-5/2}^{9/2}|\sin(t)|\,dt=$" + str(value))
fig = plt.figure(figsize=(8, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=False)
