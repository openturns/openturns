import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
from math import sqrt

mesh = ot.IntervalMesher([128]).build(ot.Interval(-1.0, 1.0))
threshold = 0.001
model = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, model, threshold)
algo.run()
ev = algo.getResult().getEigenValues()
modes = algo.getResult().getScaledModesAsProcessSample()
g = modes.drawMarginal(0)
g.setXTitle("$t$")
g.setYTitle("$\sqrt{\lambda_n}\phi_n$")
g.setTitle("P1 approx. of KL expansion for $C(s,t)=e^{-|s-t|}$")

fig = plt.figure(figsize=(6, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=False)
