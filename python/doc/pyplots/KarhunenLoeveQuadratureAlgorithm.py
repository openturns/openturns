import openturns as ot
from matplotlib import pyplot as plt
from openturns.viewer import View
from math import sqrt

domain = ot.Interval(-1.0, 1.0)
basis = ot.OrthogonalProductFunctionFactory([ot.FourierSeriesFactory()])
coll = [basis.build(i) for i in range(10)]
experiment = ot.GaussProductExperiment(basis.getMeasure(), [20])
mustScale = False
threshold = 0.001
model = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveQuadratureAlgorithm(
    domain, domain, model, experiment, coll, mustScale, threshold)
algo.run()
ev = algo.getResult().getEigenValues()
modes = algo.getResult().getScaledModes()
g = ot.Graph("Quadrature approx. of KL expansion for $C(s,t)=e^{-|s-t|}$")
g.setXTitle("$t$")
g.setYTitle("$\sqrt{\lambda_n}\phi_n$")
for mode in modes:
    g.add(mode.draw(-1.0, 1.0, 256))
g.setColors(ot.Drawable.BuildDefaultPalette(len(modes)))

fig = plt.figure(figsize=(6, 4))
axis = fig.add_subplot(111)
axis.set_xlim(auto=True)
View(g, figure=fig, axes=[axis], add_legend=False)
