import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
from math import sqrt

mesh = ot.IntervalMesher([256]).build(ot.Interval(-1.0, 1.0))
threshold = 0.001
factory = ot.KarhunenLoeveP1Factory(mesh, threshold)
model = ot.AbsoluteExponential([1.0])
ev = ot.NumericalPoint()
modes = factory.buildAsProcessSample(model, ev)
for i in range(modes.getSize()):
    modes[i] = ot.Field(mesh, modes[i].getValues() * [sqrt(ev[i])])
g = modes.drawMarginal(0)
g.setXTitle("$t$")
g.setYTitle("$\sqrt{\lambda_n}\phi_n$")

fig = plt.figure(figsize=(6, 4))
plt.suptitle("P1 approx. of KL expansion for $C(s,t)=e^{-|s-t|}$")
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=False)
